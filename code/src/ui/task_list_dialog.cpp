
#include "onyx/screen/screen_proxy.h"
#include "onyx/ui/task_list_dialog.h"
#include "onyx/ui/keyboard_navigator.h"
#include "onyx/data/data_tags.h"
#include "onyx/screen/screen_update_watcher.h"
#include "onyx/sys/sys_status.h"

static const int ITEM_HEIGHT = 100;
static const QString TITLE_INDEX = "title_index";
static const QString BUTTON_INDEX = "button_index";

const int countPerTask = 3;
const int max = 5;


static const QString BUTTON_STYLE =    "\
QPushButton                             \
{                                       \
    background: transparent;            \
    font-size: 24px;                    \
    border-width: 1px;                  \
    border-color: light;          \
    border-style: solid;                \
    color: black;                       \
    padding: 4px;                       \
    text-align:left;                   \
}                                       \
QPushButton:pressed                     \
{                                       \
    color: white;                       \
    border-color: light;                \
    background-color: black;            \
}                                       \
QPushButton:checked                     \
{                                       \
    color: white;                       \
    border-color: light;                \
    background-color: black;            \
}                                       \
QPushButton:disabled                    \
{                                       \
    border-color: dark;                 \
    color: dark;                        \
    background-color: white;            \
}";


static const QString IMAGE_BUTTON_STYLE =    "\
QPushButton                             \
{                                       \
    background: transparent;            \
    font-size: 24px;                    \
    border-width: 1px;                  \
    border-color: transparent;          \
    border-style: solid;                \
    color: black;                       \
    padding: 0px;                       \
    text-align:left;                   \
}                                       \
QPushButton:pressed                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    color: white;                       \
    border-color: transparent;                \
    background-color: black;            \
}                                       \
QPushButton:checked                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    color: white;                       \
    border-color: transparent;                \
    background-color: black;            \
}                                       \
QPushButton:disabled                    \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    border-color: transparent;                 \
    color: dark;                        \
    background-color: white;            \
}";

namespace ui
{


TaskItem::TaskItem(QWidget *parent)
    : QWidget(parent)
    , hor_layout_(this)
    , image_label_(0)
    , title_button_("", 0)
    , close_button_("" , 0)
    , index_(-1)
{
    createLayout();
}

TaskItem::~TaskItem(void)
{
}

void TaskItem::setImage(const QString & path)
{
    QPixmap pixmap(path);
    image_label_.setPixmap(pixmap);
}

void TaskItem::setTitle(const QString & title)
{
    title_button_.setText(title);
}

void TaskItem::createLayout()
{
    hor_layout_.setSpacing(10);
    hor_layout_.addWidget(&image_label_);

    title_button_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    hor_layout_.addWidget(&title_button_);
    hor_layout_.addWidget(&close_button_);

    title_button_.setStyleSheet(BUTTON_STYLE);
    close_button_.setStyleSheet(IMAGE_BUTTON_STYLE);
    QPixmap close_pixmap(":/images/close.png");
    close_button_.setIconSize(close_pixmap.size());
    close_button_.setIcon(QIcon(close_pixmap));
    close_button_.setFocusPolicy(Qt::NoFocus);
    connect(&close_button_, SIGNAL(clicked()), this, SLOT(onCloseClicked()), Qt::QueuedConnection);
    connect(&title_button_, SIGNAL(clicked()), this, SLOT(onTitleClicked()), Qt::QueuedConnection);
}

void TaskItem::setIndex(int index)
{
    index_ = index;
}

void TaskItem::onTitleClicked()
{
    emit itemClicked(index_);
}

void TaskItem::onCloseClicked()
{
    emit itemClosed(index_);
}

void TaskItem::showCloseButton(bool show)
{
    close_button_.setVisible(show);
}

TaskListDialog::TaskListDialog(QWidget *parent, SysStatus & ref)
    : OnyxDialog(parent)
    , status_(ref)
    , ver_layout_(&content_widget_)
    , battery_power_(0)
{
    setModal(true);
    resize(500, 500);
    createLayout();
    onyx::screen::watcher().addWatcher(this);
}

TaskListDialog::~TaskListDialog(void)
{
}

int TaskListDialog::exec()
{
    shadows_.show(true);
    show();
    onyx::screen::instance().flush();
    onyx::screen::instance().updateWidgetRegion(
        0,
        outbounding(parentWidget()),
        onyx::screen::ScreenProxy::GC,
        false,
        onyx::screen::ScreenCommand::WAIT_ALL);
    return QDialog::exec();
}

void TaskListDialog::keyPressEvent(QKeyEvent *ke)
{
    ke->accept();
}

void TaskListDialog::keyReleaseEvent(QKeyEvent *ke)
{
    ke->accept();
    switch (ke->key())
    {
    case Qt::Key_Up:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Down:
    case Qt::Key_PageDown:
    case Qt::Key_PageUp:
        break;
    case Qt::Key_Return:
        break;
    case Qt::Key_Escape:
        reject();
        onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC);
        break;
    }
}

void TaskListDialog::updateAll()
{
    selected_ = -1;
    all_ = sys::SysStatus::instance().allTasks();
    qDebug() << "all" << all_;
    foreach(TaskItem * item, buttons_)
    {
        ver_layout_.removeWidget(item);
    }
    buttons_.clear();
    for(int i = 0; i <  all_.size() / countPerTask; ++i)
    {
        QString title;
        QFileInfo info(all_.at(i * countPerTask));
        title += info.fileName();
        title += "\n";
        title += all_.at(i * countPerTask + 1);

        TaskItem * item = new TaskItem(this);
        ver_layout_.addWidget(item);
        item->setImage(":/images/bookmark_flag.png");
        item->setTitle(title);
        item->setIndex(i);
        buttons_.push_back(item);
        connect(item, SIGNAL(itemClicked(int)), this, SLOT(onItemClicked(int)), Qt::QueuedConnection);
        connect(item, SIGNAL(itemClosed(int)), this, SLOT(onItemClosed(int)), Qt::QueuedConnection);
    }
}

void TaskListDialog::createLayout()
{
    content_widget_.setBackgroundRole(QPalette::Button);

    // Retrieve the values from system status.
    updateTitle(QApplication::tr("Task list"));
    updateTitleIcon(QPixmap(":/images/power.png"));

    // The big layout.
    ver_layout_.setContentsMargins(SPACING, 0, SPACING, 0);
    ver_layout_.addSpacing(10);


    // Create display items
    updateAll();
    ver_layout_.addStretch(0);
    ver_layout_.addSpacing(8);
}

bool TaskListDialog::event(QEvent* qe)
{
    bool ret = QDialog::event(qe);
    if (qe->type() == QEvent::UpdateRequest
            && onyx::screen::instance().isUpdateEnabled())
    {
         onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::DW);
    }
    return ret;
}

void TaskListDialog::onItemClicked(int index)
{
    if (index >= 0 && index < all_.size() / countPerTask)
    {
        QStringList list;
        for(int i = 0; i < countPerTask; ++i)
        {
            list.push_back(all_.at(index * countPerTask + i));
        }
        sys::SysStatus::instance().activateTask(list);
    }
    onOkClicked();
}

void TaskListDialog::onItemClosed(int index)
{
    if (index >= 0 && index < all_.size() / countPerTask)
    {
        QStringList list;
        for(int i = 0; i < countPerTask; ++i)
        {
            list.push_back(all_.at(index * countPerTask + i));
        }
        sys::SysStatus::instance().closeTask(list);
    }
    updateAll();
}

void TaskListDialog::onOkClicked()
{
    accept();
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC);

}


}   // namespace ui


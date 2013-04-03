
#include "onyx/screen/screen_proxy.h"
#include "onyx/ui/task_list_dialog.h"
#include "onyx/ui/keyboard_navigator.h"
#include "onyx/data/data_tags.h"
#include "onyx/screen/screen_update_watcher.h"
#include "onyx/sys/sys_status.h"

static const int ITEM_HEIGHT = 100;
static const QString TITLE_INDEX = "title_index";
static const QString BUTTON_INDEX = "button_index";

namespace ui
{

TaskListDialog::TaskListDialog(QWidget *parent, SysStatus & ref)
    : OnyxDialog(parent)
    , status_(ref)
    , ver_layout_(&content_widget_)
    , battery_power_(0)
    , hor_layout_(0)
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
    const int countPerTask = 3;
    const int max = 5;
    selected_ = -1;
    all_ = sys::SysStatus::instance().allTasks();
    if (all_.size() < max * countPerTask)
    {
        for(int  i = all_.size(); i < countPerTask * max; ++i)
        {
            all_.push_back("");
        }
    }
    qDebug() << "all" << all_;
    ODatas d;
    for(int i = 0; i <  all_.size() / countPerTask; ++i)
    {
        QString title = all_.at(i * countPerTask);
        title += "\n";
        QFileInfo info(all_.at(i * countPerTask + 1));
        title += info.fileName();

        OData * item = new OData;
        item->insert(TAG_TITLE, title);
        item->insert(BUTTON_INDEX, i);
        d.push_back(item);
    }

    buttons_.setData(d, true);
    buttons_.setMinimumHeight( (ITEM_HEIGHT+2)*d.size());
    buttons_.setFixedGrid(d.size(), 1);
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
    buttons_.setSubItemType(CheckBoxView::type());
    buttons_.setMargin(2, 2, 2, 2);
    buttons_.setPreferItemSize(QSize(0, ITEM_HEIGHT));

    updateAll();

    buttons_.setSpacing(3);
    QObject::connect(&buttons_, SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
                     this, SLOT(onButtonChanged(CatalogView *, ContentView *, int)), Qt::QueuedConnection);

    ver_layout_.addWidget(&buttons_);

    // OK cancel buttons.
    ok_.setSubItemType(ui::CoverView::type());
    ok_.setPreferItemSize(QSize(100, 60));
    ODatas d2;

    OData * item = new OData;
    item->insert(TAG_TITLE, tr("OK"));
    item->insert(TITLE_INDEX, 1);
    d2.push_back(item);


    ok_.setData(d2, true);
    ok_.setMinimumHeight( 60 );
    ok_.setMinimumWidth(100);
    ok_.setFocusPolicy(Qt::TabFocus);
    ok_.setNeighbor(&buttons_, CatalogView::UP);
    connect(&ok_, SIGNAL(itemActivated(CatalogView *, ContentView *, int)), this, SLOT(onOkClicked()));

    hor_layout_.addStretch(0);
    hor_layout_.addWidget(&ok_);


    ver_layout_.addStretch(0);
    ver_layout_.addLayout(&hor_layout_);
    ver_layout_.addSpacing(8);
}

void TaskListDialog::onButtonChanged(CatalogView *catalog, ContentView *item, int user_data)
{
    if (!item || !item->data())
    {
        return;
    }

    OData *selected = item->data();
    selected->insert(TAG_CHECKED, true);

    catalog->update();
    onyx::screen::watcher().enqueue(catalog, onyx::screen::ScreenProxy::GU);
    int i = selected->value(BUTTON_INDEX).toInt();

    onOkClicked();
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

void TaskListDialog::onOkClicked()
{

    accept();
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC);
}


}   // namespace ui


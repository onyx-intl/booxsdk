#include "onyx/ui/password_dialog_with_msg_box.h"
#include "onyx/data/data_tags.h"
#include "onyx/ui/content_view.h"
#include "onyx/screen/screen_update_watcher.h"
#include "onyx/ui/ui_utils.h"
#include "onyx/sys/sys_status.h"
#include "onyx/ui/message_dialog.h"

namespace ui
{

static const int LABEL_WIDTH = 150;
static const int SPACING = 2;

PasswordDialogWithMsgBox::PasswordDialogWithMsgBox(QWidget *parent, const ODatas &ds,
        const QString &title, const QString &default_passwd_label)
    : QDialog(parent, Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint)
    , big_layout_(this)
    , show_plain_text_(0, this)
    , keyboard_(this)
    , title_(title)
    , passwd_edit_("", this)
{
    createLayout();
    connectWithChildren();

    onyx::screen::watcher().addWatcher(this);

    connect(&SysStatus::instance(),
            SIGNAL(connectToPCwithPasswd(const bool)),
            this,
            SLOT(onConnectToPCWithPasswd(const bool)));
}

PasswordDialogWithMsgBox::~PasswordDialogWithMsgBox()
{
    clearDatas(show_plain_text_datas_);
}

void PasswordDialogWithMsgBox::clearState()
{
    passwd_edit_.clear();
    passwd_edit_.setEchoMode(QLineEdit::Password);

    show_plain_text_datas_.clear();
    ODataPtr dd(new OData);
    dd->insert(TAG_TITLE, tr("Show Plain Text"));
    dd->insert(TAG_CHECKED, false);
    show_plain_text_datas_.push_back(dd);
    show_plain_text_.setData(show_plain_text_datas_, true);
}

int PasswordDialogWithMsgBox::popup(const QString &password, PasswordUseType type)
{
    if (isHidden())
    {
        show();
    }

    type_ = type;
    if(ui::isHD())
    {
        QWidget * widget = safeParentWidget(parentWidget());
        resize(widget->width(), height());
        move(widget->x(), widget->height() - height());
    }
    else
    {
        QWidget * widget = safeParentWidget(parentWidget());
        resize(widget->width(), widget->height());
        move(0, 0);
    }

    return exec();
}

int PasswordDialogWithMsgBox::defaultItemHeight()
{
    return 36;
}

void PasswordDialogWithMsgBox::createLayout()
{
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Button);

    QVBoxLayout *message_layout = new QVBoxLayout();

    message_layout->setContentsMargins(33, 50, 35, 50);
    OnyxLabel *info_label = new OnyxLabel(title_, this);
    info_label->setWordWrap(true);
    passwd_edit_.setEchoMode(QLineEdit::Password);
    passwd_edit_.setFixedSize(width()*2/3, 40);

    message_layout->addWidget(info_label);
    message_layout->addWidget(&passwd_edit_);

    OnyxPushButton *cok_button = new OnyxPushButton(tr("OK"), this);
    OnyxPushButton *cancel_button = new OnyxPushButton(tr("Cancel"), this);

    QHBoxLayout *button_layout = new QHBoxLayout();
    button_layout->setContentsMargins(width()*3/5, 0, 30, 10);
    button_layout->addWidget(cok_button);
    button_layout->addSpacing(5);
    button_layout->addWidget(cancel_button);

    connect(cancel_button, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));
    connect(cok_button, SIGNAL(clicked()), this, SLOT(onOKButtonClicked()));

    big_layout_.addLayout(message_layout);
    big_layout_.addSpacing(50);
    big_layout_.addLayout(button_layout);

    big_layout_.setContentsMargins(2, 2, 2, 2);
    big_layout_.setSpacing(0);

    createShowPlainText();

    big_layout_.addWidget(&show_plain_text_);
//    keyboard_.setFixedHeight(326);
    big_layout_.addWidget(&keyboard_);
    update();
}

void PasswordDialogWithMsgBox::onCancelButtonClicked()
{
    hide();
    QDialog::done(QDialog::Rejected);
}

void PasswordDialogWithMsgBox::createShowPlainText()
{
    const int height = defaultItemHeight();
    show_plain_text_.setSubItemType(CheckBoxView::type());
    show_plain_text_.setPreferItemSize(QSize(height, height));

    ODataPtr dd(new OData);
    dd->insert(TAG_TITLE, tr("Show Plain Text"));
    dd->insert(TAG_CHECKED, false);
    show_plain_text_datas_.push_back(dd);

    show_plain_text_.setFixedGrid(1, 1);
    show_plain_text_.setMargin(OnyxKeyboard::CATALOG_MARGIN);
    show_plain_text_.setFixedHeight(defaultItemHeight()+2*SPACING);
    show_plain_text_.setData(show_plain_text_datas_);
}

void PasswordDialogWithMsgBox::connectWithChildren()
{
    connect(&show_plain_text_, SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
            this, SLOT(onItemActivated(CatalogView *, ContentView *, int)));
}

void PasswordDialogWithMsgBox::showPlainTextClicked(bool target_value)
{
    if (target_value)
    {
        passwd_edit_.setEchoMode(QLineEdit::Normal);
    }
    else
    {
        passwd_edit_.setEchoMode(QLineEdit::Password);
    }
    onyx::screen::instance().updateWidget(&passwd_edit_, onyx::screen::ScreenProxy::A2);
}

void PasswordDialogWithMsgBox::paintEvent(QPaintEvent *evnet)
{
    QBrush brush(Qt::white);
    QPainter painter(this);
    QPainterPath path;
    path.addRoundedRect(rect(), 0, 0, Qt::AbsoluteSize);
    painter.fillPath(path, brush);
}

void PasswordDialogWithMsgBox::keyReleaseEvent(QKeyEvent *ke)
{
    if (ke->key() == Qt::Key_Escape)
    {
        onCancelButtonClicked();
    }
}

void PasswordDialogWithMsgBox::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();

    if (Qt::Key_Up != key
            && Qt::Key_Down != key
            && Qt::Key_Left != key
            && Qt::Key_Right != key
            && Qt::Key_Return != key
            && Qt::Key_Enter != key)
    {
        QApplication::sendEvent(&passwd_edit_, event);
    }
}

void PasswordDialogWithMsgBox::onConnectToPCWithPasswd(const bool result)
{
    if(result)
    {
        onCancelButtonClicked();
        onyx::screen::instance().flush();
        SysStatus::instance().workInUSBSlaveMode();
    }
    else
    {
        MessageDialog dialog(QMessageBox::Information,
                             tr("Information"),
                             tr("Please input the correct password!"),
                             QMessageBox::Ok);
        dialog.exec();
        onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::A2);
    }
}

bool PasswordDialogWithMsgBox::validateLength(const QString &input)
{
    int less_lenght = 4;
    if(input.length() >= less_lenght)
    {
        return true;
    }

    onyx::screen::instance().updateWidget(0, onyx::screen::ScreenProxy::GU);
    MessageDialog dialog(QMessageBox::Information,
                         tr("Information"),
                         tr("Password must be at least 4 characters long!"),
                         QMessageBox::Ok);
    dialog.exec();
    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::A2);
    return false;
}

void PasswordDialogWithMsgBox::onOKButtonClicked()
{
    if(validateLength(passwd_edit_.text()))
    {
        if(type_ == USB_CONNECTION_TYPE)
        {
            SysStatus::instance().notifyAboutPasswd(passwd_edit_.text());
        }
        else if(type_ == WIFI_CONNECTION_TYPE || type_ == PASSWORD_GENERIC_TYPE)
        {
            hide();
            QDialog::done(QDialog::Accepted);
        }
    }
}

void PasswordDialogWithMsgBox::onItemActivated(CatalogView *catalog,
                                   ContentView *item,
                                   int user_data)
{
    OData * item_data = item->data();
    if (item_data->contains(TAG_MENU_TYPE))
    {
        int menu_type = item->data()->value(TAG_MENU_TYPE).toInt();
        if(OnyxKeyboard::KEYBOARD_MENU_CLEAR == menu_type)
        {
            update();
            onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::DW);
        }
    }
    else if (catalog == &show_plain_text_)
    {
        bool checked = false;
        ContentView * checkbox = show_plain_text_.visibleSubItems().front();
        if (checkbox->data()->contains(TAG_CHECKED))
        {
            checked = qVariantValue<bool>(checkbox->data()->value(TAG_CHECKED));
        }
        checkbox->data()->insert(TAG_CHECKED, !checked);
        showPlainTextClicked(!checked);
        checkbox->repaint();
        update();
        onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::GC);
    }
}

QString PasswordDialogWithMsgBox::getPasswd()
{
	return passwd_edit_.text();
}

}   // namespace ui

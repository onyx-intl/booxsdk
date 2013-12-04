#include "onyx/ui/onyx_notes_dialog.h"
#include "onyx/screen/screen_update_watcher.h"
#include "onyx/ui/ui_utils.h"
#include "onyx/data/data_tags.h"
#include "onyx/ui/keyboard_data.h"

namespace ui
{

OnyxNotesDialog::OnyxNotesDialog(const QString & text, QWidget *parent)
    : OnyxDialog(parent)
    , big_layout_(&content_widget_)
    , notes_edit_layout_(0)
    , notes_edit_(text, this)
    , sub_menu_(0, this)
    , keyboard_(this)
    , title_(tr("Notes"))
    , to_focus_(0)
{
    createLayout();
    connectWithChildren();
    notes_edit_.installEventFilter(this);
    onyx::screen::watcher().addWatcher(this);
}

OnyxNotesDialog::~OnyxNotesDialog()
{
    clearDatas(sub_menu_datas_);
}

int OnyxNotesDialog::popup(const QString & text)
{
    notes_edit_.setText(text);
    if (isHidden())
    {
        show();
    }
    QRect rect = ui::screenGeometry();
    resize(rect.width(), height());
    move(0, rect.height() - height());
    return exec();
}

QString OnyxNotesDialog::inputText()
{
    return notes_edit_.toPlainText();
}

void OnyxNotesDialog::createNotesEdit()
{
    notes_edit_.setFixedHeight(130);
    notes_edit_.setFocusPolicy(Qt::StrongFocus);
}

void OnyxNotesDialog::createSubMenu()
{
    const int height = defaultItemHeight();
    sub_menu_.setPreferItemSize(QSize(height, height));

    ODataPtr dd(new OData);
    dd->insert(TAG_TITLE, tr("OK"));
    dd->insert(TAG_MENU_TYPE, OnyxKeyboard::KEYBOARD_MENU_OK);
    sub_menu_datas_.push_back(dd);

    ODataPtr b(new OData);
    b->insert(TAG_TITLE, tr("Clear"));
    b->insert(TAG_MENU_TYPE, OnyxKeyboard::KEYBOARD_MENU_CLEAR);
    sub_menu_datas_.push_back(b);

    sub_menu_.setSpacing(2);
    sub_menu_.setFixedGrid(1, 2);
    sub_menu_.setMargin(OnyxKeyboard::CATALOG_MARGIN);
    sub_menu_.setData(sub_menu_datas_);
    sub_menu_.setFixedWidth(defaultItemHeight()*6);
    sub_menu_.setFixedHeight(defaultItemHeight()+4*SPACING);
}

void OnyxNotesDialog::createLayout()
{
    vbox_.setSpacing(0);
    content_widget_.setBackgroundRole(QPalette::Button);
    content_widget_.setContentsMargins(0, 0, 0, 0);
    updateTitle(title_);

    createNotesEdit();
    createSubMenu();

    notes_edit_layout_.setContentsMargins(0, 2, 0, 0);
    notes_edit_layout_.addWidget(&notes_edit_);
    notes_edit_layout_.setSpacing(2);
    notes_edit_layout_.addWidget(&sub_menu_, 0, Qt::AlignTop);

    big_layout_.setContentsMargins(2, 2, 2, 2);
    big_layout_.setSpacing(0);
    big_layout_.addLayout(&notes_edit_layout_);
    big_layout_.addWidget(&keyboard_);

    notes_edit_.setFocusPolicy(Qt::StrongFocus);
}

void OnyxNotesDialog::onItemActivated(CatalogView *catalog,
                                   ContentView *item,
                                   int user_data)
{
    OData * item_data = item->data();
    if (item_data->contains(TAG_MENU_TYPE))
    {
        int menu_type = item->data()->value(TAG_MENU_TYPE).toInt();
        if(OnyxKeyboard::KEYBOARD_MENU_OK == menu_type)
        {
            accept();
        }
        else if(OnyxKeyboard::KEYBOARD_MENU_CLEAR == menu_type)
        {
            clearClicked();
            update();
            onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::DW);
        }
    }
}

void OnyxNotesDialog::onOutOfBoundary(CatalogView* child, int, int)
{
    notes_edit_.setFocus();
}

void OnyxNotesDialog::focusKeyboardTop()
{
    keyboard_.top()->visibleSubItems().back()->setFocus();
}

void OnyxNotesDialog::focusKeyboardMenu()
{
    keyboard_.menu()->visibleSubItems().back()->setFocus();
}

void OnyxNotesDialog::connectWithChildren()
{
    connect(&sub_menu_, SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
            this, SLOT(onItemActivated(CatalogView *, ContentView *, int)));

    connect(&keyboard_, SIGNAL(outOfUp(CatalogView*, int, int)),
            this, SLOT(onOutOfBoundary(CatalogView*, int, int)));
    connect(&keyboard_, SIGNAL(outOfDown(CatalogView*, int, int)),
            this, SLOT(onOutOfBoundary(CatalogView*, int, int)));

    connect(&sub_menu_, SIGNAL(outOfLeft(CatalogView*, int, int)),
            this, SLOT(onOutOfBoundary(CatalogView*, int, int)));
    connect(&sub_menu_, SIGNAL(outOfRight(CatalogView*, int, int)),
            this, SLOT(onOutOfBoundary(CatalogView*, int, int)));

    connect(&sub_menu_, SIGNAL(outOfUp(CatalogView*, int, int)),
            this, SLOT(focusKeyboardMenu()));
    connect(&sub_menu_, SIGNAL(outOfDown(CatalogView*, int, int)),
            this, SLOT(focusKeyboardTop()));
}

void OnyxNotesDialog::clearClicked()
{
    notes_edit_.clear();
}

bool OnyxNotesDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (QEvent::KeyPress == event->type())
    {
        QKeyEvent *key_event = static_cast<QKeyEvent *>(event);
        int key = key_event->key();
        if (obj == &notes_edit_)
        {
            QTextCursor cursor = notes_edit_.textCursor();
            if (Qt::Key_Right == key)
            {
                if (cursor.atBlockEnd())
                {
                    to_focus_ = sub_menu_.visibleSubItems().front();
                    return true;
                }
            }
            else if (Qt::Key_Left == key)
            {
                if (cursor.atBlockStart())
                {
                    to_focus_ = sub_menu_.visibleSubItems().back();
                    return true;
                }
            }
            else if (Qt::Key_Up == key)
            {
                if (0 == cursor.block().previous().length())
                {
                    to_focus_ = keyboard_.menu()->visibleSubItems().front();
                    return true;
                }
            }
            else if (Qt::Key_Down == key)
            {
                if (0 == cursor.block().next().length())
                {
                    to_focus_ = keyboard_.top()->visibleSubItems().front();
                    return true;
                }
            }

            if (Qt::Key_Left == key || Qt::Key_Right == key
                    || Qt::Key_Up == key || Qt::Key_Down == key
                    || Qt::Key_Return == key)
            {
                update();
                onyx::screen::watcher().enqueue(this,
                        onyx::screen::ScreenProxy::DW,
                        onyx::screen::ScreenCommand::WAIT_NONE);
            }
        }
    }
    else if (QEvent::KeyRelease == event->type())
    {
        if (to_focus_)
        {
            to_focus_->setFocus();
            to_focus_ = 0;
            return true;
        }

        QKeyEvent *key_event = static_cast<QKeyEvent *>(event);
        if (key_event->key() == Qt::Key_Escape)
        {
            onCloseClicked();
        }
    }
    to_focus_ = 0;
    return OnyxDialog::eventFilter(obj, event);
}

void OnyxNotesDialog::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (Qt::Key_Up != key
            && Qt::Key_Down != key
            && Qt::Key_Left != key
            && Qt::Key_Right != key
            && Qt::Key_Return != key)
    {
        if (Qt::Key_Enter == key && KeyboardData::ENTER_TEXT != event->text())
        {
            return;
        }

        QApplication::sendEvent(&notes_edit_, event);
        update();
        onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::DW,
                onyx::screen::ScreenCommand::WAIT_NONE);
    }
}

}   // namespace ui

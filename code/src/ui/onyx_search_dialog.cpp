#include "onyx/ui/onyx_search_dialog.h"
#include "onyx/screen/screen_update_watcher.h"
#include "onyx/ui/ui_utils.h"
#include "onyx/ui/onyx_keyboard_utils.h"
#include "onyx/data/data_tags.h"
#include "onyx/ui/ui_utils.h"
#include "onyx/sys/platform.h"


static const int MODE_FULL      = 0;
static const int MODE_NEXT_PREV = 1;
static const int MODE_SEARCHING = 2;

namespace ui
{


OnyxSearchDialog::OnyxSearchDialog(QWidget *parent, OnyxSearchContext & ctx, bool adjust_postion)
    : OnyxDialog(parent)
    , big_layout_(&content_widget_)
    , line_edit_layout_(0)
    , line_edit_(0, this)
    , sub_menu_(0, this)
    , keyboard_(this)
    , next_prev_(0, this)
    , ctx_(ctx)
    , mode_(MODE_FULL)
    , adjust_postion_(adjust_postion)
{
    createLayout();
    connectWithChildren();
}

OnyxSearchDialog::~OnyxSearchDialog()
{
    clearDatas(line_edit_datas_);
    clearDatas(sub_menu_datas_);
    clearDatas(next_prev_datas_);
}

void OnyxSearchDialog::adjustSizeAndPosition()
{
    // Change size at first.
    int x = 0, y = 0;
    QWidget *parent = safeParentWidget(parentWidget());
    QRect parent_rect = parent->rect();
    if (mode() == MODE_FULL)
    {
        setFixedSize(parent_rect.width(), minimumHeight());
    }
    else if (mode() == MODE_NEXT_PREV)
    {
        setFixedSize(parent_rect.width(), defaultItemHeight() * 2 + 2 *SPACING);
    }
    else if (mode() == MODE_SEARCHING)
    {
        setFixedSize(parent_rect.width(), defaultItemHeight() * 2 + 2 *SPACING);
    }
    y = parent->height() - height();
    if (sys::isIRTouch() && adjust_postion_)
    {
        y -= ui::statusBarHeight();
    }
    move(x, y);
}

void OnyxSearchDialog::setMode(int m)
{
    mode_ = m;
}

/// This function is called by parent widget to display the search widget.
/// It shows line edit and keyboard.
void OnyxSearchDialog::showNormal()
{
    setMode(MODE_FULL);

    if (isHidden())
    {
        show();
    }

    updateChildrenWidgets(mode());
    adjustSizeAndPosition();

    // Set line edit view each time after updating the children and resize.
    if (editor()->text().isEmpty())
    {
        editor()->setText(ctx_.pattern());
    }
    updateTitle();
}

/// Show next previous buttons and title bar.
void OnyxSearchDialog::showNextPrev()
{
    setMode(MODE_NEXT_PREV);

    if (isHidden())
    {
        show();
    }

    updateChildrenWidgets(mode());
    adjustSizeAndPosition();
    updateTitle();
    if (ctx_.forward())
    {
        next_prev_.setFocusTo(0, 1);
    }
    else
    {
        next_prev_.setFocusTo(0, 0);
    }
}

/// Show dialog in simple way.
void OnyxSearchDialog::showSearching()
{
    setMode(MODE_SEARCHING);

    if (isHidden())
    {
        show();
    }

    updateChildrenWidgets(mode());
    adjustSizeAndPosition();
    updateTitle();
}

void OnyxSearchDialog::createLineEdit()
{
    line_edit_.setSubItemType(LineEditView::type());
    line_edit_.setPreferItemSize(QSize(rect().width(), defaultItemHeight()));

    ODataPtr dd( new OData);
    dd->insert(TAG_TITLE, "");
    line_edit_datas_.push_back(dd);

    line_edit_.setFixedGrid(1, 1);
    line_edit_.setFixedHeight(defaultItemHeight()+2*SPACING);
    line_edit_.setMargin(OnyxKeyboard::CATALOG_MARGIN);
    line_edit_.setData(line_edit_datas_);
    line_edit_.setNeighbor(keyboard_.top(), CatalogView::DOWN);
    line_edit_.setNeighbor(keyboard_.menu(), CatalogView::RECYCLE_DOWN);
    line_edit_.setNeighbor(&sub_menu_, CatalogView::RIGHT);
    line_edit_.setNeighbor(&sub_menu_, CatalogView::RECYCLE_RIGHT);
}

void OnyxSearchDialog::createSubMenu()
{
    const int height = defaultItemHeight();
    sub_menu_.setPreferItemSize(QSize(height, height));

    ODataPtr dd( new OData);
    dd->insert(TAG_TITLE, tr("Search"));
    dd->insert(TAG_MENU_TYPE, OnyxKeyboard::KEYBOARD_MENU_OK);
    sub_menu_datas_.push_back(dd);

    ODataPtr d( new OData);
    d->insert(TAG_TITLE, tr("Clear"));
    d->insert(TAG_MENU_TYPE, OnyxKeyboard::KEYBOARD_MENU_CLEAR);
    sub_menu_datas_.push_back(d);

    sub_menu_.setSpacing(2);
    sub_menu_.setFixedGrid(1, 2);
    sub_menu_.setMargin(OnyxKeyboard::CATALOG_MARGIN);
    sub_menu_.setFixedHeight(defaultItemHeight()+2*SPACING);
    sub_menu_.setFixedWidth(defaultItemHeight()*6);
    sub_menu_.setData(sub_menu_datas_);
    sub_menu_.setNeighbor(&line_edit_, CatalogView::RECYCLE_LEFT);
    sub_menu_.setNeighbor(keyboard_.top(), CatalogView::DOWN);
    sub_menu_.setNeighbor(keyboard_.menu(), CatalogView::RECYCLE_DOWN);
}

void OnyxSearchDialog::createNavigateMenu()
{
    const int height = defaultItemHeight();
    next_prev_.setPreferItemSize(QSize(height, height));

    ODataPtr dd( new OData);
    dd->insert(TAG_TITLE, tr("Previous"));
    dd->insert(TAG_MENU_TYPE, SEARCH_NAV_PREVIOUS);
    next_prev_datas_.push_back(dd);

    ODataPtr b( new OData);
    b->insert(TAG_TITLE, tr("Next"));
    b->insert(TAG_MENU_TYPE, SEARCH_NAV_NEXT);
    next_prev_datas_.push_back(b);

    next_prev_.setSpacing(2);
    next_prev_.setFixedGrid(1, 2);
    next_prev_.setMargin(OnyxKeyboard::CATALOG_MARGIN);
    next_prev_.setFixedHeight(defaultItemHeight()+2*SPACING);

    int half_width = safeParentWidget(parentWidget())->width()/2;
    int min = defaultItemHeight()*8;
    if (half_width < min)
    {
        next_prev_.setFixedWidth(min);
    }
    else
    {
        next_prev_.setFixedWidth(half_width);
    }

    next_prev_.setData(next_prev_datas_);
    next_prev_.setSearchPolicy(CatalogView::NeighborFirst
            | CatalogView::AutoHorRecycle);
}

void OnyxSearchDialog::createLayout()
{
    vbox_.setSpacing(0);
    updateTitleIcon(QPixmap(":/images/search.png"));
    content_widget_.setBackgroundRole(QPalette::Button);
    content_widget_.setContentsMargins(0, 0, 0, 0);

    createLineEdit();
    createSubMenu();
    createNavigateMenu();

    line_edit_layout_.setContentsMargins(0, 2, 0, 0);
    line_edit_layout_.setSpacing(2);
    line_edit_layout_.addWidget(&line_edit_);
    line_edit_layout_.addWidget(&sub_menu_);
    line_edit_layout_.addWidget(&next_prev_);

    big_layout_.setContentsMargins(2, 2, 2, 2);
    big_layout_.setSpacing(0);
    big_layout_.addLayout(&line_edit_layout_, 0);
    big_layout_.addWidget(&keyboard_);
}

void OnyxSearchDialog::onItemActivated(CatalogView *catalog,
                                   ContentView *item,
                                   int user_data)
{
    OData * item_data = item->data();
    if (item_data->contains(TAG_MENU_TYPE))
    {
        int menu_type = item->data()->value(TAG_MENU_TYPE).toInt();
        if(OnyxKeyboard::KEYBOARD_MENU_OK == menu_type)
        {
            onSearchClicked();
        }
        else if(OnyxKeyboard::KEYBOARD_MENU_CLEAR == menu_type)
        {
            clearClicked();
            update();
            onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::DW);
        }
        else if (SEARCH_NAV_PREVIOUS == menu_type)
        {
            onSearchPrevClicked();
        }
        else if (SEARCH_NAV_NEXT == menu_type)
        {
            onSearchNextClicked();
        }
    }
}

void OnyxSearchDialog::connectWithChildren()
{
    connect(&line_edit_, SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
            this, SLOT(onItemActivated(CatalogView *, ContentView *, int)));
    connect(&sub_menu_, SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
            this, SLOT(onItemActivated(CatalogView *, ContentView *, int)));
    connect(&next_prev_, SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
            this, SLOT(onItemActivated(CatalogView *, ContentView *, int)));
}

OnyxLineEdit *OnyxSearchDialog::editor()
{
    LineEditView *view = static_cast<LineEditView *>(line_edit_.visibleSubItems().front());
    OnyxLineEdit *edit = view->innerEdit();
    qDebug() << "line edit text" << edit->text();
    return edit;
}

void OnyxSearchDialog::clearClicked()
{
    editor()->clear();
}

void OnyxSearchDialog::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (Qt::Key_Up != key
            && Qt::Key_Down != key
            && Qt::Key_Left != key
            && Qt::Key_Right != key)
    {
        QApplication::sendEvent(line_edit_.visibleSubItems().front(), event);
    }
}

void OnyxSearchDialog::keyReleaseEvent(QKeyEvent *ke)
{
    int key = ke->key();
    if (key == Qt::Key_Escape || key == ui::Device_Menu_Key)
    {
        onCloseClicked();
        return;
    }
}

void OnyxSearchDialog::readyToSearch(bool forward)
{
    ctx_.setForward(forward);
    emit search(ctx_);
}

void OnyxSearchDialog::updateTitle(const QString &message)
{
    if (message.isEmpty())
    {
        if (mode() == MODE_FULL)
        {
            OnyxDialog::updateTitle(tr("Search"));
        }
        else if (mode() == MODE_SEARCHING)
        {
            OnyxDialog::updateTitle(tr("Search...") + " " + ctx_.pattern());
        }
    }
    else
    {
        OnyxDialog::updateTitle(message);
    }
}

/// This function is called when caller can not find any matched
/// result.
void OnyxSearchDialog::noMoreMatches()
{
    updateTitle(tr("No more matches"));
    update();
    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::GC);
}

/// Update all widget according to the specified parameter.
void OnyxSearchDialog::updateChildrenWidgets(int mode)
{
    if (mode == MODE_NEXT_PREV)
    {
        next_prev_.show();
        line_edit_.hide();
        sub_menu_.hide();
        keyboard_.hide();
    }
    else if (mode == MODE_FULL)
    {
        next_prev_.hide();
        line_edit_.show();
        sub_menu_.show();
        keyboard_.show();
    }
    else if (mode == MODE_SEARCHING)
    {
        next_prev_.hide();
        line_edit_.show();
        sub_menu_.show();
        keyboard_.hide();
    }
    adjustSize();
}

void OnyxSearchDialog::onSearchClicked()
{
    if (editor()->text().isEmpty())
    {
        return;
    }

    ctx_.setPattern(editor()->text());
    if (!ctx_.searchAll())
    {
        showNextPrev();
        onyx::screen::watcher().enqueue(safeParentWidget(parentWidget()), onyx::screen::ScreenProxy::GU);
        readyToSearch(ctx_.forward());
    }
    else
    {
        showSearching();
        onyx::screen::watcher().enqueue(safeParentWidget(parentWidget()), onyx::screen::ScreenProxy::GU);
        readyToSearch(ctx_.forward());
    }
}

void OnyxSearchDialog::onSearchNextClicked()
{
    setMode(MODE_SEARCHING);
    updateTitle();
    readyToSearch(true);
}

void OnyxSearchDialog::onSearchPrevClicked()
{
    setMode(MODE_SEARCHING);
    updateTitle();
    readyToSearch(false);
}

void OnyxSearchDialog::moveEvent(QMoveEvent *e)
{
    OnyxDialog::moveEvent(e);
    update();
    onyx::screen::watcher().enqueue(safeParentWidget(parentWidget()),
            onyx::screen::ScreenProxy::GC);
}

void OnyxSearchDialog::onCloseClicked()
{
    // Make sure caller stop searching.
    ctx_.stop(true);
    done(QDialog::Rejected);
    emit closeClicked();
}

}   // namespace ui

#include "onyx/dictionary/dict_widget.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/ui/ui_utils.h"
#include "onyx/sys/sys.h"

namespace ui
{
static const int LOOKUP = 0;
static const int EXPLANATION = 1;
static const int SIMILAR_WORDS = 2;
static const int DICTIONARY_LIST = 3;
static const int RETRIEVING_WORD = 4;
static const int OPEN_DICTIONARY_TOOL = 5;
static const int ADJUST_POSITION = 6;
static const char* SCOPE = "dict_widget";
static bool s_enable_update = true;

/// Define all descriptions
const DictWidget::FunctionDescription DictWidget::DICT_FUNC_DESCRIPTION[] =
{
    { QT_TRANSLATE_NOOP("dict_widget", "Retrieve Words"), RETRIEVING_WORD},
    { QT_TRANSLATE_NOOP("dict_widget", "Explanation"), EXPLANATION},
    { QT_TRANSLATE_NOOP("dict_widget", "Similar Words"), SIMILAR_WORDS},
    { QT_TRANSLATE_NOOP("dict_widget", "Dictionary List"), DICTIONARY_LIST},
    { QT_TRANSLATE_NOOP("dict_widget", "Full screen"), OPEN_DICTIONARY_TOOL},
    { QT_TRANSLATE_NOOP("dict_widget", "Adjust Position"), ADJUST_POSITION},
};
const int DictWidget::DESCRIPTION_COUNT = sizeof(DictWidget::DICT_FUNC_DESCRIPTION)/
        sizeof(DictWidget::DICT_FUNC_DESCRIPTION[0]);


DictWidget::DictWidget(QWidget *parent,
                       DictionaryManager & dict,
                       tts::TTS *tts,
                       bool enable_full_screen)
    : OnyxDialog(parent, false)
    , dict_(dict)
    , tts_(tts)
    , big_vbox_(&content_widget_)
    , top_hbox_(0)
    , content_vbox_(0)
    , func_description_label_(qApp->translate(SCOPE,
            DICT_FUNC_DESCRIPTION[1].description), 0)
    , retrieve_words_button_(QIcon(":/images/retrieve_words.png"), 0)
    , explanation_button_(QIcon(":/images/explanation.png"), 0)
    , similar_words_button_(QIcon(":/images/similar_words.png"), 0)
    , dictionaries_button_(QIcon(":/images/dictionary_list.png"), 0)
    , position_button_(QIcon(":/images/navigation.png"), 0)
    , open_dictionary_tool_button_(QIcon(":/images/open_dictionary_tool.png"), 0)
    , close_button_(QIcon(":/images/close.png"), 0)
    , button_group_(0)
    , explanation_text_(0)
    , similar_words_view_(0, 0)
    , similar_words_offset_(0)
    , timer_(this)
    , internal_state_(-1)
    , update_parent_(false)
    , enable_full_screen_(enable_full_screen)
{
    QPalette pal(parent->palette());
    pal.setColor(QPalette::Background, Qt::darkGray);
    setPalette(pal);

    createLayout();
    initBrowser();
    initDictionaries();

    connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect(&explanation_text_, SIGNAL(highlighted(QString)), this, SLOT(lookup(QString)));
}

DictWidget::~DictWidget()
{
}

/// Ensure the region of selected word is visible. The region
/// is specified by the rectangle.
bool DictWidget::ensureVisible(const QRectF & rect,
                               bool update_parent)
{
    bool changed = false;
    if (!isVisible())
    {
        changed = true;
        show();
    }

    QRect parent_rect = parentWidget()->rect();
    int border = (frameGeometry().width() - geometry().width());
    if (border == 0)
    {
        border = Shadows::PIXELS;
    }
    int width = parent_rect.width();
    if (size().width() != width)
    {
        changed = true;
        resize(width, height());
    }

    // Check position.
    QPoint new_pos(0, 0);
    if (rect.bottom() < parent_rect.height() / 2)
    {
        new_pos.ry() = parent_rect.height() - height();
    }

    if (pos() != new_pos)
    {
        changed = true;
        move(new_pos);
    }

    update_parent_ = false;
    if (changed || update_parent)
    {
        update_parent_ = true;
    }
    return changed;
}

void DictWidget::formatResult(QString &result, QString &fuzzy_word)
{
    if (result.isEmpty())
    {
        result = tr("Not Found In Dictionary.");
    }
    if (!result.contains("<html>", Qt::CaseInsensitive))
    {
        result.replace("\n", "<br>");
    }
}

bool DictWidget::lookup(const QString &word)
{
    if (word.isEmpty() || word_ == word.trimmed())
    {
        return false;
    }

    resetSimilarWordsOffset();

    // Clean the word.
    word_ = word.trimmed();

    // Title
    QString result;
    QString fuzzy_word("");
    bool ret = dict_.fuzzyTranslate(word_, result, fuzzy_word);

    formatResult(result, fuzzy_word);

    // Result
    doc_.setHtml(result);

    // Always stop timer as the screen can be updated later.
    launchTimer(false);

    update_parent_ = false;
    updateVisibleWidgets();
    onDetailsClicked(true);
    return ret;
}

bool DictWidget::lookup(const QString &word, bool update_screen)
{
    if (word.isEmpty() || word_ == word.trimmed())
    {
        return false;
    }

    resetSimilarWordsOffset();

    // Clean the word.
    word_ = word.trimmed();

    // Title
    QString result;
    QString fuzzy_word("");
    bool ret = dict_.fuzzyTranslate(word_, result, fuzzy_word);

    formatResult(result, fuzzy_word);

    // Result
    doc_.setHtml(result);

    // Always stop timer as the screen can be updated later.
    launchTimer(false);

    updateVisibleWidgets();

    changeInternalState(EXPLANATION);
    checkSelectedButton();

    explanation_text_.show();
    explanation_text_.setFocus();
    similar_words_view_.hide();
    s_enable_update = update_screen;
    return ret;
}

int DictWidget::getPreviousFocusButtonId(const int current_checked)
{
    int array_index = 0;
    for (int i = 0; i < DESCRIPTION_COUNT; i++)
    {
        if (current_checked == DICT_FUNC_DESCRIPTION[i].index)
        {
            array_index = i;
            break;
        }
    }
    array_index--;
    if (array_index < 0)
    {
        array_index = DESCRIPTION_COUNT-1;
    }
    return DICT_FUNC_DESCRIPTION[array_index].index;
}

int DictWidget::getNextFocusButtonId(const int current_checked)
{
    int array_index = 0;
    for (int i=0; i < DESCRIPTION_COUNT; i++)
    {
        if (current_checked == DICT_FUNC_DESCRIPTION[i].index)
        {
            array_index = i;
            break;
        }
    }
    array_index++;
    if (array_index == DESCRIPTION_COUNT)
    {
        array_index = 0;
    }
    return DICT_FUNC_DESCRIPTION[array_index].index;
}

bool DictWidget::handleLeftRightKey(const int checked_id, const int key)
{

    QAbstractButton *button = button_group_.button(checked_id);
    if (button)
    {
        int target_focus_id = getNextFocusButtonId(checked_id);
        if (Qt::Key_Left == key)
        {
            target_focus_id = getPreviousFocusButtonId(checked_id);
        }
        QAbstractButton *next = button_group_.button(target_focus_id);
        if (next)
        {
            next->setFocus();
            changeDescription(target_focus_id);
        }
        return true;
    }
    else
    {
        return false;
    }
}

void DictWidget::changeDescription(const int button_id)
{
    QString description(qApp->translate(SCOPE, DICT_FUNC_DESCRIPTION[1].description));
    for (int i = 0; i < DESCRIPTION_COUNT; i++)
    {
        if (button_id == DICT_FUNC_DESCRIPTION[i].index)
        {
            description = qApp->translate(SCOPE, DICT_FUNC_DESCRIPTION[i].description);
            break;
        }
    }
    func_description_label_.setText(description);
}

void DictWidget::keyReleaseEvent(QKeyEvent *ke)
{
    QWidget * wnd = 0;
    QPushButton * btn = 0;
    int key = ke->key();
    if (key == Qt::Key_Escape || key == ui::Device_Menu_Key)
    {
        ke->accept();
        onCloseClicked();
        return;
    }

    if (internalState() == LOOKUP)
    {
        emit keyReleaseSignal(ke->key());
        return;
    }

    switch (ke->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Right:
        if (internalState() != RETRIEVING_WORD)
        {
            wnd = content_widget_.focusWidget();
            btn = qobject_cast<QPushButton*>(wnd);
            const int button_id = button_group_.id(btn);
            if (-1 == button_id)
            {
                int checked_id = button_group_.checkedId();
                handleLeftRightKey(checked_id, key);
            }
            else
            {
                wnd = ui::moveFocus(this, key);
                if (wnd)
                {
                    wnd->setFocus();
                    QPushButton *tmpButton = qobject_cast<QPushButton*>(wnd);
                    const int focus_id = button_group_.id(tmpButton);
                    changeDescription(focus_id);
                }
            }
            ke->accept();
        } else
        {
            emit keyReleaseSignal(ke->key());
        }
        return;
    case Qt::Key_Up:
    case Qt::Key_Down:
        if (internalState() != RETRIEVING_WORD)
        {
            wnd = ui::moveFocus(this, key);
            if (wnd)
            {
                wnd->setFocus();
            }
            ke->accept();
        }
        else
        {
            emit keyReleaseSignal(ke->key());
        }
        return;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if (internalState() == RETRIEVING_WORD)
        {
            emit keyReleaseSignal(ke->key());
        }
        return ;
    default:
        break;
    }
    ke->ignore();
    emit keyReleaseSignal(ke->key());
}

/// The keyPressEvent could be sent from virtual keyboard.
void DictWidget::keyPressEvent(QKeyEvent * ke)
{

    if (ke->key() == Qt::Key_Enter)
    {
        ke->accept();
        // lookup(text_edit_.text());
        return;
    }
    else if (ke->key() == Qt::Key_Shift || ke->key() == Qt::Key_CapsLock)
    {
        ke->accept();
        return;
    }
    else if (ke->key() == Qt::Key_Return)
    {
        ke->accept();
        QWidget * wnd = content_widget_.focusWidget();
        QPushButton * btn = qobject_cast<QPushButton*>(wnd);
        if (btn != 0)
        {
            // btn->click();
        }
    }
    ke->ignore();

    // Launch timer to make sure screen will be updated
    // launchTimer(true);
}

void DictWidget::launchTimer(bool launch)
{
    if (timer_.isActive())
    {
        timer_.stop();
    }

    if (launch)
    {
        timer_.start(1000);
    }
}

void DictWidget::updateSimilarWordsModel(int count)
{
    // Pick up similar words from current dictionary.
    similar_words_.clear();
    dict_.similarWords(word_, similar_words_, similar_words_offset_, count);

    QString result;
    QString fuzzy_word("");
    if(similar_words_.isEmpty())
    {
        dict_.fuzzyTranslate(word_, result, fuzzy_word);
        result.replace("<k>", "");
        int index = result.indexOf("</k>");
        result = result.remove(index, result.length());

        similar_words_.clear();
        dict_.similarWords(result, similar_words_, similar_words_offset_, count);
    }

    similar_words_model_.clear();
    QStandardItem *parentItem = similar_words_model_.invisibleRootItem();
    QString explanation;
    foreach(QString item, similar_words_)
    {
        // Get explanation.
        dict_.translate(item, explanation);
        QStandardItem *ptr = new QStandardItem();
        ptr->setData(item);

        item = item.trimmed();
        explanation = explanation.trimmed();
        item += "\t";
        item += explanation;

        // replace any tags in the similar word item
        item.replace(QRegExp("<[/a-zA-Z]*>"), " ");

        ptr->setText(item);
        parentItem->appendRow(ptr);
    }
}

void DictWidget::updateSimilarWords()
{
    similar_words_view_.clear();
    similar_words_view_.setFocus();
    similar_words_view_.show();
    updateSimilarWordsModel(similar_words_view_.itemsPerPage());

    // Update the list.
    similar_words_view_.setModel(&similar_words_model_);
}

void DictWidget::updateDictionaryListModel()
{
    int count = dict_list_model_.rowCount(dict_list_model_.invisibleRootItem()->index());
    if (count <= 0)
    {
        dict_list_model_.clear();
        QStringList list;
        dict_.dictionaries(list);
        foreach(QString item, list)
        {
            dict_list_model_.appendRow(new QStandardItem(item));
        }
    }
}

void DictWidget::updateDictionaryList()
{
    resetSimilarWordsOffset();
    updateDictionaryListModel();
    similar_words_view_.clear();
    similar_words_view_.setModel(&dict_list_model_);
    similar_words_view_.show();
    similar_words_view_.select(dict_.selected());
    similar_words_view_.setFocus();
}

void DictWidget::updateVisibleWidgets()
{
    if (similar_words_button_.isChecked())
    {
        updateSimilarWords();
    }
    else if (dictionaries_button_.isChecked())
    {
        updateDictionaryList();
    }
}

bool DictWidget::eventFilter(QObject *obj, QEvent *event)
{
    bool ret = QDialog::eventFilter(obj, event);
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Up
                || keyEvent->key() == Qt::Key_Down)
        {
            if (internalState() != RETRIEVING_WORD)
            {
                // do not filter this event because of the design of
                // OnyxTextBrowser (keyPressEvent and keyReleaseEvent)
                return false;
            }
        }
        event->ignore();
        return true;
    }
    else if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Escape)
        {
            keyEvent->ignore();
            return true;
        }
        else if (keyEvent->key() == Qt::Key_PageDown ||
                 keyEvent->key() == Qt::Key_PageUp)
        {
            keyEvent->ignore();
            return true;
        }
        else if (keyEvent->key() == Qt::Key_Left
                || keyEvent->key() == Qt::Key_Right)
        {
            if (internalState() != RETRIEVING_WORD)
            {
                keyEvent->ignore();
                return true;
            }
        }
    }
    return ret;
}

void DictWidget::changeInternalState(int state)
{
    internal_state_ = state;
    changeDescription(internal_state_);
}

bool DictWidget::event(QEvent *e)
{
    int ret = QDialog::event(e);
    if (e->type() == QEvent::UpdateRequest || e->type() == QEvent::Hide)
    {
        if (s_enable_update)
        {
            if (update_parent_)
            {
                onyx::screen::instance().updateWidget(0, onyx::screen::ScreenProxy::GC, true);
                update_parent_ = false;
            }
            else
            {
                onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU, false, onyx::screen::ScreenCommand::WAIT_NONE);
            }
            launchTimer(false);
            e->accept();
        }
        s_enable_update = true;
    }

    return ret;
}

void DictWidget::moveEvent(QMoveEvent *e)
{
    update_parent_ = true;
}

void DictWidget::resizeEvent(QResizeEvent *e)
{
    update_parent_ = true;
}

void DictWidget::hideEvent(QHideEvent * event)
{
    QDialog::hideEvent(event);
}

void DictWidget::createLayout()
{
    // remove the title bar from the OnyxDialog
    this->vbox_.removeWidget(&this->title_widget_);

    big_vbox_.setContentsMargins(SPACING, SPACING, SPACING, SPACING);
    big_vbox_.setSpacing(SPACING);
    big_vbox_.addLayout(&top_hbox_);
    big_vbox_.addLayout(&content_vbox_);

    func_description_label_.setAlignment(Qt::AlignVCenter);
    func_description_label_.setFixedHeight(defaultItemHeight()-SPACING*2);
    func_description_label_.setFixedHeight(defaultItemHeight()-SPACING*2);

    top_hbox_.setContentsMargins(0, 0, SPACING*2, 0);
    top_hbox_.setSpacing(SPACING);
    top_hbox_.addWidget(&retrieve_words_button_);
    top_hbox_.addWidget(&explanation_button_);
    top_hbox_.addWidget(&similar_words_button_);
    top_hbox_.addWidget(&dictionaries_button_);
    top_hbox_.addWidget(&open_dictionary_tool_button_);
    top_hbox_.addWidget(&position_button_);

    top_hbox_.addWidget(&func_description_label_, 0, Qt::AlignRight);

    const int WIDTH = 60;
    retrieve_words_button_.setFixedWidth(WIDTH);
    explanation_button_.setFixedWidth(WIDTH);
    similar_words_button_.setFixedWidth(WIDTH);
    dictionaries_button_.setFixedWidth(WIDTH);
    position_button_.setFixedWidth(WIDTH);

    retrieve_words_button_.useDefaultHeight();
    explanation_button_.useDefaultHeight();
    similar_words_button_.useDefaultHeight();
    dictionaries_button_.useDefaultHeight();
    position_button_.useDefaultHeight();
    open_dictionary_tool_button_.useDefaultHeight();

    button_group_.setExclusive(true);
    button_group_.addButton(&retrieve_words_button_, RETRIEVING_WORD);
    button_group_.addButton(&explanation_button_, EXPLANATION);
    button_group_.addButton(&similar_words_button_, SIMILAR_WORDS);
    button_group_.addButton(&dictionaries_button_, DICTIONARY_LIST);
    button_group_.addButton(&position_button_, ADJUST_POSITION);
    button_group_.addButton(&open_dictionary_tool_button_, OPEN_DICTIONARY_TOOL);
    if (!enable_full_screen_)
    {
        open_dictionary_tool_button_.setVisible(false);
    }

    content_vbox_.setContentsMargins(SPACING, SPACING, SPACING, SPACING);
    content_vbox_.setSpacing(SPACING);
    content_vbox_.addWidget(&explanation_text_);
    content_vbox_.addWidget(&similar_words_view_);
    similar_words_view_.showHeader(false);
    similar_words_view_.hide();

    // Setup connection.
    connect(&retrieve_words_button_, SIGNAL(clicked(bool)), this,
            SLOT(onRetrieveWordClicked(bool)), Qt::QueuedConnection);

    connect(&explanation_button_, SIGNAL(clicked(bool)), this,
        SLOT(onDetailsClicked(bool)), Qt::QueuedConnection);

    connect(&similar_words_button_, SIGNAL(clicked(bool)), this,
            SLOT(onWordListClicked(bool)), Qt::QueuedConnection);

    connect(&dictionaries_button_, SIGNAL(clicked(bool)), this,
            SLOT(onDictListClicked(bool)), Qt::QueuedConnection);

    connect(&position_button_, SIGNAL(clicked(bool)), this,
        SLOT(onPositionClicked(bool)), Qt::QueuedConnection);

    connect(&open_dictionary_tool_button_, SIGNAL(clicked(bool)), this,
                SLOT(onOpenDictionaryToolClicked(bool)), Qt::QueuedConnection);

    connect(&similar_words_view_, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onItemClicked(const QModelIndex &)));
    connect(&similar_words_view_, SIGNAL(exceed(bool)),
            this, SLOT(moreSimilarWords(bool)));

    // Change buttons attributes.
    retrieve_words_button_.setFocusPolicy(Qt::StrongFocus);
    explanation_button_.setFocusPolicy(Qt::StrongFocus);
    similar_words_button_.setFocusPolicy(Qt::StrongFocus);
    dictionaries_button_.setFocusPolicy(Qt::StrongFocus);
    open_dictionary_tool_button_.setFocusPolicy(Qt::StrongFocus);

    explanation_text_.setFocusPolicy(Qt::StrongFocus);
    similar_words_view_.setFocusPolicy(Qt::StrongFocus);

    retrieve_words_button_.setCheckable(true);
    explanation_button_.setCheckable(true);
    similar_words_button_.setCheckable(true);
    dictionaries_button_.setCheckable(true);
    open_dictionary_tool_button_.setCheckable(true);

    top_hbox_.addWidget(&close_button_);
    connect(&close_button_, SIGNAL(clicked(bool)), this,
            SLOT(onCloseClicked()), Qt::QueuedConnection);
    close_button_.setFocusPolicy(Qt::NoFocus);
    close_button_.setCheckable(false);

    // Focus on explanation button on dictionary startup.
    explanation_button_.setChecked(true);
    explanation_text_.setFocus();

    // Install event filter.
    explanation_text_.installEventFilter(this);
    similar_words_view_.installEventFilter(this);
}

void DictWidget::initBrowser()
{
    QFont font(QApplication::font());
    font.setPointSize(20);
    doc_.setDefaultFont(font);
    explanation_text_.setDocument(&doc_);
}

void DictWidget::initDictionaries()
{
    dict_.loadDictionaries();
}

/// Use the timeout to update the screen when necessary.
/// Because we don't wait for the screen update finished, there
/// would be some dirty region on the screen.
void DictWidget::onTimeout()
{
    onyx::screen::instance().updateScreen(onyx::screen::ScreenProxy::GU);

    // Make sure the timer is stopped.
    launchTimer(false);
}

void DictWidget::onItemClicked(const QModelIndex & index)
{
    if (similar_words_button_.isChecked())
    {
        QString text = similar_words_model_.itemFromIndex(index)->data().toString();
        lookup(text);
        explanation_button_.click();
    }
    else if (dictionaries_button_.isChecked())
    {
        dict_.select(dict_list_model_.itemFromIndex(index)->text());
        lookup(word_);
        explanation_button_.click();
    }
}

void DictWidget::adjustPosition(int position)
{
    QRect parent_rect = parentWidget()->rect();
    int border = (frameGeometry().width() - geometry().width());
    if (border == 0)
    {
        border = Shadows::PIXELS;
    }
    int width = parent_rect.width();
    if (size().width() != width)
    {
        resize(width, height());
    }

    // Check position.
    QPoint new_pos(0, 0);
    if (frameGeometry().bottom() < parent_rect.height() / 2)
    {
        new_pos.ry() = parent_rect.height() - height();
    }

    move(new_pos);
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC, true);
}

void DictWidget::moreSimilarWords(bool begin)
{
    if (!similar_words_button_.isChecked())
    {
        return;
    }

    if (begin)
    {
        similar_words_offset_ -= similar_words_view_.itemsPerPage();
    }
    else
    {
        similar_words_offset_ += similar_words_view_.itemsPerPage();
    }
    updateSimilarWords();
}

void DictWidget::checkSelectedButton(bool clear_focus)
{
    QAbstractButton *buttonPtr = button_group_.button(internalState());
    buttonPtr->setChecked(true);
    if (clear_focus)
    {
        buttonPtr->clearFocus();
    }
}

void DictWidget::onDetailsClicked(bool)
{
    changeInternalState(EXPLANATION);
    checkSelectedButton();

    explanation_text_.show();
    explanation_text_.setFocus();
    similar_words_view_.hide();
    if (!update_parent_)
    {
        onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GC, false);
    }
    else
    {
        onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC, true);
    }
}

void DictWidget::onWordListClicked(bool)
{
    changeInternalState(SIMILAR_WORDS);
    checkSelectedButton();

    explanation_text_.hide();
    updateSimilarWords();
    onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GC, false);
}

void DictWidget::onDictListClicked(bool)
{
    changeInternalState(DICTIONARY_LIST);
    checkSelectedButton();

    explanation_text_.hide();
    updateDictionaryList();
    onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GC, false);
}

void DictWidget::onRetrieveWordClicked(bool)
{
    changeInternalState(RETRIEVING_WORD);
    checkSelectedButton();
    repaint();
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC, false);
}

void DictWidget::onOpenDictionaryToolClicked(bool)
{
    emit dictToolClicked();
    // changeInternalState(OPEN_DICTIONARY_TOOL);
    // checkSelectedButton();
}

void DictWidget::onPositionClicked(bool)
{
    changeInternalState(ADJUST_POSITION);
    checkSelectedButton();
    adjustPosition(0);
}

void DictWidget::onCloseClicked()
{
    onyx::screen::instance().ensureUpdateFinished();
    releaseKeyboard();
    hide();
    emit closeClicked();
}

bool DictWidget::isInRetrieveWordsMode()
{
    return this->internal_state_ == RETRIEVING_WORD;
}

}   // namespace ui


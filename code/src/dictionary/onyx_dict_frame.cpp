#ifdef BUILD_FOR_ARM
#include <QtGui/qwsdisplay_qws.h>
#include <QtGui/qscreen_qws.h>
#endif

#include "onyx/sys/sys.h"
#include "onyx/dictionary/onyx_dict_frame.h"
#include "onyx/data/data_tags.h"
#include "onyx/screen/screen_update_watcher.h"

enum OnyxDictionaryMenuType
{
    MENU_DICTIONARIES = 11,
    MENU_SIMILAR_WORDS = 12,
    MENU_EXPLANATION = 13,
    MENU_LOOKUP = 14,
    MENU_TTS = 15,
};

static const int DICT_MENU_FONT_SIZE = 20;

OnyxDictFrame::OnyxDictFrame(QWidget *parent,
                             DictionaryManager & dict,
                             tts::TTS *tts,
                             bool exit_by_menu)
    : OnyxDialog(parent)
    , big_layout_(&content_widget_)
    , line_edit_layout_(0)
    , dict_menu_layout_(0)
    , line_edit_(0, this)
    , explanation_(0)
    , list_widget_(0, 0)
    , help_widget_(tr("No dictionary found. Please put dictionaries in SD Card or internal flash's \"dicts\" folder."), this)
    , dictionary_menu_(0, this)
    , tts_button_view_(0, this)
    , keyboard_(this)
    , status_bar_(this, MENU | TASK_MANAGEMENT | MESSAGE | BATTERY | CLOCK | SCREEN_REFRESH | INPUT_TEXT)
    , dict_mgr_(dict)
    , tts_engine_(tts)
    , internal_state_(-1)
    , similar_words_checked_(false)
    , exit_by_menu_(exit_by_menu)
{
#ifndef Q_WS_QWS
    resize(600, 800);
#else
    resize(qApp->desktop()->screenGeometry().size());
#endif

    createLayout();
    initBrowser();
    initDictionaries();

    connectWithChildren();
    connect(&status_bar_, SIGNAL(menuClicked()), this, SLOT(popupMenu()));
    connect(&status_bar_, SIGNAL(requestInputText()), this, SLOT(onHideKeyboard()));
#ifdef Q_WS_QWS
    connect(qApp->desktop(), SIGNAL(resized(int)), this, SLOT(onScreenSizeChanged(int)), Qt::QueuedConnection);
#endif

    // connect the signals with sys_state_
    SysStatus & sys_status = SysStatus::instance();
    connect(&sys_status, SIGNAL(volumeChanged(int, bool)), this, SLOT(onSystemVolumeChanged(int, bool)));

    connect(&explanation_, SIGNAL(highlighted(QString)), this, SLOT(lookup(QString)));
    explanation_.installEventFilter(this);
    list_widget_.installEventFilter(this);
}

OnyxDictFrame::~OnyxDictFrame()
{
}

void OnyxDictFrame::initBrowser()
{
    QFont font(QApplication::font());
    font.setPointSize(20);
    doc_.setDefaultFont(font);
    explanation_.setDocument(&doc_);
}

void OnyxDictFrame::initDictionaries()
{
    dict_mgr_.loadDictionaries();
}

void OnyxDictFrame::onItemClicked(const QModelIndex & index)
{
    onyx::screen::instance().enableUpdate(false);
    if (similar_words_checked_)
    {
        QString text = similar_words_model_.itemFromIndex(index)->data().toString();
        editor()->setText(text);
        lookup(text);
    }
    else
    {
        dict_mgr_.select(dict_list_model_.itemFromIndex(index)->text());
        if (!word_.isEmpty())
        {
            lookup(word_);
        }
        else
        {
            showWidgetWhenInputIsEmpty();
        }
    }
    onyx::screen::instance().enableUpdate(true);
    onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GU);
}

void OnyxDictFrame::moreSimilarWords(bool begin)
{
    if (!similar_words_checked_)
    {
        return;
    }

    if (begin)
    {
        similar_words_offset_ -= list_widget_.itemsPerPage();
    }
    else
    {
        similar_words_offset_ += list_widget_.itemsPerPage();
    }
    updateSimilarWords();
}


void OnyxDictFrame::createLineEdit()
{
    line_edit_.setSubItemType(LineEditView::type());
    line_edit_.setPreferItemSize(QSize(rect().width(), defaultItemHeight()));

    OData *dd = new OData;
    dd->insert(TAG_TITLE, "");
    line_edit_datas_.push_back(dd);

    line_edit_.setFixedGrid(1, 1);
    line_edit_.setFixedHeight(defaultItemHeight()+2*SPACING);
    line_edit_.setMargin(OnyxKeyboard::CATALOG_MARGIN);
    line_edit_.setData(line_edit_datas_);
    line_edit_.setNeighbor(&dictionary_menu_, CatalogView::DOWN);
    line_edit_.setNeighbor(keyboard_.menu(), CatalogView::RECYCLE_DOWN);
    line_edit_.setNeighbor(&sub_menu_, CatalogView::RIGHT);
    line_edit_.setNeighbor(&sub_menu_, CatalogView::RECYCLE_RIGHT);
}

void OnyxDictFrame::createSubMenu()
{
    const int height = defaultItemHeight();
    sub_menu_.setPreferItemSize(QSize(height, height));

    OData *dd = new OData;
    dd->insert(TAG_TITLE, tr("Clear"));
    dd->insert(TAG_MENU_TYPE, OnyxKeyboard::KEYBOARD_MENU_CLEAR);
    sub_menu_datas_.push_back(dd);

    sub_menu_.setFixedGrid(1, 1);
    sub_menu_.setMargin(OnyxKeyboard::CATALOG_MARGIN);
    sub_menu_.setFixedHeight(defaultItemHeight()+2*SPACING);
    sub_menu_.setFixedWidth(defaultItemHeight()*3);
    sub_menu_.setData(sub_menu_datas_);
    sub_menu_.setNeighbor(&line_edit_, CatalogView::RECYCLE_LEFT);
    sub_menu_.setNeighbor(&dictionary_menu_, CatalogView::DOWN);
    sub_menu_.setNeighbor(keyboard_.menu(), CatalogView::RECYCLE_DOWN);
}

void OnyxDictFrame::createDictionaryMenu()
{
    const int height = defaultItemHeight();
    dictionary_menu_.setPreferItemSize(QSize(height, height));

    OData *dd = new OData;
    dd->insert(TAG_COVER, QPixmap(":/images/dictionary_list.png"));
    dd->insert(TAG_MENU_TYPE, MENU_DICTIONARIES);
    dd->insert(TAG_FONT_SIZE, DICT_MENU_FONT_SIZE);
    dictionary_menu_datas_.push_back(dd);

    dd = new OData;
    dd->insert(TAG_COVER, QPixmap(":/images/similar_words.png"));
    dd->insert(TAG_MENU_TYPE, MENU_SIMILAR_WORDS);
    dd->insert(TAG_FONT_SIZE, DICT_MENU_FONT_SIZE);
    dictionary_menu_datas_.push_back(dd);

    dd = new OData;
    dd->insert(TAG_COVER, QPixmap(":/images/explanation.png"));
    dd->insert(TAG_MENU_TYPE, MENU_EXPLANATION);
    dd->insert(TAG_FONT_SIZE, DICT_MENU_FONT_SIZE);
    dictionary_menu_datas_.push_back(dd);

    dd = new OData;
    dd->insert(TAG_COVER, QPixmap(":/images/lookup.png"));
    dd->insert(TAG_MENU_TYPE, MENU_LOOKUP);
    dd->insert(TAG_FONT_SIZE, DICT_MENU_FONT_SIZE);
    dictionary_menu_datas_.push_back(dd);

    dictionary_menu_.setFixedGrid(1, 4);
    dictionary_menu_.setMargin(OnyxKeyboard::CATALOG_MARGIN);
    dictionary_menu_.setFixedHeight(defaultItemHeight()+5*SPACING);
    dictionary_menu_.setData(dictionary_menu_datas_);
    dictionary_menu_.setNeighbor(&line_edit_, CatalogView::UP);
    dictionary_menu_.setNeighbor(&sub_menu_, CatalogView::UP);
    dictionary_menu_.setNeighbor(keyboard_.top(), CatalogView::DOWN);
    dictionary_menu_.setNeighbor(&tts_button_view_, CatalogView::RIGHT);
    dictionary_menu_.setNeighbor(&tts_button_view_, CatalogView::RECYCLE_RIGHT);
}

void OnyxDictFrame::createTtsButtonView()
{
    const int height = defaultItemHeight();
    tts_button_view_.setPreferItemSize(QSize(height, height));

    OData *dd = new OData;
    QPixmap tts_pixmap(":/images/tts_icon.png");
    dd->insert(TAG_COVER, tts_pixmap);
    dd->insert(TAG_MENU_TYPE, MENU_TTS);
    tts_button_datas_.push_back(dd);

    tts_button_view_.setFixedGrid(1, 1);
    tts_button_view_.setMargin(OnyxKeyboard::CATALOG_MARGIN);
    tts_button_view_.setFixedHeight(defaultItemHeight()+5*SPACING);
    tts_button_view_.setFixedWidth(defaultItemHeight()+7*SPACING);
    tts_button_view_.setData(tts_button_datas_);
    tts_button_view_.setNeighbor(&sub_menu_, CatalogView::UP);
    tts_button_view_.setNeighbor(keyboard_.top(), CatalogView::DOWN);
    tts_button_view_.setNeighbor(&dictionary_menu_, CatalogView::LEFT);
    tts_button_view_.setNeighbor(&dictionary_menu_, CatalogView::RECYCLE_LEFT);

    if((qgetenv("DISABLE_TTS").toInt() == 1))
    {
        tts_button_view_.hide();
    }

}

void OnyxDictFrame::createLayout()
{
    OnyxDialog::updateTitle(tr("Dictionary"));
    updateTitleIcon(QPixmap(":/images/dictionary_title.png"));
    content_widget_.setBackgroundRole(QPalette::Button);
    content_widget_.setContentsMargins(0, 0, 0, 0);

    createLineEdit();
    createSubMenu();
    createDictionaryMenu();
    createTtsButtonView();

    line_edit_layout_.setContentsMargins(0, 2, 0, 0);
    line_edit_layout_.setSpacing(2);
    line_edit_layout_.addWidget(&line_edit_);
    line_edit_layout_.addWidget(&sub_menu_);

    big_layout_.setContentsMargins(2, 2, 2, 2);
    big_layout_.setSpacing(0);
    big_layout_.addLayout(&line_edit_layout_, 0);

    // for explanation and similar words list
    big_layout_.addWidget(&explanation_);
    big_layout_.addWidget(&list_widget_);
    big_layout_.addWidget(&help_widget_);
    list_widget_.setVisible(false);
    help_widget_.setVisible(false);
    help_widget_.setReadOnly(true);

    dict_menu_layout_.setContentsMargins(0, 2, 0, 0);
    dict_menu_layout_.setSpacing(2);
    dict_menu_layout_.addWidget(&dictionary_menu_);
    dict_menu_layout_.addWidget(&tts_button_view_);

    big_layout_.addLayout(&dict_menu_layout_, 0);
    big_layout_.addWidget(&keyboard_);
    big_layout_.addWidget(&status_bar_);

    connect(&list_widget_, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onItemClicked(const QModelIndex &)));
    connect(&list_widget_, SIGNAL(exceed(bool)),
            this, SLOT(moreSimilarWords(bool)));
    list_widget_.showHeader(false);
}

void OnyxDictFrame::onSystemVolumeChanged(int value, bool muted)
{
    qDebug("Volume Change:%d", value);
    tts_engine_->sound().setVolume(value);
}

void OnyxDictFrame::onHideKeyboard()
{
    if (keyboard_.isHidden())
    {
        keyboard_.setVisible(true);
    }
    else
    {
        keyboard_.setVisible(false);
    }
    update();
    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::GC);
}

void OnyxDictFrame::onScreenSizeChanged(int)
{
    onyx::screen::instance().enableUpdate(false);
    resize(qApp->desktop()->screenGeometry().size());
    QApplication::processEvents();
    onyx::screen::instance().enableUpdate(true);
}


void OnyxDictFrame::updateActions()
{
    std::vector<int> actions;
    actions.push_back(ROTATE_SCREEN);
    actions.push_back(SCREEN_UPDATE_TYPE);
    actions.push_back(RETURN_TO_LIBRARY);
    system_actions_.generateActions(actions);
}

void OnyxDictFrame::connectWithChildren()
{
    connect(&line_edit_, SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
            this, SLOT(onItemActivated(CatalogView *, ContentView *, int)));
    connect(&sub_menu_, SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
            this, SLOT(onItemActivated(CatalogView *, ContentView *, int)));
    connect(&dictionary_menu_, SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
            this, SLOT(onItemActivated(CatalogView *, ContentView *, int)));
    connect(&tts_button_view_, SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
            this, SLOT(onItemActivated(CatalogView *, ContentView *, int)));
}

void OnyxDictFrame::formatResult(QString &result, QString &fuzzy_word)
{
    if (result.isEmpty())
    {
        result = tr("Not Found In Dictionary.");
    }
    else
    {
        int start = result.indexOf("<tr>");
        int end = result.indexOf("</tr>");
        if(start >= 0 && end >= 0)
        {
            result.replace(end, 5, "]");
            result.replace(start, 4, "[");
        }
    }

    if (!result.contains("<html>", Qt::CaseInsensitive))
    {
        result.replace("\n", "<br>");
    }

    // prepend the look up word
    QString prepend("%1<br><br>");
    qDebug() << "at OnyxDictFrame::formatResult, fuzzy word: " << fuzzy_word;
    if (!fuzzy_word.isEmpty())
    {

        prepend = prepend.arg(fuzzy_word);
    }
    else
    {
        prepend = prepend.arg(word_);
    }
    result.prepend(prepend);
}

bool OnyxDictFrame::lookup(const QString &word)
{
    this->editor()->setText(word);
    if (word.isEmpty())
    {
        return false;
    }

    resetSimilarWordsOffset();

    // Clean the word.
    word_ = word.trimmed();

    //remove quotes (“ ” " ) If it in the words
    if(word_.contains(QChar(0x201C)) || word_.contains(QChar(0x201D)) || word_.contains(QChar(0x0022)))
    {
        word_.remove(QChar(0x201C));
        word_.remove(QChar(0x201D));
        word_.remove(QChar(0x0022));
    }

    // Title
    QString result;
    QString fuzzy_word;
    bool ret = dict_mgr_.fuzzyTranslate(word_, result, fuzzy_word);
    fuzzy_word_ = fuzzy_word;

    formatResult(result, fuzzy_word);

    // Result
    doc_.setHtml(result);

    explanationClicked();
    return ret;
}

void OnyxDictFrame::setDefaultFocus()
{
    keyboard_.initFocus();
}

void OnyxDictFrame::resizeEvent(QResizeEvent *e)
{
    QDialog::resizeEvent(e);
    update();
    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::GC);
}

OnyxLineEdit *OnyxDictFrame::editor()
{
    return static_cast<LineEditView *>(line_edit_.visibleSubItems().front())->innerEdit();
}

void OnyxDictFrame::updateSimilarWordsModel(int count)
{
    // Pick up similar words from current dictionary.
    similar_words_.clear();

    if (word_.isEmpty() && !editor()->text().isEmpty())
    {
        word_ = editor()->text().isEmpty();
    }

    dict_mgr_.similarWords(word_, similar_words_, similar_words_offset_, count);

    if(similar_words_.isEmpty())
    {
        if(!fuzzy_word_.isEmpty())
        {
            similar_words_.clear();
            dict_mgr_.similarWords(fuzzy_word_, similar_words_, similar_words_offset_, count);
        }
    }

    similar_words_model_.clear();
    QStandardItem *parentItem = similar_words_model_.invisibleRootItem();
    QString explanation;
    foreach(QString item, similar_words_)
    {
        // Get explanation.
        dict_mgr_.translate(item, explanation);
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

void OnyxDictFrame::updateSimilarWords()
{
    similar_words_checked_ = true;
    updateSimilarWordsModel(list_widget_.itemsPerPage());

    // Update the list.
    list_widget_.setModel(&similar_words_model_);
}

void OnyxDictFrame::updateDictionaryListModel()
{
    int count = dict_list_model_.rowCount(dict_list_model_.invisibleRootItem()->index());
    if (count <= 0)
    {
        dict_list_model_.clear();
        QStringList list;
        dict_mgr_.dictionaries(list);
        foreach(QString item, list)
        {
            dict_list_model_.appendRow(new QStandardItem(item));
        }
    }
}

void OnyxDictFrame::updateDictionaryList()
{
    similar_words_checked_ = false;
    resetSimilarWordsOffset();
    updateDictionaryListModel();
    list_widget_.clear();
    list_widget_.setModel(&dict_list_model_);
}

void OnyxDictFrame::onItemActivated(CatalogView *catalog, ContentView *item,
                                   int user_data)
{
    OData * item_data = item->data();
    if (item_data->contains(TAG_MENU_TYPE))
    {
        int menu_type = item->data()->value(TAG_MENU_TYPE).toInt();
        if(OnyxKeyboard::KEYBOARD_MENU_CLEAR == menu_type)
        {
            editor()->clear();
            word_ = "";
            doc_.setHtml("");
            update();
            onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::DW);
        }
        else if (MENU_DICTIONARIES == menu_type)
        {
            dictionariesClicked();
        }
        else if (MENU_SIMILAR_WORDS == menu_type)
        {
            similarWordsClicked();
        }
        else if (MENU_EXPLANATION == menu_type)
        {
            explanationClicked();
        }
        else if (MENU_LOOKUP == menu_type)
        {
            lookupClicked();
        }
        else if (MENU_TTS == menu_type)
        {
            ttsClicked();
        }
    }
}

void OnyxDictFrame::dictionariesClicked()
{
    onyx::screen::instance().enableUpdate(false);
    explanation_.hide();
    updateDictionaryList();

    int rows = dict_list_model_.rowCount(dict_list_model_.invisibleRootItem()->index());
    if (rows > 0)
    {
        help_widget_.hide();
        list_widget_.show();
        list_widget_.select(dict_mgr_.selected());
        list_widget_.setFocus();
    }
    else
    {
        list_widget_.hide();
        help_widget_.show();
        setDefaultFocus();
    }

    onyx::screen::instance().enableUpdate(true);
    update();
    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::GU);
}

void OnyxDictFrame::similarWordsClicked()
{
    onyx::screen::instance().enableUpdate(false);
    explanation_.hide();

    updateDictionaryListModel();
    int rows = dict_list_model_.rowCount(dict_list_model_.invisibleRootItem()->index());
    if (rows > 0)
    {
        if (word_.isEmpty() && editor()->text().isEmpty())
        {
            showWidgetWhenInputIsEmpty();
        }
        else
        {
            help_widget_.hide();
            list_widget_.clear();
            list_widget_.show();
            list_widget_.setFocus();

            updateSimilarWords();
        }
    }
    else
    {
        list_widget_.hide();
        help_widget_.show();
        setDefaultFocus();
    }

    onyx::screen::instance().enableUpdate(true);
    update();
    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::GU);
}

void OnyxDictFrame::explanationClicked()
{
    updateDictionaryListModel();
    int rows = dict_list_model_.rowCount(dict_list_model_.invisibleRootItem()->index());
    if (rows > 0)
    {
        list_widget_.hide();
        help_widget_.hide();
        explanation_.show();
        explanation_.setFocus();
    }
    else
    {
        explanation_.hide();
        list_widget_.hide();
        help_widget_.show();
        setDefaultFocus();
    }

    update();
    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::GU);
}

void OnyxDictFrame::lookupClicked()
{
    lookup(editor()->text());
}

void OnyxDictFrame::ttsClicked()
{
    QString text = editor()->text();
    if (!text.isEmpty() && tts_engine_ != 0)
    {
        tts_engine_->speak(text);
    }
}

void OnyxDictFrame::showWidgetWhenInputIsEmpty()
{
    list_widget_.hide();
    help_widget_.hide();
    explanation_.show();
    setDefaultFocus();
}

QWidget * OnyxDictFrame::getVisibleWidget()
{
    QWidget * wid = &explanation_;
    if (list_widget_.isVisible())
    {
        wid = &list_widget_;
    }
    return wid;
}

void OnyxDictFrame::keyPressEvent(QKeyEvent *event)
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

void OnyxDictFrame::keyReleaseEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == ui::Device_Menu_Key)
    {
        popupMenu();
        return;
    }
    OnyxDialog::keyReleaseEvent(event);
}

bool OnyxDictFrame::eventFilter(QObject *obj, QEvent *event)
{
    bool ret = OnyxDialog::eventFilter(obj, event);
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_Escape)
        {
            ke->accept();
            sub_menu_.visibleSubItems().front()->setFocus();
            return true;
        }
        else if (ke->key() == Qt::Key_PageDown
                || ke->key() == Qt::Key_PageUp
                || ke->key() == Qt::Key_Up
                || ke->key() == Qt::Key_Down
                || ke->key() == Qt::Key_Left
                || ke->key() == Qt::Key_Right)
        {
            onyx::screen::ScreenProxy::Waveform w = onyx::screen::ScreenProxy::GU;
            if (obj == &list_widget_
                    && ke->key() != Qt::Key_PageUp
                    && ke->key() != Qt::Key_PageDown)
            {
                w = onyx::screen::ScreenProxy::DW;
            }
            update();
            onyx::screen::watcher().enqueue(this, w,
                    onyx::screen::ScreenCommand::WAIT_NONE);
        }
    }
    return ret;
}

static RotateDegree getSystemRotateDegree()
{
    int degree = 0;
#ifdef BUILD_FOR_ARM
    degree = QScreen::instance()->transformOrientation();
#endif
    return static_cast<RotateDegree>(degree);
}

void OnyxDictFrame::rotate()
{
#ifndef Q_WS_QWS
    RotateDegree prev_degree = getSystemRotateDegree();
    if (prev_degree == ROTATE_0_DEGREE)
    {
        resize(800, 600);
    }
    else
    {
        resize(600, 800);
    }
#else
    SysStatus::instance().rotateScreen();
#endif
}

void OnyxDictFrame::popupMenu()
{
    ui::PopupMenu menu(this);
    updateActions();
    menu.setSystemAction(&system_actions_);

    if (menu.popup() != QDialog::Accepted)
    {
        return;
    }

    QAction * group = menu.selectedCategory();
    if (group == system_actions_.category())
    {
        SystemAction system = system_actions_.selected();
        if (system == RETURN_TO_LIBRARY)
        {
            returnToLibrary();
        }
        else if (system == SCREEN_UPDATE_TYPE)
        {
            onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU);
            onyx::screen::instance().toggleWaveform();
        }
        else if (system == MUSIC)
        {
            // Start or show music player.
            onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GU);
            sys::SysStatus::instance().requestMusicPlayer(sys::START_PLAYER);
        }
        else if (system == ROTATE_SCREEN)
        {
            rotate();
        }
        return;
    }
}

void OnyxDictFrame::returnToLibrary()
{
    if (exit_by_menu_)
    {
        qApp->exit();
    }
    else
    {
        close();
    }
}

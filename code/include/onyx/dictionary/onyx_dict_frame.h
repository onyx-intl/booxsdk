#ifndef ONYX_DICT_FRAME_H_
#define ONYX_DICT_FRAME_H_

#include "onyx/base/base.h"
#include "onyx/ui/catalog_view.h"
#include "onyx/ui/onyx_keyboard.h"
#include "onyx/sys/sys.h"
#include "onyx/tts/tts.h"
#include "dictionary_manager.h"

using namespace ui;
using namespace base;

class OnyxDictFrame: public OnyxDialog
{
    Q_OBJECT

public:
    OnyxDictFrame(QWidget *parent, DictionaryManager & dict, tts::TTS *tts = 0, bool exit_by_menu = true);
    ~OnyxDictFrame();

    void setDefaultFocus();

public slots:
    bool lookup(const QString &word);

Q_SIGNALS:
    void rotateScreen();
    void keyReleaseSignal(int);
    void closeClicked();

protected Q_SLOTS:
    void onItemActivated(CatalogView *catalog, ContentView *item,
                                           int user_data);

protected:
    void resizeEvent(QResizeEvent *e);

private Q_SLOTS:
    void popupMenu();
    void onScreenSizeChanged(int);
    void onHideKeyboard();
    void onSystemVolumeChanged(int value, bool muted);

    void onItemClicked(const QModelIndex & index);
    void moreSimilarWords(bool);

private:
    void createLineEdit();
    void createSubMenu();
    void createDictionaryMenu();
    void createTtsButtonView();
    void createLayout();
    void connectWithChildren();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

    OnyxLineEdit *editor();

    void initBrowser();
    void initDictionaries();
    void updateActions();

    void rotate();
    void returnToLibrary();

    void updateSimilarWordsModel(int count);
    void resetSimilarWordsOffset() { similar_words_offset_ = 0; }
    void updateSimilarWords();

    void updateDictionaryListModel();
    void updateDictionaryList();

    void showWidgetWhenInputIsEmpty();
    QWidget * getVisibleWidget();
    void dictionariesClicked();
    void similarWordsClicked();
    void explanationClicked();
    void lookupClicked();
    void ttsClicked();

    // for the result of dictionary look up
    void formatResult(QString &result, QString &fuzzy_word);

private:
    QVBoxLayout big_layout_;
    QHBoxLayout line_edit_layout_;
    QHBoxLayout dict_menu_layout_;

    CatalogView line_edit_;
    CatalogView sub_menu_;

    OnyxTextBrowser explanation_;       ///< The lookup result.
    OnyxTreeView list_widget_;          ///< Storing words and dictionary list.
    OnyxTextEdit help_widget_;          ///< Remind user to put dictionaries into SD Card

    CatalogView dictionary_menu_;
    CatalogView tts_button_view_;

    ODatas line_edit_datas_;
    ODatas sub_menu_datas_;
    ODatas dictionary_menu_datas_;
    ODatas tts_button_datas_;

    OnyxKeyboard keyboard_;
    StatusBar status_bar_;              ///< Status Bar

    DictionaryManager &dict_mgr_;
    tts::TTS *tts_engine_;

    QButtonGroup button_group_;
    QTextDocument doc_;
    QString word_;                      ///< Word currently queried.
    QString fuzzy_word_;
    QStringList similar_words_;
    int similar_words_offset_;
    QTimer timer_;                      ///< Timer to update the screen.

    QStandardItemModel similar_words_model_;
    QStandardItemModel dict_list_model_;

    SystemActions system_actions_;      ///< System Actions
    int internal_state_;
    bool similar_words_checked_;
    bool exit_by_menu_;

private:
    NO_COPY_AND_ASSIGN(OnyxDictFrame);
};

#endif

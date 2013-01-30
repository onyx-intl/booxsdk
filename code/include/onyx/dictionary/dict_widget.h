#ifndef LIB_DICT_WIDGET_H_
#define LIB_DICT_WIDGET_H_

#include "onyx/base/base.h"
#include "onyx/ui/ui.h"
#include "dictionary_manager.h"
#include "onyx/tts/tts.h"

namespace ui
{

/// The dictionary widget library. It provides a gui frontend
/// for LibDict to enable user to search in dictionary.
class DictWidget : public OnyxDialog
{
    Q_OBJECT

public:
    DictWidget(QWidget *parent, DictionaryManager & dict, tts::TTS *tts = 0, bool enable_full_screen = false);
    ~DictWidget();

public:
    bool ensureVisible(const QRectF & rect, bool update_parent = false);

    bool isInRetrieveWordsMode();

public slots:
    bool lookup(const QString &word);
    bool lookup(const QString &word, bool update_screen);

protected:
    virtual void keyReleaseEvent(QKeyEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual bool event(QEvent *);
    void moveEvent(QMoveEvent *e);
    void resizeEvent(QResizeEvent *e);
    void hideEvent(QHideEvent * event);

public:
    struct FunctionDescription
    {
        const char * description;
        int index;
    };

Q_SIGNALS:
    void keyReleaseSignal(int);
    void closeClicked();
    void positionAdjusted();
    void dictToolClicked();

private:
    void createLayout();
    void initBrowser();
    void initDictionaries();
    void launchTimer(bool launch);

    void updateSimilarWordsModel(int count);
    void resetSimilarWordsOffset() { similar_words_offset_ = 0; }
    void updateSimilarWords();

    void updateDictionaryListModel();
    void updateDictionaryList();

    void updateVisibleWidgets();
    bool eventFilter(QObject *obj, QEvent *event);

    void changeInternalState(int);
    int  internalState() { return internal_state_; }

    int getPreviousFocusButtonId(const int current_checked);
    int getNextFocusButtonId(const int current_checked);
    bool handleLeftRightKey(const int checked_id, const int key);
    void changeDescription(const int button_id);
    bool handleUpDownKey(QKeyEvent * ke);

    void checkSelectedButton(bool clear_focus = true);

    // for the result of dictionary look up
    void formatResult(QString &result, QString &fuzzy_word);

private Q_SLOTS:
    void onTimeout();
    void onItemClicked(const QModelIndex & index);
    void onDetailsClicked(bool);
    void onWordListClicked(bool);
    void onDictListClicked(bool);
    void onPositionClicked(bool);
    void onRetrieveWordClicked(bool);
    void onOpenDictionaryToolClicked(bool);
    void onCloseClicked();
    void moreSimilarWords(bool);
    void adjustPosition(int);

private:
    DictionaryManager&      dict_;
    tts::TTS *tts_;

    QVBoxLayout   big_vbox_;
    QHBoxLayout   top_hbox_;
    QVBoxLayout   content_vbox_;

    OnyxPushButton retrieve_words_button_;
    OnyxPushButton explanation_button_;
    OnyxPushButton similar_words_button_;
    OnyxPushButton dictionaries_button_;
    OnyxPushButton position_button_;
    OnyxPushButton close_button_;
    OnyxPushButton open_dictionary_tool_button_;
    QButtonGroup    button_group_;

    OnyxLabel func_description_label_;

    OnyxTextBrowser  explanation_text_; ///< The lookup result.
    OnyxTreeView similar_words_view_;

    QTextDocument   doc_;
    QString         word_;                  ///< Word currently queried.
    QString         fuzzy_word_;
    QStringList     similar_words_;
    int             similar_words_offset_;
    QTimer          timer_;                 ///< Timer to update the screen.

    QStandardItemModel similar_words_model_;
    QStandardItemModel dict_list_model_;

    int internal_state_;
    bool update_parent_;
    bool enable_full_screen_;

private:
    static const FunctionDescription DICT_FUNC_DESCRIPTION[];
    static const int DESCRIPTION_COUNT;

};

};  // namespace ui

#endif // LIB_DICT_WIDGET_H_

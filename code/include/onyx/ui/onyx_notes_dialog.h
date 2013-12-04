#ifndef ONYX_NOTES_WIDGET_H_
#define ONYX_NOTES_WIDGET_H_

#include "onyx/base/base.h"
#include "ui_global.h"
#include "onyx_dialog.h"
#include "catalog_view.h"
#include "onyx_keyboard.h"
#include "text_edit.h"

namespace ui
{

// Dialog for getting input with soft keyboard support.
class OnyxNotesDialog: public OnyxDialog
{
    Q_OBJECT

public:
    OnyxNotesDialog(const QString & text = QString(), QWidget *parent = 0);
    ~OnyxNotesDialog();

public:
    int popup(const QString & text);
    QString inputText();
    void clearClicked();

protected Q_SLOTS:
    void onItemActivated(CatalogView *catalog, ContentView *item,
            int user_data);
    void onOutOfBoundary(CatalogView*, int, int);
    void focusKeyboardTop();
    void focusKeyboardMenu();

private:
    void createLayout();
    void createNotesEdit();
    void createSubMenu();
    void connectWithChildren();
    bool eventFilter(QObject *obj, QEvent *event);
    // handling key press event
    void keyPressEvent(QKeyEvent *event);

private:
    QVBoxLayout big_layout_;
    QHBoxLayout notes_edit_layout_;
    OnyxTextEdit notes_edit_;
    // contains OK and Clear menu items
    CatalogView sub_menu_;

    ODatas sub_menu_datas_;

    OnyxKeyboard keyboard_;
    QString title_;
    QWidget *to_focus_;
};

}   // namespace ui

#endif

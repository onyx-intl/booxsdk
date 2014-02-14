
#ifndef SIMPLE_MESSAGE_DIALOG_H_
#define SIMPLE_MESSAGE_DIALOG_H_

#include "onyx/base/base.h"
#include "ui_global.h"
#include "onyx_dialog.h"
#include "onyx_keyboard.h"
#include "line_edit_view_group.h"

namespace ui
{

class PasswordDialogWithMsgBox: public QDialog
{
    Q_OBJECT

public:
    PasswordDialogWithMsgBox(QWidget *parent = 0, const ODatas &ds = ODatas(),
            const QString &title = QString(),
            const QString &default_passwd_label = tr("Password "));
    ~PasswordDialogWithMsgBox();

public:
    int popup(const QString &password, PasswordUseType type = USB_CONNECTION_TYPE);
    QString getPasswd();
    void clearState();

protected Q_SLOTS:
    void onItemActivated(CatalogView *catalog, ContentView *item,
            int user_data);
    void onCancelButtonClicked();
    void onOKButtonClicked();
    void onConnectToPCWithPasswd(const bool result);

Q_SIGNALS:
    void textFinished();

private:
    void createLayout();
    void createShowPlainText();
    void connectWithChildren();

    void showPlainTextClicked(bool target_value);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *ke);
    void paintEvent(QPaintEvent *event);
    int defaultItemHeight();
    bool validateLength(const QString &input);

private:
    QVBoxLayout big_layout_;
    QHBoxLayout *line_edit_layout_;

    CatalogView sub_menu_;
    CatalogView show_plain_text_;
    QVector<CatalogView *> edit_view_list_;
    LineEditViewGroup edit_view_group_;

    ODatas sub_menu_datas_;
    ODatas show_plain_text_datas_;
    QVector<ODatas *> all_line_edit_datas_;

    OnyxKeyboard keyboard_;
    QString title_;
    QString default_passwd_label_;
    ODatas edit_list_;

    OnyxLineEdit passwd_edit_;
    PasswordUseType type_;
};

}   // namespace ui

#endif

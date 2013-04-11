#ifndef UI_TASK_LIST_DIALOG_H_
#define UI_TASK_LIST_DIALOG_H_

#include "onyx/base/base.h"
#include "onyx/sys/sys.h"
#include "onyx_dialog.h"
#include "onyx/ui/catalog_view.h"
#include "onyx/ui/buttons.h"

namespace ui
{

class TaskItem: public QWidget
{
    Q_OBJECT

public:
    TaskItem(QWidget *parent);
    ~TaskItem(void);

Q_SIGNALS:
    void itemClicked();
    void itemClosed();

public:
    void setImage(const QString & path);
    void setTitle(const QString & title);

private:
    void createLayout();

private:
    QHBoxLayout hor_layout_;
    OnyxLabel image_label_;
    OnyxLabel title_label_;
    OnyxPushButton close_button_;
};

/// Locale widget. Not sure we should place it in the ui library.
/// So far, only explorer use it.
class TaskListDialog : public OnyxDialog
{
    Q_OBJECT

public:
    TaskListDialog(QWidget *parent, SysStatus & ref);
    ~TaskListDialog(void);

public:
    int exec();

private:
    void createLayout();
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    bool event(QEvent*);

private Q_SLOTS:
    void onButtonChanged(CatalogView *catalog, ContentView *item, int user_data);
    void onOkClicked();
    void updateAll();

private:
    SysStatus & status_;
    QVBoxLayout ver_layout_;

    OnyxLabel battery_power_;

    QVector<TaskItem *> buttons_;
    int selected_;
    QStringList all_;

};

}   // namespace ui

#endif      // UI_TASK_LIST_DIALOG_H_

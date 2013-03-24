#ifndef UI_TASK_LIST_DIALOG_H_
#define UI_TASK_LIST_DIALOG_H_

#include "onyx/base/base.h"
#include "onyx/sys/sys.h"
#include "onyx_dialog.h"
#include "onyx/ui/catalog_view.h"

namespace ui
{

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

private:
    SysStatus & status_;
    QVBoxLayout ver_layout_;

    OnyxLabel battery_power_;

    CatalogView buttons_;
    OData *interval_selected_;

    QHBoxLayout hor_layout_;
    CatalogView ok_;

};

}   // namespace ui

#endif      // UI_TASK_LIST_DIALOG_H_

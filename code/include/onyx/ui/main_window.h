#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include "onyx/base/base.h"
#include "onyx/ui/ui.h"
#include "onyx/sys/sys.h"

#include "onyx/base/base_model.h"

using namespace ui;
using namespace base;

namespace vbf
{

typedef QMap<int, QWidget*> Views;
typedef Views::iterator ViewIter;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow(QObject *parent);
    ~MainWindow();

    void attachModel(BaseModel* model);
    QWidget* getView(int type);
    QWidget* activateView(int type);

    void clearViews();
    void setContentMargins(int left, int top, int right, int bottom);

    StatusBar * getStatusBar();

public Q_SLOTS:
    // handle events from view
    void handlePositionChanged(const int, const int);
    void handleSetStatusBarFunctions(const StatusBarItemTypes funcs);
    void handleFullScreen(bool enable);
    void handleItemStatusChanged(const StatusBarItemType type,
                                 const int state);
    void handleRequestUpdate(bool update_now);
    void handlePopupJumpPageDialog();
    void handleClockClicked();

    void onVolumeClicked();

Q_SIGNALS:
    // base signals
    void createView(int type,
                    MainWindow* main_window,
                    QWidget* &result);
    void attachView(int type,
                    QWidget* view,
                    MainWindow* main_window);
    void deattachView(int type,
                      QWidget* view,
                      MainWindow* main_window);

    // signals to member widgets
    void pagebarClicked(const int, const int);
    void popupContextMenu();
    void statusBarStylusClicked();
    void itemStatusChanged(const StatusBarItemType, const int);

private Q_SLOTS:
    // handle events from member widgets
    void onPagebarClicked(const int, const int);
    void onPopupContextMenu();
    void onStatusBarStylusClicked();
    void onMusicPlayerStateChanged(int);

private:
    bool event(QEvent * event);
    void initializeLayout();

private:
    Views                   views_;             ///< List of available views
    QWidget*                active_view_;       ///< Pointer of active view
    BaseModel*              model_;             ///< Document Model
    QVBoxLayout             vlayout_;           ///< Vertical Layout
    QHBoxLayout             hlayout_;           ///< Horizontal Layout
    StatusBar               status_bar_;        ///< Status Bar

    NO_COPY_AND_ASSIGN(MainWindow);
};

};
#endif

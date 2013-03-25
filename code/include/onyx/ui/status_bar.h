// Author: John

/// This file defines StatusBar. It should be used by other
/// packages in the code base to use statusbar.

#ifndef ONYX_STATUS_BAR_H_
#define ONYX_STATUS_BAR_H_

#include "onyx/base/base.h"
#include "onyx/ui/ui_global.h"
#include "status_bar_item.h"
#include "common_dialogs.h"
#include "clock_dialog.h"
#include "volume_control.h"
#include "legacy_power_management_dialog.h"
#include "power_management_dialog.h"

namespace ui
{

/// Status bar library for GUI shell and other applications.
/// Status bar separates the whole bar into three parts:
/// - Menu area.
/// - Message area.
/// - System area.
class StatusBar : public QStatusBar
{
    Q_OBJECT
public:
    StatusBar(QWidget *parent, StatusBarItemTypes items = MENU|PROGRESS|MESSAGE|BATTERY|MUSIC_PLAYER);
    ~StatusBar(void);

public:
    void addItems(StatusBarItemTypes ids);
    int itemState(const StatusBarItemType type);
    void setAutoConnect(bool connect){auto_connect_ = connect;}

public Q_SLOTS:
    void showItem(StatusBarItemType item, bool show = true);
    bool setProgress(const int value, const int total,
            bool show_message = true, const QString &message = "");
    bool setMessage(const QString & message);
    bool setItemState(const StatusBarItemType type, const int state);

    void enableJumpToPage(bool enable) { enable_jump_to_page_ = enable; }
    bool isJumpToPageEnabled() { return enable_jump_to_page_; }

    bool remove(const StatusBarItemType type);
    void clear();
    void closeChildrenDialogs();
    void closeUSBDialog();
    void closeVolumeDialog();
    void closeLowBatteryDialog();

    void onMessageAreaClicked();
    void onBatteryClicked();
    void onClockClicked();
    void onScreenRefreshClicked();
    void onInputUrlClicked();
    void onInputTextClicked();
    void onVolumeClicked();
    void onMusicPlayerClicked();

    void addAppItem(StatusBarItemType before, const int appId, const QImage & image);
    void removeAppItem(const int appId);
    void setAppIcon(const int appId, const QImage & image);
    void setAppItemState(const int appId, const int state);

    void showTaskManagementDialog();

Q_SIGNALS:
    void progressClicked(const int percent, const int value);
    void menuClicked();
    void stylusClicked();
    void requestInputUrl();
    void requestInputText();
    void appClicked(int id);

private Q_SLOTS:
    void onProgressChanging(const int current, const int total);
    void onProgressChanged(const int percent, const int value);
    void onMenuClicked();
    void onStylusClicked();
    bool onItemStatusChanged(const StatusBarItemType type, const int state);
    void onViewportChanged(const QRect & parent, const QRect & child, int current_column, int total);
    void onAppClicked(int );

    // handle the events from system status manager
    void onBatterySignal(int value, int status);
    void onLowBatterySignal();
    void onAboutToSuspend();
    void onWakeup();
    void onAboutToShutdown();
    void onWifiDeviceChanged(bool enabled);
    void onReport3GNetwork(const int signal, const int total, const int network);
    void onPppConnectionChanged(const QString &message, int value);
    void onStylusChanged(bool inserted);
    void onConnectToPC(bool);
    void onVolumeButtonsPressed();
    void onHideVolumeDialog();
    void onConfigKeyboard();
    void autoSelect();

private:
    virtual void mouseMoveEvent(QMouseEvent *me);
    virtual void paintEvent(QPaintEvent *pe);

private:
    void createLayout();
    void setupConnections();
    void initUpdate();
    StatusBarItem *item(const StatusBarItemType type, bool create = true);

    void changeConnectionStatus(const int conn_status);
    void changeBatteryStatus(const int value, const int status, bool update_screen);
    void changeStylus(const int stylus);

    USBConnectionDialog * usbConnectionDialog(bool create);
    LowBatteryDialog  * lowBatteryDialog(bool create);
    ClockDialog * clockDialog(bool create, const QDateTime & start);
    VolumeControlDialog *volumeDialog(bool create);
    LegacyPowerManagementDialog *legacyPMDialog(bool create);
    PowerManagementDialog *pmDialog(bool create);

private:
    typedef shared_ptr<StatusBarItem>          StatusBarItemPtr;
    typedef vector<StatusBarItemPtr>           StatusBarItems;
    typedef vector<StatusBarItemPtr>::iterator StatusBarIter;

private:
    StatusBarItemTypes items_;
    StatusBarItems     widgets_;
    bool               enable_jump_to_page_;
    scoped_ptr<USBConnectionDialog> usb_connection_dialog_;
    scoped_ptr<LowBatteryDialog> low_battery_dialog_;
    scoped_ptr<ClockDialog> clock_dialog_;
    scoped_ptr<VolumeControlDialog> volume_dialog_;
    scoped_ptr<LegacyPowerManagementDialog> legacy_pm_dialog_;
    scoped_ptr<PowerManagementDialog> pm_dialog_;

    scoped_ptr<OnyxLabel> right_margin_;
    bool auto_connect_;
};

};  // namespace ui

#endif  // ONYX_STATUS_BAR_H_

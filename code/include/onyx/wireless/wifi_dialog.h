/// Wifi dialog.
#ifndef UI_WIFI_DIALOG_H_
#define UI_WIFI_DIALOG_H_

#include <map>
#include <QtGui/QtGui>
#include "onyx/ui/paginator.h"
#include "ap_item.h"
#include "onyx/wireless/ap_conf_dialog_s.h"

using namespace sys;

namespace ui
{

class WifiDialog : public QDialog
{
    Q_OBJECT

public:
    WifiDialog(QWidget *parent, SysStatus & sys);
    ~WifiDialog();
    void updateTr();
    void updateFonts();

public Q_SLOTS:
    void triggerScan();

public:
    void assignConnect(const WifiProfile & profile);
    void wifiScanResults(WifiProfiles &wifi_profiles) { wifi_profiles = scan_results_; }
    void runBackGround();
    int  popup(bool start_scan = true, bool auto_connect = true);
    void connect(const QString & ssid, const QString &password);
    bool connectToBestAP();
    bool connectToDefaultAP();
    QString connectingAccessPoint();
    void enableIsConfiguration();

protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual bool event(QEvent *e);

private Q_SLOTS:
    void onScanTimeout();
    void onConnectionTimeout();

    void scan();
    void onAPItemClicked(WifiProfile & profile);
    void onRefreshClicked();
    void onCustomizedClicked();
    void onBackClicked();
    void onCloseClicked();

    void onSdioChanged(bool on);
    void enableChildren(bool enable);

    void onScanReturned();
    void onConnectionChanged(WifiProfile , WpaConnection::ConnectionState state);
    void onControlStateChanged(WpaConnectionManager::ControlState control);
    void onNeedPassword(WifiProfile profile);
    void onNoMatchedAP();

    void onComplete();
    void onAPConfig(WifiProfile &);

    void onItemActivated(CatalogView *catalog, ContentView *item, int user_data);
    void onPositionChanged(const int, const int);

    void onRestartWpa();

private:
    void createLayout();
    void connectAllAPItems(CatalogView &view);
    void arrangeAPItems(WifiProfiles & list);
    void appendStoredAPs(WifiProfiles & list);
    void setupConnections();
    void clear();

    void setPassword(WifiProfile & profile, const QString & password);
    void storeAp(WifiProfile & profile);

    void updateStateLabel(WpaConnectionManager::ControlState state);

    void enableAutoConnect(bool e) { auto_connect_to_best_ap_ = e; }
    bool allowAutoConnect() { return auto_connect_to_best_ap_; }

    void scanResults(WifiProfiles &);
    WifiProfiles records(sys::SystemConfig& conf);

//    void updateHardwareAddress();
    void showPaginationButtons(bool show_prev = false, bool show_next = false);
    bool showConfigurationDialog(WifiProfile &profile);

    void checkAndRestorePassword(WifiProfile &profile);

    void sort(ODatas &list);

    ApConfigDialogS * apConfigDialog(WifiProfile &profile);

private:
    QVBoxLayout  big_box_;
    QHBoxLayout state_widget_layout_;
    QVBoxLayout content_layout_;
    QVBoxLayout ap_layout_;
    QHBoxLayout buttons_layout_;

//    OnyxLabel hardware_address_;
    OnyxPushButton close_button_;

    WifiTitleItem state_widget_;
    OnyxPushButton prev_button_;
    OnyxPushButton next_button_;
    ui::CatalogView ap_view_;

    const SysStatus & sys_;
    WpaConnectionManager& proxy_;
    bool auto_connect_to_best_ap_;      ///< Access points used successfully before.
    bool auto_connect_to_default_ap_;    ///< If we have pre-installed access point.

    WifiProfiles scan_results_;
    ODatas datas_;
    QString clicked_ssid_;

    bool is_configuration_;

    scoped_ptr<ApConfigDialogS> ap_config_dialog_;
    bool restart_for_auth_failed_;

};

}

#endif // UI_WIFI_DIALOG_H_

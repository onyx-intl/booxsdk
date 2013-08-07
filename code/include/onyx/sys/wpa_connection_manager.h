#ifndef WAP_CONNECTION_MANAGER_H_
#define WAP_CONNECTION_MANAGER_H_

#include <string>
#include <QtCore/QtCore>
#include "onyx/base/base.h"
#include "onyx/base/dbus.h"
#include "onyx/sys/wpa_connection.h"
#include "onyx/sys/wifi_conf.h"
#include "onyx/sys/sys_conf.h"

using namespace sys;

/// Wpa Connection Manager to provide for convenience.
/// Message between WpaConnectionManager and WpaConnection
/// - sdio state changed
/// - scan
/// - connection changed
/// - authentication
class WpaConnectionManager : public QObject
{
    Q_OBJECT

public:
    // Used by manager and caller. Normally, caller is not necessary to access
    // wpa connection state.
    enum ControlState
    {
        CONTROL_INIT,               ///< Init state.
        CONTROL_SCANNING,           ///< In scanning.
        CONTROL_SCANNED,            ///< Scan finished.
        CONTROL_CONNECTING,         ///< In connecting.
        CONTROL_CONNECTED,          ///< connected.
        CONTROL_CONNECTING_TIMEOUT, ///< timeout.
        CONTROL_CONNECTING_FAILED,  ///< failed, psk incorrect.
        CONTROL_DISCONNECTED,       ///< Disconnected
        CONTROL_ACQUIRING_ADDRESS,  ///< Acquiring address.
        CONTROL_ACQUIRING_ADDRESS_FAILED,
        CONTROL_COMPLETE,           ///< All finished.
        CONTROL_STOP,
    };

public:
    WpaConnectionManager();
    ~WpaConnectionManager();

    bool isConnectionOnProgress();
    bool isConnectionComplete();

public Q_SLOTS:
    void enableAutoConnect(bool e) { auto_connect_ = e; }
    bool allowAutoConnect() { return auto_connect_; }

    void enableAutoReconnect(bool e) { auto_reconnect_ = e; }
    bool allowAutoReconnect() { return auto_reconnect_; }

    bool start();
    bool stop();

    WpaConnection::ConnectionState wpaState();
    ControlState controlState();

    void scanResults(WifiProfiles &);

    bool connectTo(WifiProfile profile);
    WifiProfile connectingAP();
    void resetConnectRetry();

    QString networkInterface();
    QString address();
    QString hardwareAddress();

    void queryStatus();

    void enableSuspendPeriod();
    void disableSuspendPeriod();

private Q_SLOTS:
    bool enableSdio(bool enable = true) const;
    bool sdioState() const ;
    bool enableSdio(bool enable = true);
    bool isWpaSupplicantRunning();
    bool startWpaSupplicant(const QString & conf_file_path);
    bool stopWpaSupplicant();
    void onSdioChanged(bool on);

    void initScan();
    void scan();
    void onScanTimeout();
    void onScanReturned(WifiProfiles & list);

    void onNeedPassword(WifiProfile profile);
    void onConnectionChanged(WifiProfile, WpaConnection::ConnectionState state);
    void onConnectionTimeout();

    void onComplete();

    void onRestartWpa();

Q_SIGNALS:
    // signals for caller
    void wpaStateChanged(bool running);
    void controlStateChanged(WpaConnectionManager::ControlState control);
    void connectionChanged(WifiProfile profile, WpaConnection::ConnectionState state);
    void passwordRequired(WifiProfile profile);
    void noMatchedAP();
    void restartWpa();

private:
    bool checkWifiDevice();
    bool checkWpaSupplicant();
    void setupConnections();

    void resetProfile(WifiProfile & profile);
    void setPassword(WifiProfile & profile, const QString & password);
    bool checkAuthentication(WifiProfile & profile);
    bool syncAuthentication(WifiProfile & source, WifiProfile & target);
    void saveAp(WifiProfile & profile);

    void broadcastPasswordRequireSignal(WifiProfile profile);

    void increaseScanRetry() { ++scan_retry_; }
    void resetScanRetry() { scan_retry_ = 0; }
    bool canScanRetry();
    void clearScanContext();

    int increaseConnectRetry() { return ++connect_retry_; }
    bool canRetryConnect();
    void clearConnectContext();

    bool connectToBestAP();

    void stopAllTimers();
    void setWpaState(WifiProfile profile, WpaConnection::ConnectionState s);

    void setControlState(ControlState control);

    bool isWifiEnabled() { return wifi_enabled_; }

    WifiProfiles & records(sys::SystemConfig& conf);
    WpaConnection & proxy();

private:
    QDBusConnection connection_;    ///< Connection to system manager.
    scoped_ptr<WpaConnection> proxy_;

    QTimer scan_timer_;         ///< Scan timer
    int scan_retry_;            ///< Scan retry.

    QTimer connection_timer_;   ///< Authentication timer.
    int connect_retry_;         ///< retry.

    WpaConnection::ConnectionState wpa_state_;  ///< report wpa supplicant state.
    ControlState control_state_;
    
    bool auto_connect_;
    bool auto_reconnect_;
    bool wifi_enabled_;

    WifiProfiles scan_results_;     ///< Also serves as connect list.
    scoped_ptr<WifiProfiles> records_;  ///< All profiles that stored in database.

    bool suspend_period_;
    bool disable_idle_called_;

};

#endif      // WAP_CONNECTION_MANAGER_H_

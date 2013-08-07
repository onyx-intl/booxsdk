#ifndef WAP_SUPPLICANT_CONNECTION_H_
#define WAP_SUPPLICANT_CONNECTION_H_

#include <string>
#include <QtCore/QtCore>
#include "onyx/base/base.h"
#include "onyx/base/dbus.h"
#include "onyx/wpa/wpa_ctrl.h"
#include "onyx/sys/wifi_conf.h"

using namespace sys;

/// Maintains connection to wpa_supplicant daemon.
/// It sends command to wap_supplicant and broadcasts
/// messages to all listeners.
class WpaConnection : public QObject
{
    Q_OBJECT

public:
    WpaConnection(const QString & name);
    ~WpaConnection();

public:
    enum ConnectionState
    {
        STATE_UNKNOWN = -1,
        STATE_DISABLED,         ///< The wifi device is disabled.
        STATE_HARDWARE_ERROR,   ///< Wifi switch issue.
        STATE_ABORTED,          ///< Aborted by user.
        STATE_SCANNING,
        STATE_SCANNED,
        STATE_CONNECTING,       ///< Associating.
        STATE_DISCONNECTED,
        STATE_AUTHENTICATION,
        STATE_AUTHENTICATION_FAILED,
        STATE_CONNECTED,        ///< Association complete.
        STATE_ACQUIRING_ADDRESS,///< Acquiring IP address.
        STATE_COMPLETE,         ///< Complete, now able to access internet.
        STATE_CONNECT_ERROR,
        STATE_ACQUIRING_ADDRESS_ERROR,  ///< Could not acquire address.
        STATE_TIMEOUT,
        STATE_DISCONNECT,
        STATE_CONNECTION_LOST           ///< Only for disconnected from complete state.
    };

    static QString connectionStateToString(ConnectionState state);

public Q_SLOTS:
    int  openCtrlConnection(const QString & name = QString());
    bool isWpaSupplicantConnected() { return wpa_supplicant_connected_; }

    bool update();
    bool status(QVariantMap &info, bool broadcast = true);
    bool isComplete(bool auto_connect = true);
    bool isConnectionEstablished();
    bool listNetworks(WifiProfiles &networks);
    bool scan();
    bool scanResults(WifiProfiles & aps);
    bool reAssociate();
    bool saveConf();

    bool stop();
    bool connectTo(WifiProfile network);
    WifiProfile connectingAP();

    bool disconnectNetwork();
    bool removeAllNetworks();

    bool ping();
    bool acquireAddress(bool acquire = true);
    bool assignStaticAddress(const QString & ip,
                             const QString & mask,
                             const QString & gateway,
                             const QString & dns1,
                             const QString & dns2);

    QString address();
    QString hardwareAddress();

Q_SIGNALS:
    void scanResultsReady(WifiProfiles & aps);
    void messageReceived(QByteArray & data);
    void stateChanged(WifiProfile, WpaConnection::ConnectionState state);
    void needPassword(WifiProfile ap);

private Q_SLOTS:
    void receiveMessages();
    void onAddressAcquired(bool);

private:
    QString defaultInterface();

    int  ctrlRequest(const QString & command, QByteArray & result);
    int sendCtrlRequest(wpa_ctrl * ctrl, const QString & command, QByteArray & result);

    int  addNetwork();
    bool setNetworkParam(int id, const char *field, const char * value, bool quote);
    bool removeNetwork(int id);

    bool setWepKey(int network_id, const QString &key, int id);

    void parseScanResult(WifiProfiles & result, QByteArray & data);
    void parseMessage(QByteArray & data);
    bool parseItem(QString & tag, QVariant & value, const QByteArray & data);
    bool parseEntry(const QStringList & title, const QByteArray & data, QVariantMap & result);
    bool encryptionAttributes(WifiProfile & profile);
    bool detailInfo(WifiProfile & profile);

    void broadcastState(ConnectionState state);
    void debugDump(WifiProfiles & all);

    void clearCachedNetworks();
    void updateCachedNetworks();

private:
    QDBusConnection connection_;    ///< Connection to system manager.
    bool wpa_supplicant_connected_;
    QString ctrl_iface_;                  ///< Control interface.
    struct wpa_ctrl *ctrl_conn_;         ///< Control connection to send command.
    struct wpa_ctrl *monitor_conn_;      ///< monitor connection to receive notification.

    WifiProfiles cached_network_;   ///< All networks scanned.
    WifiProfile connecting_ap_;     ///< Access point currently used.

#ifndef _WIN32
    scoped_ptr<QSocketNotifier> notifier_;
#else
    QTimer timer_;
#endif
};

#endif      // WAP_SUPPLICANT_CONNECTION_H_

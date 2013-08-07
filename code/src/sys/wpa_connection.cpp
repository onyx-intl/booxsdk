
#include <QtNetwork/QtNetwork>

#include "onyx/sys/wpa_connection.h"
#include "onyx/sys/platform.h"
#ifdef CONFIG_CTRL_IFACE_UNIX
#include "dirent.h"
#endif

static const char * const INVALID_BSSID = "00:00:00:00:00:00";

static const QString PATH = "path";
static const QString NETWORK_PATH = "network_path";
static const QString IFNAME = "wlan0";
static const QString CTRL_IFACE_DIR = "/var/run/wpa_supplicant";
static const QString NETWORK_ID = "network id";

static const int ERROR_INVALID_PARAMETER = -3;
static const int ERROR_TIMEOUT = -2;
static const int ERROR_FAILED = -1;

static const char SEPARATOR = '\n';
static const int LENGTH = 4096;

static const QString service = "com.onyx.service.system_manager";
static const QString object  = "/com/onyx/object/system_manager";
static const QString iface   = "com.onyx.interface.system_manager";

static const int IMX508_WIFI_QUAL_MAX = 55;
static const int IMX508_WIFI_QUAL_MIN = 14;


#if _WIN32
#define snprintf _snprintf_s
#endif


/// Encryption attributes.
struct EncryptionAttributes
{
    int auth;
    int encrypt;
    int wep;
    std::string key_mgmt;
    std::string proto;
    std::string pairwise;
};


enum
{
    AUTH_NONE = 0,
    AUTH_IEEE8021X = 1,
    AUTH_WPA_PSK = 2,
    AUTH_WPA_EAP = 3,
    AUTH_WPA2_PSK = 4,
    AUTH_WPA2_EAP = 5,
    AUTH_WPS = 6
};



/// WpaConnection constructor.
/// \name The interface name. If the name is empty, WpaConnection
/// would check the interface automatically.
WpaConnection::WpaConnection(const QString &name)
#ifndef _WINDOWS
: connection_(QDBusConnection::systemBus())
#else
: connection_(QDBusConnection::sessionBus())
#endif
, wpa_supplicant_connected_(false)
, ctrl_iface_(name)
, ctrl_conn_(0)
, monitor_conn_(0)
#ifndef _WIN32
, notifier_(0)
#else
, timer_(this)
#endif
{
#ifndef _WIN32
    if (ctrl_iface_.isEmpty())
    {
        ctrl_iface_ = IFNAME;
    }
#endif

    // Setup connection in order to get notification
    // when IP address acquired.
    if (!connection_.connect(service, object, iface,
                             "addressAcquired",
                             this,
                             SLOT(onAddressAcquired(bool))))
    {
        qDebug("\nCan not connect the addressAcquired!\n");
    }
}

WpaConnection::~WpaConnection()
{
}

QString WpaConnection::connectionStateToString(ConnectionState state)
{
    switch (state)
    {
    case STATE_UNKNOWN:
        return "STATE_UNKNOWN";
    case STATE_DISABLED:
        return "STATE_DISABLED";
    case STATE_HARDWARE_ERROR:
        return "STATE_HARDWARE_ERROR";
    case STATE_ABORTED:
        return "STATE_ABORTED";
    case STATE_SCANNING:
        return "STATE_SCANNING";
    case STATE_SCANNED:
        return "STATE_SCANNED";
    case STATE_CONNECTING:
        return "STATE_CONNECTING";
    case STATE_DISCONNECTED:
        return "STATE_DISCONNECTED";
    case STATE_AUTHENTICATION:
        return "STATE_AUTHENTICATION";
    case STATE_AUTHENTICATION_FAILED:
        return "STATE_AUTHENTICATION_FAILED";
    case STATE_CONNECTED:
        return "STATE_CONNECTED";
    case STATE_ACQUIRING_ADDRESS:
        return "STATE_ACQUIRING_ADDRESS";
    case STATE_COMPLETE:
        return "STATE_COMPLETE";
    case STATE_CONNECT_ERROR:
        return "STATE_CONNECT_ERROR";
    case STATE_ACQUIRING_ADDRESS_ERROR:
        return "STATE_ACQUIRING_ADDRESS_ERROR";
    case STATE_TIMEOUT:
        return "STATE_TIMEOUT";
    case STATE_DISCONNECT:
        return "STATE_DISCONNECT";
    default:
        return "STATE_UNKNOWN";
    }
}

QString WpaConnection::defaultInterface()
{
#ifdef CONFIG_CTRL_IFACE_UDP
    ctrl_iface_ = "udp";
#endif

#ifdef CONFIG_CTRL_IFACE_UNIX
    struct dirent *dent;
    DIR *dir = opendir(CTRL_IFACE_DIR.toAscii().constData());
    if (dir)
    {
        while ((dent = readdir(dir)))
        {
//#ifdef _DIRENT_HAVE_D_TYPE
             //Skip the file if it is not a socket.
             //Also accept DT_UNKNOWN (0) in case
             //the C library or underlying file
             //system does not support d_type.
            if (dent->d_type != DT_SOCK &&
                dent->d_type != DT_UNKNOWN)
            {
                continue;
            }
//#endif // _DIRENT_HAVE_D_TYPE

            if (strcmp(dent->d_name, ".") == 0 ||
                strcmp(dent->d_name, "..") == 0)
            {
                continue;
            }
            qDebug("Selected interface '%s'", dent->d_name);
            ctrl_iface_ = dent->d_name;
            break;
        }
        closedir(dir);
    }
#endif

#ifdef CONFIG_CTRL_IFACE_NAMED_PIPE
    struct wpa_ctrl *ctrl;
    ctrl = wpa_ctrl_open(NULL);
    if (ctrl)
    {
        QByteArray result;
        int ret = sendCtrlRequest(ctrl, "INTERFACES", result);
        if (ret >= 0)
        {
            wpa_supplicant_connected_ = true;
            ctrl_iface_ = result;
            ctrl_iface_ = ctrl_iface_.trimmed();
        }
        wpa_ctrl_close(ctrl);
    }
#endif
    return ctrl_iface_;
}

/// Open control connection to wpa supplicant daemon.
/// \name The interface name.
int WpaConnection::openCtrlConnection(const QString & name)
{
    if (!name.isEmpty())
    {
        ctrl_iface_ = name;
    }

    // If no ctrl interface
    if (ctrl_iface_.isEmpty())
    {
        ctrl_iface_ = defaultInterface();
    }
    qDebug("ctrl interface name %s", qPrintable(ctrl_iface_));

    QString cfile;
#ifdef CONFIG_CTRL_IFACE_UNIX
    cfile = CTRL_IFACE_DIR + "/" + ctrl_iface_;
#else
    cfile = ctrl_iface_;
#endif

    if (ctrl_conn_)
    {
        wpa_ctrl_close(ctrl_conn_);
        ctrl_conn_ = NULL;
    }

    if (monitor_conn_)
    {
        wpa_ctrl_detach(monitor_conn_);
        wpa_ctrl_close(monitor_conn_);
        monitor_conn_ = NULL;
    }

    qDebug("Trying to connect to '%s'\n", qPrintable(cfile));
    ctrl_conn_ = wpa_ctrl_open(cfile.toAscii().constData());
    if (ctrl_conn_ == NULL)
    {
        qWarning("ctrl connection is empty.");
        return ERROR_FAILED;
    }
    monitor_conn_ = wpa_ctrl_open(cfile.toAscii().constData());
    if (monitor_conn_ == NULL)
    {
        qWarning("monitor connection is empty.");
        wpa_ctrl_close(ctrl_conn_);
        return ERROR_FAILED;
    }
    if (wpa_ctrl_attach(monitor_conn_))
    {
        qDebug("Failed to attach to wpa_supplicant\n");
        wpa_ctrl_close(monitor_conn_);
        monitor_conn_ = NULL;
        wpa_ctrl_close(ctrl_conn_);
        ctrl_conn_ = NULL;
        return ERROR_FAILED;
    }

#if defined(CONFIG_CTRL_IFACE_UNIX) || defined(CONFIG_CTRL_IFACE_UDP)
    notifier_.reset(0);
    notifier_.reset(new QSocketNotifier(wpa_ctrl_get_fd(monitor_conn_), QSocketNotifier::Read, this));
    connect(notifier_.get(), SIGNAL(activated(int)), SLOT(receiveMessages()));
#endif

#ifdef _WIN32
    connect(&timer_, SIGNAL(timeout()), SLOT(receiveMessages()));
    timer_.setSingleShot(FALSE);
    timer_.start(1000);
#endif

    QByteArray result;
    if (sendCtrlRequest(ctrl_conn_, "INTERFACES", result) >= 0)
    {
        // TODO:
    }

    if (sendCtrlRequest(ctrl_conn_, "GET_CAPABILITY eap", result) >= 0)
    {
        // TODO:
    }

    return 0;
}

bool WpaConnection::update()
{
    QVariantMap info;
    status(info, true);

    WifiProfiles all;
    listNetworks(all);
    return true;
}

/// Retrieve current status.
bool WpaConnection::status(QVariantMap & info, bool broadcast)
{
    QByteArray reply;
    if (ctrlRequest("STATUS", reply) < 0)
    {
        return false;
    }

    QString tag;
    QVariant value;
    QList<QByteArray> items = reply.split(SEPARATOR);
    foreach(QByteArray item, items)
    {
        if (parseItem(tag, value, item))
        {
            // qDebug() << tag << value;
            info[tag] = value;
        }
    }

    // Check wpa_state.
    if (info.value("wpa_state").toByteArray().contains("COMPLETE"))
    {
        if (broadcast)
        {
            broadcastState(WpaConnection::STATE_CONNECTED);

            broadcastState(STATE_ACQUIRING_ADDRESS);
            acquireAddress(true);
        }
    }

    return true;
}

bool WpaConnection::isComplete(bool auto_connect)
{
    QVariantMap info;
    if (!status(info, auto_connect))
    {
        return false;
    }
    if (info.value("wpa_state").toByteArray().contains("COMPLETE"))
    {
        return true;
    }
    return false;
}

bool WpaConnection::isConnectionEstablished()
{
    return isComplete(false);
}

/// List all available networks.
/// \networks The returned network list.
/// \return This function returns true if network list
/// has been updated. Otherwise it returns false.
bool WpaConnection::listNetworks(WifiProfiles &networks)
{
    networks.clear();
    QByteArray reply;
    if (ctrlRequest("LIST_NETWORKS", reply) < 0)
    {
        return false;
    }

    QStringList tags;
    int index = 0;
    QList<QByteArray> lines = reply.split(SEPARATOR);
    QList<QByteArray> items = lines.at(index).split('/');
    foreach(QByteArray item, items)
    {
        tags.push_back(item.trimmed());
    }

    for(int i = 1; i < lines.size(); ++i)
    {
        WifiProfile network;
        if (parseEntry(tags, lines.at(i), network))
        {
            networks.push_back(network);
        }
    }
    return true;
}

/// Trigger scan action. The scan result will be available
/// later.
bool WpaConnection::scan()
{
    broadcastState(WpaConnection::STATE_SCANNING);
    QByteArray result;
    if (ctrlRequest("SCAN", result) < 0)
    {
        return false;
    }

    return result.contains("OK");
}

/// Re-associate the network that stored in previous configuration file.
bool WpaConnection::reAssociate()
{
    QByteArray result;
    if (ctrlRequest("REASSOCIATE", result) < 0)
    {
        return false;
    }
    return result.contains("OK");
}

/// Store current configuration.
bool WpaConnection::saveConf()
{
    QByteArray reply;
    if (ctrlRequest("SAVE_CONFIG", reply) < 0)
    {
        return false;
    }
    return reply.contains("OK");
}

bool WpaConnection::removeAllNetworks()
{
    QByteArray reply;
    if (ctrlRequest("REMOVE_NETWORK all", reply) < 0)
    {
        return false;
    }
    return true;
}

bool WpaConnection::stop()
{
    return disconnectNetwork();
}

/// Connect to specified access point.
bool WpaConnection::connectTo(WifiProfile ap)
{
    if (ap.bssid().contains(INVALID_BSSID) && ap.ssid().isEmpty())
    {
        return false;
    }

    // Always remove all networks.
    disconnectNetwork();
    removeAllNetworks();

    // Update state.
    broadcastState(WpaConnection::STATE_CONNECTING);

    // Add new network.
    int id = addNetwork();
    setNetworkParam(id, "ssid", ap.ssid().toAscii().constData(), true);

    // To connect to hidden ssid. Seems work.
    setNetworkParam(id, "ap_scan", "1", false);
    setNetworkParam(id, "scan_ssid", "1", false);
    
    // timeout.
    setNetworkParam(id, "dot11RSNAConfigSATimeout", "5", false);

    // encryption.
    encryptionAttributes(ap);
    connecting_ap_ = ap;

    // Check ap
    if (ap.isWpa() || ap.isWpa2())
    {
        if (ap.psk().isEmpty())
        {
            emit needPassword(ap);
            return false;
        }
    }
    else if (ap.isWep())
    {
        if (ap.wepKey1().isEmpty() && ap.wepKey2().isEmpty() &&
            ap.wepKey3().isEmpty() && ap.wepKey4().isEmpty())
        {
            emit needPassword(ap);
            return false;
        }
    }

    if (!ap.proto().isEmpty())
    {
        setNetworkParam(id, "proto", ap.proto().toAscii().constData(), false);
    }
    if (!ap.key_mgmt().isEmpty())
    {
        setNetworkParam(id, "key_mgmt", ap.key_mgmt().toAscii().constData(), false);
    }
    if (!ap.pairwise().isEmpty())
    {
        setNetworkParam(id, "pairwise", ap.pairwise().toAscii().constData(), false);
        setNetworkParam(id, "group", "TKIP CCMP WEP104 WEP40", false);
    }

    // Check wpa or wpa2
    if (ap.key_mgmt() == "WPA-PSK")
    {
        setNetworkParam(id, "psk", ap.psk().toAscii().constData(), true);
    }
    else if (ap.isWep())
    {
        setWepKey(id, ap.wepKey1(), 0);
        setWepKey(id, ap.wepKey2(), 1);
        setWepKey(id, ap.wepKey3(), 2);
        setWepKey(id, ap.wepKey4(), 3);

        if (!ap.wepKey1().isEmpty())
        {
            setNetworkParam(id, "wep_tx_keyidx", "0", false);
        }
        else if (!ap.wepKey2().isEmpty())
        {
            setNetworkParam(id, "wep_tx_keyidx", "1", false);
        }
        else if (!ap.wepKey3().isEmpty())
        {
            setNetworkParam(id, "wep_tx_keyidx", "2", false);
        }
        else if (!ap.wepKey4().isEmpty())
        {
            setNetworkParam(id, "wep_tx_keyidx", "3", false);
        }
        //setNetworkParam(id, "phase2", "NULL", false);
        //setNetworkParam(id, "password", "NULL", false);
        //setNetworkParam(id, "id_str", "NULL", false);
        //setNetworkParam(id, "ca_cert", "NULL", false);

        // It would be necessary, but not sure yet.
        // From testing, without shared attribute, we can not
        // connect to wep shared access point.
        // It could be different if the router uses different options.
        if (ap.authAlg().length() > 0)
        {
            setNetworkParam(id, "auth_alg" , ap.authAlg().toStdString().c_str(), false);
        }
    }

    // Enable network now.
    QByteArray reply;
    QString cmd("ENABLE_NETWORK %1");
    cmd = cmd.arg(id);
    if (ctrlRequest(cmd, reply) < 0)
    {
        return false;
    }

    // Need to invoke reAssociate to start associate.
    reAssociate();

    // Update status. Actually, it may be not necessary.
    // Just to update status immediately. The status is changed
    // slow on embedded device.
    return update();
}

WifiProfile WpaConnection::connectingAP()
{
    return connecting_ap_;
}

/// Disconnect from network.
bool WpaConnection::disconnectNetwork()
{
    broadcastState(WpaConnection::STATE_DISCONNECT);
    connecting_ap_.clear();
    QByteArray reply;
    if (ctrlRequest("DISCONNECT", reply) < 0)
    {
        return false;
    }
    return true;
}

/// Update encryption attributes of specified access point.
bool WpaConnection::encryptionAttributes(WifiProfile & profile)
{
    // Ignore if profile is defined by end user.
    if (profile.isDefinedByUser())
    {
        return true;
    }

    int auth = 0, encrypt = 0;
    QString flags = profile.value("flags").toString();

    if (flags.indexOf("[WPA2-EAP") >= 0)
        auth = AUTH_WPA2_EAP;
    else if (flags.indexOf("[WPA-EAP") >= 0)
        auth = AUTH_WPA_EAP;
    else if (flags.indexOf("[WPA2-PSK") >= 0)
        auth = AUTH_WPA2_PSK;
    else if (flags.indexOf("[WPA-PSK") >= 0)
        auth = AUTH_WPA_PSK;
    else
        auth = AUTH_NONE;

    if (flags.indexOf("CCMP") >= 0)
        encrypt = 1;
    else if (flags.indexOf("TKIP") >= 0)
        encrypt = 0;
    else if (flags.indexOf("WEP") >= 0)
        encrypt = 1;
    else
        encrypt = 0;

    profile.setWep(auth == AUTH_NONE && encrypt == 1);

    switch (auth)
    {
    case AUTH_NONE:
        profile.setKeyMgmt("NONE");
        break;
    case AUTH_IEEE8021X:
        profile.setKeyMgmt("IEEE8021X");
        break;
    case AUTH_WPA_PSK:
        profile.setKeyMgmt("WPA-PSK");
        profile.setProto("WPA");
        break;
    case AUTH_WPA_EAP:
        profile.setKeyMgmt("WPA-EAP");
        profile.setProto("WPA");
        break;
    case AUTH_WPA2_PSK:
        profile.setKeyMgmt("WPA-PSK");
        profile.setProto("WPA2");
        break;
    case AUTH_WPA2_EAP:
        profile.setKeyMgmt("WPA-EAP");
        profile.setProto("WPA2");
        break;
    }

    bool ccmp = (flags.indexOf("CCMP") >= 0);
    bool tkip = (flags.indexOf("TKIP") >= 0);
    if (auth == AUTH_WPA_PSK ||
        auth == AUTH_WPA_EAP)
    {
        profile.setWpa(true);
        if (ccmp && tkip)
        {
            profile.setPairwise("TKIP CCMP");
        }
        else if (ccmp && !tkip)
        {
            profile.setPairwise("CCMP");
        }
        else if (!ccmp && tkip)
        {
            profile.setPairwise("TKIP");
        }
    }

    if (auth == AUTH_WPA2_PSK ||
        auth == AUTH_WPA2_EAP)
    {
        profile.setWpa2(true);
        if (ccmp && tkip)
        {
            profile.setPairwise("TKIP CCMP");
        }
        else if (ccmp && !tkip)
        {
            profile.setPairwise("CCMP");
        }
        else if (!ccmp && tkip)
        {
            profile.setPairwise("TKIP");
        }
    }
    return true;
}

/// Query more details information of access point.
bool WpaConnection::detailInfo(WifiProfile & profile)
{
    if (cached_network_.size() <= 0)
    {
        updateCachedNetworks();
    }

    // Query the cached network for details.
    foreach(WifiProfile ap, cached_network_)
    {
        if (profile.bssid() == ap.bssid())
        {
            // Update nosie, qual and level
            // qDebug("profile ssid %s", qPrintable(profile.ssid()));
            // qDebug("ap ssid %s", qPrintable(ap.ssid()));
            profile.setQuality(ap.quality());
            profile.setLevel(ap.level());
            profile.setNoise(ap.noise());

            if (sys::isImx508())
            {
                int offset = IMX508_WIFI_QUAL_MAX-IMX508_WIFI_QUAL_MIN;
                double d_real_qual = (double)(ap.quality()-IMX508_WIFI_QUAL_MIN)/(double)offset;

                int real_qual = d_real_qual*100+1;
                if (real_qual < 0)
                {
                    real_qual = 0;
                }
                qDebug() << "snr value: " << ap.quality();
                qDebug() << "quality: " << real_qual;
                profile.setQuality(real_qual);
            }

            return true;
        }
    }
    return false;
}

/// Broadcast state to all listeners.
void WpaConnection::broadcastState(ConnectionState state)
{
    // Some post process.
    if (state == WpaConnection::STATE_CONNECTED)
    {
        saveConf();
    }

    emit stateChanged(connecting_ap_, state);
}

/// Dump.
void WpaConnection::debugDump(WifiProfiles & all)
{
    foreach(WifiProfile profile, all)
    {
        profile.debugDump();
    }
}

/// Update cached networks.
void WpaConnection::updateCachedNetworks()
{
    clearCachedNetworks();
    for(int i = 0; i < 200; ++i)
    {
        QString command("BSS %1");
        command = command.arg(i);
        QByteArray reply;
        if (ctrlRequest(command, reply) < 0)
        {
            continue;
        }
        if (!reply.contains(SEPARATOR))
        {
            break;
        }

        WifiProfile ap;
        QString tag;
        QVariant value;
        QList<QByteArray> lines = reply.split(SEPARATOR);
        foreach(QByteArray line, lines)
        {
            if (parseItem(tag, value, line))
            {
                ap.insert(tag, value);
            }
        }
        cached_network_.push_back(ap);
    }
}

void WpaConnection::clearCachedNetworks()
{
    cached_network_.clear();
}

/// Send control request. This function can receive result immediately.
/// If it does not receive any messages, the callback function will
/// be invoked later.
int WpaConnection::ctrlRequest(const QString & command, QByteArray & result)
{
    return sendCtrlRequest(ctrl_conn_, command, result);
}

void WpaConnection::onAddressAcquired(bool ok)
{
    if (!address().isEmpty())
    {
        broadcastState(STATE_COMPLETE);
        return;
    }
    broadcastState(STATE_ACQUIRING_ADDRESS_ERROR);
}

/// Receive messages from wpa supplicant.
void WpaConnection::receiveMessages()
{
    size_t length = LENGTH;
    QByteArray result;
    result.resize(length);

    while (monitor_conn_ && wpa_ctrl_pending(monitor_conn_) > 0)
    {
        if (wpa_ctrl_recv(monitor_conn_, result.data(), &length) == 0)
        {
            result[static_cast<uint>(length)] = 0;
            parseMessage(result);
        }
    }
}

void WpaConnection::parseMessage(QByteArray & data)
{
    qDebug() << "message received: " << data;

    if (data.contains(WPA_EVENT_SCAN_RESULTS))
    {
        WifiProfiles aps;
        scanResults(aps);
        emit scanResultsReady(aps);
        broadcastState(STATE_SCANNED);
    }
    else if (data.contains("Associated with"))
    {
        // Means connecting..., but does not need to broadcast any message here.
        // broadcastState(STATE_CONNECTING);
        update();
    }
    else if (data.contains(WPA_EVENT_DISCONNECTED) && !data.contains(INVALID_BSSID))
    {
        // Incorrect password.
        // "WPA: 4-Way Handshake failed - pre-shared key may be incorrect"))
        broadcastState(STATE_DISCONNECTED);
    }
    else if (data.contains("Authentication with"))
    {
        if (data.contains("timed out"))
        {
            // key error or timeout, but we need to check bssid now.
            // It depends on access point type.
            // When connecting wep (shared/open), the bssid could be invalid.
            if (!data.contains(INVALID_BSSID) || connecting_ap_.isWep())
            {
                broadcastState(STATE_AUTHENTICATION_FAILED);
            }
        }
    }
    else if (data.contains(WPA_EVENT_CONNECTED) ||
             data.contains("Key negotiation completed"))
    {
        // Connected.
        broadcastState(STATE_CONNECTED);

        // Now try to acquire ip address if dhcp is enabled.
        broadcastState(STATE_ACQUIRING_ADDRESS);
        acquireAddress(true);
    }
    else if (data.contains("WPA: 4-Way Handshake failed"))
    {
        broadcastState(STATE_AUTHENTICATION_FAILED);
    }
}

/// To query the wpa_supplicant to update scan results.
bool WpaConnection::scanResults(WifiProfiles & aps)
{
    QByteArray reply;
    QString cmd("SCAN_RESULTS");
    if (ctrlRequest(cmd, reply) < 0)
    {
        return false;
    }

    clearCachedNetworks();
    parseScanResult(aps, reply);
    return true;
}

void WpaConnection::parseScanResult(WifiProfiles & result,
                                    QByteArray & data)
{
    QString tag;
    QVariant value;
    QList<QByteArray> lines = data.split(SEPARATOR);

    // first line
    // data = 0x02ec8350 "bssid / frequency / signal level / flags / ssid"
    QStringList tags;
    int index = 0;
    QList<QByteArray> items = lines.at(index).split('/');
    foreach(QByteArray item, items)
    {
        tags.push_back(item.trimmed());
    }

    // other lines.
    // data = 0x02fc27a8 "00:24:01:1e:55:e0	2412	-49	[WPA-PSK-TKIP+CCMP][WPA2-PSK-TKIP+CCMP][WPS]	tiger"
    for(index = 1; index < lines.size(); ++index)
    {
        WifiProfile profile;
        if (parseEntry(tags, lines.at(index), profile))
        {
            // Update attributes according to flag.
            encryptionAttributes(profile);
            detailInfo(profile);

            // Always add the scan result to result list no matter the bssid is valid or not.
            // if (!profile.bssid().contains(INVALID_BSSID))
            {
                result.push_back(profile);
            }
        }
    }

    //debugDump(result);
}

bool WpaConnection::parseItem(QString & tag, QVariant & value, const QByteArray & data)
{
    int pos = data.indexOf('=');
    if (pos < 0)
    {
        return false;
    }
    tag = data.left(pos);
    value = data.mid(pos + 1);
    return true;
}

bool WpaConnection::parseEntry(const QStringList & tags,
                               const QByteArray & data,
                               QVariantMap & result)
{
    if (!data.contains('\t'))
    {
        // qDebug("ParseEntry error: No tab found %s.", data.constData());
        return false;
    }

    // Parse the entry
    // data = 0x02fc27a8 "00:24:01:1e:55:e0	2412	-49	[WPA-PSK-TKIP+CCMP][WPA2-PSK-TKIP+CCMP][WPS]	tiger"
    QList<QByteArray> items = data.split('\t');
    if (items.size() != tags.size())
    {
        // qDebug("ParseEntry error: unmatched size. %s", data.constData());
        return false;
    }

    for(int i = 0; i < items.size(); ++i)
    {
        result[tags[i]] = items.at(i);
        // qDebug("tag %s value %s", qPrintable(tags[i]), items.at(i).constData());
    }
    return true;
}

bool WpaConnection::ping()
{

#ifdef CONFIG_CTRL_IFACE_NAMED_PIPE
    receiveMessages();
    return true;
#endif

    QByteArray reply;
    if (ctrlRequest("PING", reply) < 0)
    {
        qWarning("PING failed - trying to reconnect.");
        if (openCtrlConnection(ctrl_iface_) >= 0)
        {
            qDebug("Reconnected successfully.");
            return true;
        }
    }
    return false;

#ifndef CONFIG_CTRL_IFACE_NAMED_PIPE

#endif
}

/// Call ifup eth0 to acquire address.
bool WpaConnection::acquireAddress(bool acquire)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "acquireAddress"      // method.
    );
    message << acquire;

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        QList<QVariant> args = reply.arguments();
        if (args.size() > 0)
        {
            return args.at(0).toBool();
        }
        return false;
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool WpaConnection::assignStaticAddress(const QString & ip,
                                        const QString & mask,
                                        const QString & gateway,
                                        const QString & dns1,
                                        const QString & dns2)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "assignStaticAddress"      // method.
    );
    message << ip;
    message << mask;
    message << gateway;
    message << dns1;
    message << dns2;

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        QList<QVariant> args = reply.arguments();
        if (args.size() > 0)
        {
            return args.at(0).toBool();
        }
        return false;
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

QString WpaConnection::address()
{
    QString result;
    QList<QNetworkInterface> all = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface ni, all)
    {
        qDebug("interface name %s", qPrintable(ni.name()));
        QList<QNetworkAddressEntry> addrs = ni.addressEntries();
        foreach(QNetworkAddressEntry entry, addrs)
        {
            if (ni.name().compare(ctrl_iface_, Qt::CaseInsensitive) == 0)
            {
                result = entry.ip().toString();
            }
            qDebug("ip address %s", qPrintable(entry.ip().toString()));
        }
    }
    return result;
}

QString WpaConnection::hardwareAddress()
{
    QString result;
    QList<QNetworkInterface> all = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface ni, all)
    {
        qDebug("interface name %s", qPrintable(ni.name()));
        if (ni.name().compare(ctrl_iface_, Qt::CaseInsensitive) == 0)
        {
            return ni.hardwareAddress();
        }
    }
    return result;
}

int WpaConnection::sendCtrlRequest(wpa_ctrl * ctrl,
                                   const QString & command,
                                   QByteArray & result)
{
    if (ctrl == 0)
    {
        return ERROR_INVALID_PARAMETER;
    }

    size_t length = LENGTH;
    result.resize(length);
    int ret = wpa_ctrl_request(ctrl, command.toAscii().constData(), command.length(),
                               result.data(), &length, 0);

    if (ret == ERROR_TIMEOUT)
    {
        qWarning("Command %s timed out.", qPrintable(command));
    }
    else if (ret < 0)
    {
        qWarning("Command %s failed.", qPrintable(command));
    }
    result[static_cast<uint>(length)] = 0;
    return ret;
}

/// Return id if ok.
int WpaConnection::addNetwork()
{
    QByteArray reply;
    if (ctrlRequest("ADD_NETWORK", reply) < 0)
    {
        return -1;
    }

    if (reply.startsWith('F'))
    {
        return -1;
    }

    return reply.toInt();
}

/// Remove the network specified by the id.
bool WpaConnection::removeNetwork(int id)
{
    QString cmd("REMOVE_NETWORK %1");
    cmd = cmd.arg(id);

    QByteArray reply;
    if (ctrlRequest(cmd, reply) < 0)
    {
        return false;
    }

    return reply.contains("OK");
}

bool WpaConnection::setWepKey(int network_id, const QString &key, int id)
{
    if (key.isEmpty())
    {
        return true;
    }

    char buf[10] = {0};
    bool hex = false;
    std::string txt;
    //
    // Assume hex key if only hex characters are present and length matches
    // with 40, 104, or 128-bit key
    //
    txt = key.toStdString();
    hex = true;
    size_t i = 0;
    while (i < txt.length())
    {
        if (!((txt[i] >= '0' && txt[i] <= '9') ||
            (txt[i] >= 'a' && txt[i] <= 'f') ||
            (txt[i] >= 'A' && txt[i] <= 'F')))
        {
            hex = false;
            break;
        }
        ++i;
    }
    if (hex && txt.length() != 10 && txt.length() != 26 && txt.length() != 32)
    {
        hex = false;
    }
    snprintf(buf, sizeof(buf), "wep_key%d", id);
    return setNetworkParam(network_id, buf, txt.c_str(), !hex);
}

bool WpaConnection::setNetworkParam(int id, const char *field,
                                    const char *value, bool quote)
{
    QByteArray reply;
    char cmd[256] = {0};
    snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s %s%s%s",
              id, field, quote ? "\"" : "", value, quote ? "\"" : "");
    if (ctrlRequest(cmd, reply) < 0)
    {
        return false;
    }
    return reply.contains("OK");
}

// Authors: John

/// Public header of the system configuration library.

#ifndef SYSTEM_CONF_H__
#define SYSTEM_CONF_H__


#include "service_conf.h"
#include "locale_conf.h"
#include "font_conf.h"
#include "wifi_conf.h"
#include "dialup_conf.h"
#include "misc_conf.h"

using namespace base;

namespace sys
{

class SystemConfig
{
public:
    SystemConfig();
    ~SystemConfig();

public:
    bool open();
    bool close();

    // Services session.
    void loadAllServices(Services &);
    bool calibrationService(Service &);
    bool metService(Service &);
    bool musicService(Service &);
    bool mediaService(Service &);
    bool noteService(Service &);
    bool networkService(Service &);
    bool webBrowserService(Service &);
    bool DRMService(Service &);
    bool messengerService(Service &);
    bool writePadService(Service &);
    bool dictionaryService(Service &);
    bool rssService(Service &);
    bool sudokuService(Service &);
    bool news_feeds_Service(Service &);
    bool officeViewerService(Service & service);
    bool hasOfficeViewer();
    bool onyxReaderService(Service & service);
    bool nabooReaderService(Service &  service);
    bool onyxPdfReaderService(Service &  service);
    bool coolReaderService(Service & service);
    bool htmlReaderService(Service & service);
    bool registerService(const Service &, const QString &);
    bool unRegisterService(const Service &);

    // Locale
    QLocale locale();
    bool setLocale(const QLocale & locale);

    // Font Family
    QString defaultFontFamily();
    void setDefaultFontFamily(const QString & name);

    // Dictionary directory.
    bool dictionaryRoots(QStringList & dirs);

    QString selectedDictionary();
    bool selectDictionary(const QString & name);

    // Time and date.
    static bool setTimezone(const QString & name);
    static QString currentTimezone();
    static void setDate(int year, int month, int day,
                        int hour, int minute, int second);

    // Music service
    static bool isMusicPlayerAvailable();

    // Media player.
    static bool isMediaPlayerAvailable();

    // volume.
    int volume();
    bool setVolume(const int);
    QVector<int> volumes();
    int minVolume();
    int maxVolume();

    bool mute(bool mute);
    bool isMute();

    // The suspend interval in ms. 0 means it's disabled.
    int suspendInterval();
    bool setSuspendInterval(int ms);

    // The shutdown interval in ms. 0 means it's disabled.
    int shutdownInterval();
    bool setShutdownInterval(int ms);

    // Wifi configuration.
    bool clearWifiProfiles();
    bool loadWifiProfiles(WifiProfiles & all);
    bool saveWifiProfiles(WifiProfiles & all);
    QString wifiHardwareAddress();
    QString saveWifiHardwareAddr(const QString &wifi_hardware_addr);

    // Dialup configuration.
    bool clearDialupProfiles();
    bool loadDialupProfiles(DialupProfiles & all);
    bool saveDialupProfiles(DialupProfiles & all);

    static QString defaultPincode();
    static void setDefaultPincode(const QString &);

    // Use MiscConfig
    bool setMiscValue(const QString &key, const QString &value);
    QString miscValue(const QString &key);
    int screenUpdateGCInterval();
    int screenUpdateGrayScaleSetting();

    // Page turning direction.
    static int direction(const QPoint & old_position, const QPoint & new_position);
    static void setDirection(int);
    static int distance();
    static int whichArea(const QPoint & old_position, const QPoint & new_position);

    // Internet settings.
    QString downloadFolder();

    // Device information.
    QString serialNumber();
    QString deviceId();
    QString setSerialNumber(const QString &str);
    QString setDeviceId(const QString &device_id);
    static QString version();
    static QString otaPathName();

    QString cpuInfo();
    QString memInfo();
    QString flashInfo();

    // Enable to change boot splash.
    static bool isUpdateSplashEnabled();

    // Get customized home page.
    static bool hasHomePage();
    static const QString & homePageName();
    static QString homeUrl();
    static QString getServerHost();

    // Notes.
    static bool isImage(const QString& suffix);
    static QString notesTemplate(int index = -1);
    static bool notesExportDirectory(const QString & note_name, QDir & dir);
    static bool exportNotes(const QString & src, const QString & dst);

    // Mount tree
    static bool isSDMounted();
    static bool isFlashMounted();
    static bool isMounted(const std::string & point);

    // show browsing history or not
    static bool showBrowsingHistory();

    static QString defaultAccessPoint();

    // Firware Update
    void askMeForUpdate(bool askme);
    bool askMeForUpdate();

    void installUpdate(bool install);
    bool installUpdate();

private:
    scoped_ptr<QSqlDatabase> database_;
};

};

#endif  // SYSTEM_CONF_H__

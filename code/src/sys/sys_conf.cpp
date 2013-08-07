#include "onyx/sys/sys_conf.h"

#include <stdlib.h>
#ifndef _WINDOWS
#include <mntent.h>
#endif

#include "onyx/base/device.h"
#include "onyx/sys/dict_conf.h"
#include "onyx/sys/page_turning_conf.h"
#include "onyx/sys/pm_conf.h"
#include "onyx/sys/volume_conf.h"
#include "onyx/sys/sys_utils.h"
#include "onyx/sys/font_conf.h"
#include "onyx/sys/misc_conf.h"
#include "onyx/sys/platform.h"
#include "device_conf.h"

namespace sys
{

SystemConfig::SystemConfig()
{
    open();

    DictConfig::makeSureTableExist(*database_);
    PMConfig::makeSureTableExist(*database_);
    LocaleConfig::makeSureTableExist(*database_);
    ServiceConfig::makeSureTableExist(*database_);
    VolumeConfig::makeSureTableExist(*database_);
    WifiConfig::makeSureTableExist(*database_);
    DialupConfig::makeSureTableExist(*database_);
    DeviceConfig::makeSureTableExist(*database_);
    PageTurningConfig::makeSureTableExist(*database_);
    FontConfig::makeSureTableExist(*database_);
    MiscConfig::makeSureTableExist(*database_);
}

SystemConfig::~SystemConfig()
{
    close();
}

void SystemConfig::loadAllServices(Services &services)
{
    ServiceConfig::loadAllServices(*database_, services);
}

/// Retrieve the calibration service.
bool SystemConfig::calibrationService(Service & service)
{
    return ServiceConfig::calibrationService(*database_, service);
}

bool SystemConfig::metService(Service &service)
{
    return ServiceConfig::metService(*database_, service);
}

bool SystemConfig::musicService(Service & service)
{
    return ServiceConfig::musicService(*database_, service);
}

bool SystemConfig::mediaService(Service &service)
{
    return ServiceConfig::mediaService(*database_, service);
}

bool SystemConfig::noteService(Service &service)
{
    return ServiceConfig::noteService(*database_, service);
}

bool SystemConfig::networkService(Service &service)
{
    return ServiceConfig::networkService(*database_, service);
}

bool SystemConfig::webBrowserService(Service &service)
{
    return ServiceConfig::webBrowserService(*database_, service);
}

bool SystemConfig::DRMService(Service &service)
{
    return ServiceConfig::DRMService(*database_, service);
}

bool SystemConfig::messengerService(Service & service)
{
    return ServiceConfig::messengerService(*database_, service);
}

bool SystemConfig::writePadService(Service &service)
{
    return ServiceConfig::writePadService(*database_, service);
}

bool SystemConfig::dictionaryService(Service &service)
{
    return ServiceConfig::dictionaryService(*database_, service);
}

bool SystemConfig::rssService(Service &service)
{
    return ServiceConfig::rssService(*database_, service);
}

bool SystemConfig::news_feeds_Service(Service &service)
{
    return ServiceConfig::news_feeds_Service(*database_, service);
}

bool SystemConfig::sudokuService(Service &service)
{
    return ServiceConfig::sudokuService(*database_, service);
}

bool SystemConfig::officeViewerService(Service & service)
{
    return ServiceConfig::officeViewerService(*database_, service);
}

bool SystemConfig::hasOfficeViewer()
{
    return ServiceConfig::hasOfficeViewer();
}

bool SystemConfig::onyxReaderService(Service & service)
{
    return ServiceConfig::onyxReaderService(*database_, service);
}

bool SystemConfig::coolReaderService(Service & service)
{
    return ServiceConfig::coolReaderService(*database_, service);
}

bool SystemConfig::htmlReaderService(Service & service)
{
    return ServiceConfig::htmlReaderService(*database_, service);
}

bool SystemConfig::nabooReaderService(Service & service)
{
    return ServiceConfig::nabooReaderService(*database_, service);
}

bool SystemConfig::onyxPdfReaderService(Service &service)
{
    return ServiceConfig::onyxPdfReaderService(*database_, service);
}

bool SystemConfig::registerService(const Service &service,
                                   const QString &path)
{
    return ServiceConfig::registerService(*database_, service, path);
}

bool SystemConfig::unRegisterService(const Service &service)
{
    return ServiceConfig::unRegisterService(*database_, service);
}

/// The database is in home directory.
bool SystemConfig::open()
{
    if (!database_)
    {
        if (QSqlDatabase::contains("system_config"))
        {
            database_.reset(new QSqlDatabase(QSqlDatabase::database("system_config")));
        }
        else
        {
            database_.reset(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "system_config")));
        }
        database_->setDatabaseName(QDir::home().filePath("system_config.db"));
    }
    return database_->open();
}

bool SystemConfig::close()
{
    if (database_)
    {
//        database_->close();
        database_.reset(0);
//        QSqlDatabase::removeDatabase("system_config");
        return true;
    }
    return false;
}

// Locale
QLocale SystemConfig::locale()
{
    return LocaleConfig::locale(*database_);
}

bool SystemConfig::setLocale(const QLocale & locale)
{
    return LocaleConfig::setLocale(*database_, locale);
}

void SystemConfig::setDefaultFontFamily(const QString & name)
{
    return FontConfig::setDefaultFontFamily(*database_, name);
}

QString SystemConfig::defaultFontFamily()
{
    return FontConfig::defaultFontFamily(*database_);
}

bool SystemConfig::dictionaryRoots(QStringList & dirs)
{
    return DictConfig::dictionaryRoots(*database_, dirs);
}

/// Read the dictionary name that user selected.
QString SystemConfig::selectedDictionary()
{
    return DictConfig::selectedDictionary(*database_);
}

/// Save the dictionary name as the selected.
bool SystemConfig::selectDictionary(const QString & name)
{
    return DictConfig::selectDictionary(*database_, name);
}

static const QString ZONE_PREFIX = "/usr/share/zoneinfo/";
/// See /usr/share/zoneinfo for the list of valid names.
/// The name should contain all relative path like Asia/Chongqing.
bool SystemConfig::setTimezone(const QString & name)
{
    QString format("%1%2");
    format = format.arg(ZONE_PREFIX).arg(name);

    if (QFile::exists(format))
    {
        QStringList args;
        args << format;
        QProcess::startDetached("update_timezone.sh", args);
        return true;
    }
    return false;
}

QString SystemConfig::currentTimezone()
{
    // Check default link at first.
    const QString defaultLink = "/etc/localtime";
    QFileInfo info(defaultLink);
    if (info.exists() && info.isSymLink())
    {
        QString path = info.symLinkTarget();
        return path.remove(ZONE_PREFIX);
    }

    // Check TZ now.
    QString path = qgetenv("TZ");
    if (QFile::exists(path))
    {
        return path.remove(ZONE_PREFIX);
    }
    return QString();
}

/// Change system date.
/// The steps should be:
/// - Change system date by using date -s 2009.04.27-12:10:20
/// - Change the hardware clock by using hwclock -w
void SystemConfig::setDate(int year, int month, int day,
                           int hour, int minute, int second)
{
    QString format("%1.%2.%3-%4:%5:%6");
    format = format.arg(year).arg(month).arg(day).arg(hour).arg(minute).arg(second);

    QStringList args;
    args << format;
    QProcess::startDetached("update_date.sh", args);
}

bool SystemConfig::isMusicPlayerAvailable()
{
    return system("which music_player") == 0;
}

bool SystemConfig::isMediaPlayerAvailable()
{
    return system("which onyx_media_player") == 0;
}

bool SystemConfig::isUpdateSplashEnabled()
{
    return system("which update_splash") == 0;
}

bool SystemConfig::hasHomePage()
{
    return false;
}

const QString & SystemConfig::homePageName()
{
    static QString name;
    return name;
}

QString SystemConfig::homeUrl()
{
    return QString();
}

/// Check the suffix is a image suffix or not.
bool SystemConfig::isImage(const QString& suffix)
{
    static QList<QString> supported_formats;
    if (supported_formats.size() <= 0)
    {
        QList<QByteArray> list = QImageReader::supportedImageFormats();
        for(QList<QByteArray>::iterator it = list.begin(); it != list.end(); ++it)
        {
            QString ext(*it);
            ext = ext.toLower();
            supported_formats.push_back(ext);
        }
    }
    return supported_formats.contains(suffix.toLower());
}

QString SystemConfig::notesTemplate(int index)
{
    // Check SD at first.
    const QString NOTES_TEMPLATE = "notes_template";
    QDir dir(SDMMC_ROOT);
    if (!dir.cd(NOTES_TEMPLATE))
    {
        dir = QDir::home();
        if (!dir.cd(NOTES_TEMPLATE))
        {
            dir = QDir("/usr/share/");
            dir.cd(NOTES_TEMPLATE);
        }
    }

    QDir::Filters filters = QDir::Files|QDir::NoDotAndDotDot;
    QFileInfoList all = dir.entryInfoList(QStringList(), filters);
    if (index >= all.size() || index < 0)
    {
        index = 0;
    }

    while (index < all.size())
    {
        if (isImage(all.at(index).suffix()))
        {
            return all.at(index).absoluteFilePath();
        }
        ++index;
    }

    if (index >= all.size())
    {
        qWarning("Should not happen. %s", qPrintable(dir.path()));
    }
    return QString();
}

/// By default, the order of export directory is:
/// SD card/notes
/// Flash/notes
bool SystemConfig::notesExportDirectory(const QString & note_name, QDir & dir)
{
    const QString NOTES = "notes";

    // Check sd card at first.
    if (isSDMounted())
    {
        dir.cd(SDMMC_ROOT);
        if (!dir.exists(NOTES))
        {
            dir.mkdir(NOTES);
        }
        if (dir.cd(NOTES))
        {
            QString name_for_dir = note_name;
            name_for_dir = name_for_dir.replace(":", "_");
            if (!dir.exists(name_for_dir))
            {
                dir.mkdir(name_for_dir);
            }
            if (dir.cd(name_for_dir))
            {
                return true;
            }
        }
    }

    // Check flash now.
    if (isFlashMounted())
    {
        dir.cd(LIBRARY_ROOT);
        if (!dir.exists(NOTES))
        {
            dir.mkdir(NOTES);
        }
        if (dir.cd(NOTES))
        {
            QString name_for_dir = note_name;
            name_for_dir = name_for_dir.replace(":", "_");
            if (!dir.exists(name_for_dir))
            {
                dir.mkdir(name_for_dir);
            }
            if (dir.cd(name_for_dir))
            {
                return true;
            }
        }
    }

    qWarning("Can not find export directory.");
    return false;
}

/// Export notes from source to destination.
bool SystemConfig::exportNotes(const QString & src, const QString & dst)
{
    return QFile::copy(src, dst);
}

bool SystemConfig::isSDMounted()
{
    return isMounted(SDMMC_DEVICE);
}

bool SystemConfig::isFlashMounted()
{
    if (is166E())
    {
        return isMounted("/media/mtd8/vfat.bin");
    }
    else if (isImx508())
    {
        return isMounted("/dev/mmcblk0p3");
    }

    return isMounted("/dev/mtdblock5");
}

bool SystemConfig::isMounted(const std::string & point)
{
#ifndef _WINDOWS
    const char* MOUNT_FS_DESC_FILE  = "/proc/mounts";

    // Initialize the local mount entries.
    FILE* mount_fp = setmntent(MOUNT_FS_DESC_FILE, "r");

    if (mount_fp == NULL)
    {
        qFatal("Can't get mount point list %s", MOUNT_FS_DESC_FILE);
        return false;
    }

    // Loop over /proc/mounts
    struct mntent* mnt_entry = NULL;
    while ((mnt_entry = getmntent(mount_fp)))
    {
        // We get a mount entry in /proc/mounts
        // qDebug("%s\t%s", mnt_entry->mnt_fsname,  mnt_entry->mnt_dir);
        if (point == mnt_entry->mnt_fsname)
        {
            return true;
        }
    }

    // Finish with mount_fp.
    endmntent(mount_fp);
#endif
    return false;
}

/// Get volume. We always read the volume from database.
/// It's necessary as this function is called after startuped.
/// At that time, the volume from hardware is undefined.
int SystemConfig::volume()
{
    return VolumeConfig::volume(*database_);
}

bool SystemConfig::setVolume(const int v)
{
    return VolumeConfig::setVolume(*database_, v);
}

bool SystemConfig::mute(bool m)
{
    return VolumeConfig::mute(*database_, m);
}

QVector<int> SystemConfig::volumes()
{
    return VolumeConfig::volumes();
}

int SystemConfig::minVolume()
{
    return VolumeConfig::minVolume();
}

int SystemConfig::maxVolume()
{
    return VolumeConfig::maxVolume();
}

bool SystemConfig::isMute()
{
    return VolumeConfig::isMute(*database_);
}

int SystemConfig::suspendInterval()
{
    return PMConfig::suspendInterval(*database_);
}

bool SystemConfig::setSuspendInterval(int ms)
{
    return PMConfig::setSuspendInterval(*database_, ms);
}

int SystemConfig::shutdownInterval()
{
    return PMConfig::shutdownInterval(*database_);
}

bool SystemConfig::setShutdownInterval(int ms)
{
    return PMConfig::setShutdownInterval(*database_, ms);
}

bool SystemConfig::clearWifiProfiles()
{
    return WifiConfig::clear(*database_);
}

bool SystemConfig::loadWifiProfiles(WifiProfiles & all)
{
    return WifiConfig::load(*database_, all);
}

QString SystemConfig::wifiHardwareAddress()
{
    return DeviceConfig::wifiHardwareAddrFromDatabase(*database_);
}

QString  SystemConfig::saveWifiHardwareAddr(const QString &wifi_hardware_addr)
{
    return DeviceConfig::setWifiHardwareAddrToDatabase(*database_, wifi_hardware_addr);
}

bool SystemConfig::saveWifiProfiles(WifiProfiles & all)
{
    return WifiConfig::save(*database_, all);
}

bool SystemConfig::clearDialupProfiles()
{
    return DialupConfig::clear(*database_);
}

bool SystemConfig::loadDialupProfiles(DialupProfiles & all)
{
    return DialupConfig::load(*database_, all);
}

bool SystemConfig::saveDialupProfiles(DialupProfiles & all)
{
    return DialupConfig::save(*database_, all);
}

QString SystemConfig::defaultPincode()
{
    return DialupConfig::defaultPincode();
}

void SystemConfig::setDefaultPincode(const QString &pincode)
{
    return DialupConfig::setDefaultPincode(pincode);
}

/// This function returns 1 for next page. It returns -1 for previous page.
/// If the distance is too small, it returns 0.
/// Caller can the direction by using setDirection.
int SystemConfig::direction(const QPoint & old_position, const QPoint & new_position)
{
    return PageTurningConfig::direction(old_position, new_position);
}

void SystemConfig::setDirection(int conf)
{
    PageTurningConfig::setDirection(conf);
}

int SystemConfig::distance()
{
    return PageTurningConfig::distance();
}

int SystemConfig::whichArea(const QPoint & old_position, const QPoint & new_position)
{
    return PageTurningConfig::whichArea(old_position, new_position);
}

/// Return the default download folder. Ensure the folder exist.
QString SystemConfig::downloadFolder()
{
    QString path;

    // Please note that this path is directory, and should exists
    // in file system.
    QString predefined_path = QString::fromLocal8Bit(qgetenv("ABSOLUTE_DOWNLOAD_PATH"));

    qDebug() << "at sys conf, download path is: " << predefined_path;
    if (!predefined_path.isEmpty())
    {
        path = predefined_path;
        QDir dir(path);
        if (dir.exists())
        {
            return path;
        }
    }

    if (isSDMounted())
    {
        path = SDMMC_ROOT;
    }
    else if (isFlashMounted())
    {
        path = LIBRARY_ROOT;
    }
    else
    {
        path = QDir::home().path();
    }

    QDir dir(path);
    QString ret;
    if (!dir.exists())
    {
        return ret;
    }

    static const QString FOLDER_NAME = "downloads";
    if (!dir.exists(FOLDER_NAME))
    {
        if (!dir.mkdir(FOLDER_NAME))
        {
            return ret;
        }
    }

    if (dir.cd(FOLDER_NAME))
    {
        ret = dir.absolutePath();
    }
    return ret;
}

QString SystemConfig::serialNumber()
{
    return DeviceConfig::serialNumber(*database_);
}

QString SystemConfig::deviceId()
{
    return DeviceConfig::deviceId(*database_);
}

QString SystemConfig::version()
{
    return DeviceConfig::version();
}

QString SystemConfig::cpuInfo()
{

    QString cpu,tmp;
    QString key1("Hardware");
    QString key2("BogoMIPS");

    QFile file("/proc/cpuinfo");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return cpu;
    QTextStream in(&file);
    QString line = in.readLine();
    while (!line.isNull()) 
    {
        if(line.contains(key1))
        {
            cpu.append( line.right( line.length() - line.indexOf(':') - 1 ).trimmed() );

        }

        if(line.contains(key2))
        {
            int i;
            tmp.append( line.right( line.length() - line.indexOf(':') - 1 ).trimmed() ); 
            if ( (i=tmp.indexOf(QChar('.'))) != -1)
            {
                tmp= tmp.left(i);
            }
        }

        line = in.readLine();
    }

    file.close();
    cpu.append(" ").append(tmp).append(" MHz");

    return cpu;
}

QString SystemConfig::memInfo()
{
    QString mem("%1/%2 MB");
    mem=mem.arg(systemFreeMemory()/1024/1024).arg(systemTotalMemory()/1024/1024);

    return mem;
}

QString SystemConfig::flashInfo()
{
    QString mount_point("/media/flash");
    QString flash("%1/%2 MB");
    flash=flash.arg(freeSpace(mount_point)/1024/1024).arg(diskSpace(mount_point)/1024/1024);
    return flash; 
}

bool SystemConfig::showBrowsingHistory()
{
    return qgetenv("SHOW_BROWSING_HISTORY").toInt()?true:false;
}

QString SystemConfig::defaultAccessPoint()
{
    return qgetenv("DEFAULT_ACCESS_POINT");
}

bool SystemConfig::setMiscValue(const QString &key, const QString &value)
{
    return MiscConfig::setValue(*database_, key, value);
}

QString SystemConfig::miscValue(const QString &key)
{
    return MiscConfig::getValue(*database_, key);
}

int SystemConfig::screenUpdateGCInterval()
{
    const int DEFAULT_GC_INTERVAL = 5;
    QString value = MiscConfig::getValue(*database_, "screen_update_setting");
    bool ok;
    int interval = value.toInt(&ok, 10);
    if (!ok)
    {
        interval = DEFAULT_GC_INTERVAL;
    }
    return interval;
}

int SystemConfig::screenUpdateGrayScaleSetting()
{
    const int DEFAULT_GRAY_SCALE_SETTING = 8;
    QString value = MiscConfig::getValue(*database_, "gray_scale_setting");
    bool ok;
    int gray_scale_setting = value.toInt(&ok, 10);
    if (!ok)
    {
        gray_scale_setting = DEFAULT_GRAY_SCALE_SETTING;
    }
    if (gray_scale_setting != DEFAULT_GRAY_SCALE_SETTING
            && gray_scale_setting != 16)
    {
        gray_scale_setting = DEFAULT_GRAY_SCALE_SETTING;
    }
    return gray_scale_setting;
}

}


#include "onyx/base/device.h"
#include <QtGui/QtGui>
#ifdef BUILD_FOR_ARM
#include <QtGui/qwsdisplay_qws.h>
#include <QtGui/qscreen_qws.h>
#endif
#include <QtNetwork/QtNetwork>
#include "onyx/sys/sys_status.h"
#include "onyx/sys/sys_conf.h"
#include "onyx/sys/platform.h"
#include "onyx/sys/service.h"


namespace sys
{


static const QString MOBILE_DEVICE = "mobile";
static const QString TETHERED_DEVICE = "tethered";
static const QString DEVICE_NAME = "OnyxBoox";
static const QString ROOT_FOLDER = "/";
static const QString DRM_CONTENT_DIR = "media/flash/DRM Contents";
static const QString ADOBE_RESOURCE_FOLDER = "adobe/resources/";
static const QString DEVICE_FILE     = "media/flash/.adobe-digital-editions/device.xml";
static const QString ACTIVATION_FILE = "media/flash/.adobe-digital-editions/activation.xml";

static void putDeviceType()
{
    QString is_mobile;
    is_mobile.setNum(1);
    qputenv("ADOBE_DE_MOBILE", is_mobile.toAscii());

    QString device_type = SysStatus::instance().hasTouchScreen() ? MOBILE_DEVICE : TETHERED_DEVICE;
    qputenv("ADEPT_DEVICE_TYPE", device_type.toAscii());

    qDebug("ADOBE_DE_MOBILE : %s", qgetenv("ADOBE_DE_MOBILE").constData());
    qDebug("ADEPT_DEVICE_TYPE : %s", qgetenv("ADEPT_DEVICE_TYPE").constData());
}

static void putRootFolder()
{
    qputenv("ADOBE_DE_ROOT_FOLDER", ROOT_FOLDER.toAscii());
    qDebug("ADOBE_DE_ROOT_FOLDER : %s", qgetenv("ADOBE_DE_ROOT_FOLDER").constData());
}

static QString getCustomizedDownloadPath()
{
    QString context = qgetenv("DOWNLOAD_PATH_CONTEXT");
    if (context.isEmpty())
    {
        return QString();
    }
    context = context.arg(QCoreApplication::tr("Downloaded"));
    qDebug() << "Download path for drm: " << context;
    QDir dir(context);
    if (!dir.exists())
    {
        if (!dir.mkpath(context))
        {
            qWarning() << "Could not create folder" << context;
            return QString();
        }
    }
    return context;
}

static void putDocumentFolder()
{
    // set the folder of Digital Edition
    QString customized_path = getCustomizedDownloadPath();
    if (!customized_path.isEmpty())
    {
        qputenv("ADOBE_DE_DOC_FOLDER", customized_path.toLocal8Bit());
    }
    else
    {
        QString path;
#ifdef Q_WS_QWS
        path = ROOT_FOLDER;
#else
        path = QDir::home().path();
#endif
        QDir dir(path);
        qputenv("ADOBE_DE_DOC_FOLDER", dir.absoluteFilePath(DRM_CONTENT_DIR).toAscii());
    }
    qDebug() << "path after set: " << QString::fromLocal8Bit(qgetenv("ADOBE_DE_DOC_FOLDER"));
}

static void putResourceFolder()
{
    // set the folder of Digital Edition
    QString path;
#ifdef Q_WS_QWS
    path = SHARE_ROOT;
#else
    path = QDir::home().path();
#endif
    QDir dir(path);
    qputenv("ADOBE_RESOURCE_FOLDER", dir.absoluteFilePath(ADOBE_RESOURCE_FOLDER).toAscii());
    qDebug("ADOBE_RESOURCE_FOLDER : %s", qgetenv("ADOBE_RESOURCE_FOLDER").constData());
}

static void putDeviceName()
{
    if (qgetenv("ADEPT_DEVICE_NAME").isEmpty())
    {
        qputenv("ADEPT_DEVICE_NAME", DEVICE_NAME.toAscii());
    }
    qDebug("ADEPT_DEVICE_NAME : %s", qgetenv("ADEPT_DEVICE_NAME").constData());
}

static void putDeviceFile()
{
    // set the folder of Digital Edition
    QString path;
#ifdef Q_WS_QWS
    path = ROOT_FOLDER;
#else
    path = QDir::home().path();
#endif
    QDir dir(path);
    if (!isImx508())
    {
        qputenv("ADEPT_DEVICE_FILE", dir.absoluteFilePath(DEVICE_FILE).toAscii());
        qDebug("ADEPT_DEVICE_FILE : %s", qgetenv("ADEPT_DEVICE_FILE").constData());
    }

    qputenv("ADEPT_ACTIVATION_FILE", dir.absoluteFilePath(ACTIVATION_FILE).toAscii());
    qDebug("ADEPT_ACTIVATION_FILE : %s", qgetenv("ADEPT_ACTIVATION_FILE").constData());
}

SysStatus::SysStatus()
#ifndef _WINDOWS
    : connection_(QDBusConnection::systemBus())
#else
    : connection_(QDBusConnection::sessionBus())
#endif
    , usb_mounted_(false)
    , sd_mounted_(false)
    , flash_mounted_(false)
    , system_busy_(true)
{
    installSlots();
}

SysStatus::SysStatus(SysStatus & ref)
#ifndef _WINDOWS
: connection_(QDBusConnection::systemBus())
#else
: connection_(QDBusConnection::sessionBus())
#endif
, usb_mounted_(false)
, sd_mounted_(false)
, system_busy_(true)
{
}

SysStatus::~SysStatus()
{
}

/// Register slots to system manager to enable application to receive signals from
/// system manager.
void SysStatus::installSlots()
{
    if (!connection_.connect(service, object, iface,
                             "mountTreeSignal",
                             this,
                             SLOT(onMountTreeChanged(bool, const QString &))))
    {
        qDebug("\nCan not connect the mountTreeSignal!\n");
    }

    if (!connection_.connect(service, object, iface,
                             "mountTreeSignal2",
                             this,
                             SLOT(onMountTreeChanged2(bool, const QString &, const QString &))))
    {
        qDebug("\nCan not connect the mountTreeSignal2!\n");
    }

    if (!connection_.connect(service, object, iface,
                             "sdCardChangedSignal",
                             this,
                             SLOT(onSdCardChanged(bool))))
    {
        qDebug("\nCan not connect the sdCardChangedSignal!\n");
    }

    if (!connection_.connect(service, object, iface,
                             "sdioChangedSignal",
                             this,
                             SLOT(onSdioChanged(bool))))
    {
        qDebug("\nCan not connect the sdioChangedSignal!\n");
    }

    if (!connection_.connect(service, object, iface,
                             "batterySignal",
                             this,
                             SLOT(onBatteryChanged(int,int))))
    {
        qDebug("\nCan not connect the batterySignal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "connectToPC",
                             this,
                             SLOT(onConnectToPC(bool))))
    {
        qDebug("\nCan not connect the connectToPC\n");
    }

    if (!connection_.connect(service, object, iface,
                             "inUSBSlaveMode",
                             this,
                             SLOT(onInUSBSlaveMode())))
    {
        qDebug("\nCan not connect the inUSBSlaveMode\n");
    }

    if (!connection_.connect(service, object, iface,
                             "screenRotated",
                             this,
                             SLOT(onScreenRotated(const int))))
    {
        qDebug("\nCan not connect the screenRotated\n");
    }

    if (!connection_.connect(service, object, iface,
                             "aboutToSuspend",
                             this,
                             SLOT(onAboutToSuspend())))
    {
        qDebug("\nCan not connect the aboutToSuspend\n");
    }

    if (!connection_.connect(service, object, iface,
                             "wakeup",
                             this,
                             SLOT(onWakeup())))
    {
        qDebug("\nCan not connect the wakeup\n");
    }

    if (!connection_.connect(service, object, iface,
                             "aboutToShutdown",
                             this,
                             SLOT(onAboutToShutdown())))
    {
        qDebug("\nCan not connect the aboutToShutdown\n");
    }

    if (!connection_.connect(service, object, iface,
                             "forceQuit",
                             this,
                             SLOT(onForceQuit())))
    {
        qDebug("\nCan not connect the forceQuit\n");
    }

    if (!connection_.connect(service, object, iface,
                             "onlineServiceSignal",
                             this,
                             SLOT(onlineService())))
    {
        qDebug("\nCan not connect the onlineService\n");
    }

    if (!connection_.connect(service, object, iface,
                             "pppConnectionChanged",
                             this,
                             SLOT(onPppConnectionChanged(const QString&, int))))
    {
        qDebug("\nCan not connect the pppConnectionChanged\n");
    }

    if (!connection_.connect(service, object, iface,
                             "volumeChanged",
                             this,
                             SLOT(onVolumeChanged(int, bool))))
    {
        qDebug("\nCan not connect the volume changed\n");
    }

    if (!connection_.connect(service, object, iface,
                             "volumeUpPressed",
                             this,
                             SLOT(onVolumeUpPressed())))
    {
        qDebug("\nCan not connect the volumeUpPressed\n");
    }

    if (!connection_.connect(service, object, iface,
                             "volumeDownPressed",
                             this,
                             SLOT(onVolumeDownPressed())))
    {
        qDebug("\nCan not connect the volumeDownPressed\n");
    }

    if (!connection_.connect(service, object, iface,
                             "stylusChanged",
                             this,
                             SLOT(onStylusChanged(bool))))
    {
        qDebug("\nCan not connect the signal stylus changed\n");
    }

    if (!connection_.connect(service, object, iface,
                             "musicPlayerStateChanged",
                             this,
                             SLOT(onMusicPlayerStateChanged(int))))
    {
        qDebug("\nCan not connect the musicPlayerStateChanged signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "downloadStateChanged",
                             this,
                             SLOT(onDownloadStateChanged(const QString &, int, bool))))
    {
        qDebug("\nCan not connect the downloadStateChanged signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "requestDRMUserInfo",
                             this,
                             SLOT(onRequestDRMUserInfo(const QString &, const QStringList &))))
    {
        qDebug("\nCan not connect the requestDRMUserInfo signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "fulfillmentStart",
                             this,
                             SLOT(onFulfillmentStart(const QString &))))
    {
        qDebug("\nCan not connect the fulfillmentStart signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "activationFinished",
                             this,
                             SLOT(onActivationFinished(bool))))
    {
        qDebug("\nCan not connect the activationFinished signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "fulfillmentFinished",
                             this,
                             SLOT(onFulfillmentFinished(const QString &, const QString &,int,int))))
    {
        qDebug("\nCan not connect the fulfillmentFinished signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "loanReturnFinished",
                             this,
                             SLOT(onLoanReturnFinished(const QString &))))
    {
        qDebug("\nCan not connect the loanReturnFinished signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "reportWorkflowError",
                             this,
                             SLOT(onReportWorkflowError(const QString &, const QString &))))
    {
        qDebug("\nCan not connect the reportWorkflowError signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "signalStrengthChanged",
                             this,
                             SLOT(onReport3GNetwork(const int, const int, const int))))
    {
        qDebug("\nCan not connect the signalStrengthChanged signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "volumeUpPressed",
                             this,
                             SLOT(onVolumeUpPressed())))
    {
        qDebug("\nCan not connect the volumeUpPressed signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "volumeDownPressed",
                             this,
                             SLOT(onVolumeDownPressed())))
    {
        qDebug("\nCan not connect the volumeDownPressed signal\n");
    }
   
    if (!connection_.connect(service, object, iface,
                             "hardwareTimerTimeout",
                             this,
                             SLOT(onHardwareTimerTimeout())))
    {
        qDebug("\nCan not connect the hardwareTimerTimeout signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "lowBatterySignal",
                             this,
                             SLOT(onLowBatterySignal())))
    {
        qDebug("\nCan not connect the lowBatterySignal signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "mouseLongPress",
                             this,
                             SLOT(onMouseLongPress(int, int, int, int))))
    {
        qDebug("\nCan not connect the mouseLongPress signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "multiTouchPressDetected",
                             this,
                             SLOT(onMultiTouchPressDetected(int, int, int, int, int, int, int, int))))
    {
        qDebug("\nCan not connect the multiTouchPressDetected signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "multiTouchReleaseDetected",
                             this,
                             SLOT(onMultiTouchReleaseDetected(int, int, int, int, int, int, int, int))))
    {
        qDebug("\nCan not connect the multiTouchReleaseDetected signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "multiTouchHoldDetected",
                             this,
                             SLOT(onMultiTouchHoldDetected(int, int, int, int, int, int, int, int, int, int))))
    {
        qDebug("\nCan not connect the multiTouchHoldDetected signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "ledSignal",
                             this,
                             SLOT(onLedSignal(const QByteArray &, const QByteArray &))))
    {
        qDebug("\nCan not connect the ledSignal signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "configKeyboard",
                             this,
                             SLOT(onConfigKeyboard())))
    {
        qDebug("\nCan not connect the configKeyboard signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "userBehaviorSignal",
                             this,
                             SLOT(onUserBehaviorSignal(
                                     const QByteArray &))))
    {
        qDebug("\nCan not connect the reportUserBehavior signal\n");
    }
}

bool SysStatus::batteryStatus(int& current,
                              int& status)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "batteryStatus"      // method.
    );

    // Call.
    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        QList<QVariant> args = reply.arguments();
        current = args[1].toInt();
        status = args[2].toInt();
        return true;
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

/// Ask system manager to broadcast battery signals to all listeners.
bool SysStatus::updateBatteryStatus()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "updateBatteryStatus"      // method.
    );

    // Call.
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return true;
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

/// Query whether USB is mounted or not.
/// TODO: improve it
bool SysStatus::isUSBMounted()
{
    static bool queried = false;
    if (!queried)
    {
        QFile file(USB_DEVICE);
        usb_mounted_ = file.open(QIODevice::ReadOnly);
        queried = true;
    }
    return usb_mounted_;
}

bool SysStatus::isSDMounted()
{
    return QFile::exists(SDMMC_DEVICE);
}

bool SysStatus::isFlashMounted()
{
#ifdef _WINDOWS
    return true;
#endif

    QFile file("/proc/mounts");
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    return file.readAll().contains(LIBRARY_ROOT);
}

bool SysStatus::isFlashMountedDynamic()
{
    return flash_mounted_;
}

bool SysStatus::isMusicPlayerRunning()
{
    return isProcessRunning("music_player");
}

bool SysStatus::isProcessRunning(const QString & proc_name)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "isProcessRunning"      // method.
    );
    message << proc_name;
    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::umountUSB()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "umountUSB"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::umountSD()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "umountSD"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

/// Ask the device to work in USB slave mode.
/// Usually this function is called when user connects device to PC
/// through a USB cable.
bool SysStatus::workInUSBSlaveMode()
{
    // Emit another signal to ensure caller close all resource.
    emit inUSBSlaveMode();

    // Now tell system manager to
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "workInUSBSlaveMode"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}


bool SysStatus::enableSdio(bool enable) const
{
#ifdef WIN32
    return true;
#endif
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "enableSdio"      // method.
    );

    message << enable;
    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::sdioState() const
{
#ifdef WIN32
    return true;
#endif

    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "sdioState"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::enableSdio(bool enable)
{
#ifdef WIN32
    return true;
#endif
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "enableSdio"      // method.
    );

    message << enable;
    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

void SysStatus::rotateScreen()
{
    // check default rotation at first.
    int default_rotation = sys::defaultRotation();
    int degree = screenTransformation();
    if (default_rotation == 0)
    {
        if (degree == 0)
        {
            setScreenTransformation(90);
        }
        else if (degree == 90)
        {
            setScreenTransformation(270);
        }
        else if (degree == 270)
        {
            setScreenTransformation(0);
        }
    }
    else if (default_rotation == 270)
    {
        if (degree == 0)
        {
            setScreenTransformation(180);
        }
        else if (degree == 180)
        {
            setScreenTransformation(270);
        }
        else if (degree == 270)
        {
            setScreenTransformation(0);
        }
    }
    else if (default_rotation == 90 || default_rotation == 180)
    {
        degree = (degree + 90) % 360;
        setScreenTransformation(degree);
    }
}

bool SysStatus::setScreenTransformation(int degree)
{
#ifdef BUILD_FOR_ARM
    QWSDisplay::setTransformation(degree / 90);

    // Also write to system conf file.
    const QString PATH = "/root/Settings/screen";
    QFile file(PATH);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        qWarning("Could not open %s for writing", qPrintable(PATH));
    }
    else
    {
        QString qws_args=getenv("QWS_DISPLAY");
        QString data;
        if (qws_args.isEmpty())
        {
            data = QString("export QWS_DISPLAY=Transformed:Rot%1:OnyxScreen:/dev/mem");
#ifdef BUILD_WITH_TFT
            data = QString("export QWS_DISPLAY=Transformed:Rot%1:LinuxFb:/dev/fb0:depth=all"); 
#endif
            data = data.arg(degree);
        }
        else
        {
            //replace Rotxx to Rot&degree
            data = qws_args.replace(QRegExp(":Rot\\d+"),QString(":Rot%1").arg(degree));
            data.prepend("export QWS_DISPLAY=");
        }
        file.write(data.toAscii());
        file.flush();
        file.close();
    }
#endif
    return true;
}

int  SysStatus::screenTransformation()
{
    int degree = 0;
#ifdef BUILD_FOR_ARM
    degree = QScreen::instance()->transformOrientation() * 90;
#endif
    return degree;
}

/// The calibration must be done in the server process.
#ifdef BUILD_FOR_ARM
bool SysStatus::clearCalibration()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "clearCalibration"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::calibrate(QWSPointerCalibrationData & data)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "calibrate"      // method.
    );

    for(int i = static_cast<int>(QWSPointerCalibrationData::TopLeft);
        i <= static_cast<int>(QWSPointerCalibrationData::Center);
        ++i)
    {
        message << data.screenPoints[i].x();
        message << data.screenPoints[i].y();
    }

    for(int i = static_cast<int>(QWSPointerCalibrationData::TopLeft);
        i <= static_cast<int>(QWSPointerCalibrationData::Center);
        ++i)
    {
        message << data.devPoints[i].x();
        message << data.devPoints[i].y();
    }

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}
#endif

bool SysStatus::setSuspendInterval(int ms)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "setSuspendInterval"      // method.
    );
    message << ms;

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

int  SysStatus::suspendInterval()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "suspendInterval"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        QList<QVariant> args = reply.arguments();
        return args[0].toInt();
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return 0;
}

bool SysStatus::suspend()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "suspend"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return true;
}

bool SysStatus::setShutdownInterval(int ms)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "setShutdownInterval"      // method.
    );
    message << ms;

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

int  SysStatus::shutdownInterval()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "shutdownInterval"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        QList<QVariant> args = reply.arguments();
        return args[0].toInt();
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return 0;
}

void SysStatus::shutdown(int reason)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "shutdown"      // method.
    );

    message << reason;
    connection_.asyncCall(message);
}

void SysStatus::resetIdle()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "resetIdle"      // method.
    );
    connection_.call(message);
}

void SysStatus::enableIdle(bool enable)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "enableIdle"      // method.
    );

    message << enable;
    connection_.call(message);
}

bool SysStatus::isIdleEnabled()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "isIdleEnabled"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

/// Show or hide music player. Usually, this function is used by
/// viewer or normal application to launch music player.
void SysStatus::requestMusicPlayer(int cmd)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "requestMusicPlayer"      // method.
    );

    message << cmd;
    connection_.asyncCall(message);
}

/// Broadcast music player state signal. This function is used
/// by music player to notify all listeners.
void SysStatus::reportMusicPlayerState(int state)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "reportMusicPlayerState"      // method.
    );

    message << state;
    connection_.asyncCall(message);
}

int SysStatus::volume()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "volume"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        QList<QVariant> args = reply.arguments();
        return args[0].toInt();
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return -1;
}

bool SysStatus::setVolume(int volume)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "setVolume"      // method.
    );
    message << volume;

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::mute(bool m)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "mute"      // method.
    );
    message << m;

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::isMute()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "isMute"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::isWpaSupplicantRunning()
{
#ifdef WIN32
    return true;
#endif

    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "isWpaSupplicantRunning"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::startWpaSupplicant(const QString & conf_file_path)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "startWpaSupplicant"      // method.
    );
    message << conf_file_path;

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::stopWpaSupplicant()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "stopWpaSupplicant"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());;
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::connect3g(const QString & chat_file,
                          const QString & username,
                          const QString & password)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "connect3g"      // method.
    );

    message << chat_file;
    message << username;
    message << password;

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());;
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

void SysStatus::disconnect3g()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "disconnect3g"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return;
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

bool SysStatus::isPowerSwitchOn()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "isPowerSwitchOn"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());;
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}


QString SysStatus::currentConnection()
{
    QString result;
    QList<QNetworkInterface> all = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface ni, all)
    {
        if (ni.flags().testFlag(QNetworkInterface::IsUp) && !ni.addressEntries().empty())
        {
            if (ni.name().contains("eth", Qt::CaseInsensitive) ||
                ni.name().contains("wlan", Qt::CaseInsensitive))
            {
                return "wifi";
            }
            else if (ni.name().contains("ppp", Qt::CaseInsensitive))
            {
                return "3g";
            }
        }
    }

    return result;
}

/// Check hardware connection type. It returns wifi or 3g.
/// But it does not mean the connection has been established.
QString SysStatus::connectionType()
{
    if (qgetenv("TG").toInt() > 0)
    {
        return "3g";
    }
    return "wifi";
}

/// Retrieve the wpa_proxy instance. Before using this function make sure
/// you call startWpaSupplicant already. Although dbus supports activation,
/// but we need to start wpa_supplicant with different conf file sometimes,
/// so it's better to ask system manager to start wpa_supplicant with different
/// parameters.
WpaConnection & SysStatus::wpa_proxy(const QString & name)
{
    if (!wpa_proxy_)
    {
        wpa_proxy_.reset(new WpaConnection(name));
    }
    return *wpa_proxy_;
}

WpaConnectionManager & SysStatus::connectionManager()
{
    if (!connection_manager_)
    {
        connection_manager_.reset(new WpaConnectionManager);
    }
    return *connection_manager_;
}

void SysStatus::setSystemBusy(bool busy,
                              bool show_indicator)
{
    system_busy_ = busy;
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "setSystemBusy"      // method.
    );
    message << busy;
    message << show_indicator;
    connection_.call(message);
}

void SysStatus::reportDownloadState(const QString &path,
                                    int percentage,
                                    bool open)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "reportDownloadState"      // method.
    );
    message << path;
    message << percentage;
    message << open;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

void SysStatus::triggerOnlineService()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "triggerOnlineService"      // method.
    );
    connection_.call(message);
}

bool SysStatus::setGrayScale(int colors)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "setGrayScale"      // method.
    );
    message << colors;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
        return false;
    }
    return true;
}

int SysStatus::grayScale()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "grayScale"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() > 0)
        {
            return reply.arguments().front().toInt();
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return 8;
}

bool SysStatus::startDRMService(const QStringList &strings)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "startDRMService"      // method.
    );
    message << strings;

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() > 0)
        {
            return reply.arguments().front().toBool();
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::stopDRMService()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "stopDRMService"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() > 0)
        {
            return reply.arguments().front().toBool();
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::startMessenger()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "startMessenger"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() > 0)
        {
            return reply.arguments().front().toBool();
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::stopMessenger()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "stopMessenger"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() > 0)
        {
            return reply.arguments().front().toBool();
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

void SysStatus::addDRMEnvironment()
{
    SystemConfig conf;
    qputenv("DEVICE_SERIAL", conf.serialNumber().toAscii());

    putDeviceType();
    putRootFolder();
    putDocumentFolder();
    putDeviceName();
    putDeviceFile();

    QByteArray res_folder = getenv( "ADOBE_RESOURCE_FOLDER" );
    if (res_folder.isEmpty())
    {
        putResourceFolder();
    }
}

void SysStatus::initDRMService()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "initDRMService"      // method.
    );
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}


// TODO, implement in system manager later.
bool SysStatus::setAsScreensaver(const QString & path)
{
    QDir dir(SHARE_ROOT);
    dir.cd("system_manager/images");

    int count =  dir.entryInfoList(QDir::NoDotAndDotDot|QDir::Files).size();
    QString result_path("screen_saver%1.png");
    result_path = result_path.arg(count);
    result_path = dir.absoluteFilePath(result_path);
    return QFile::copy(path, result_path);
}

void SysStatus::snapshot(const QString &path)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "snapshot"      // method.
    );
    message << path;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

bool SysStatus::hasTouchScreen()
{
#ifdef _WINDOWS
    return true;
#endif

    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "hasTouchScreen"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() > 0)
        {
            return reply.arguments().front().toBool();
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return true;
}

bool SysStatus::isTTSEnabled()
{
#ifdef _WINDOWS
    return true;
#endif
    return (qgetenv("ENABLE_TTS").toInt() > 0);
}

bool SysStatus::isDictionaryEnabled()
{
#ifdef _WINDOWS
    return true;
#endif
    return (qgetenv("ENABLE_DICT").toInt() > 0);
}

void SysStatus::dbgUpdateBattery(int left, int status)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "dbgUpdateBattery"      // method.
    );
    message << left;
    message << status;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

void SysStatus::startSingleShotHardwareTimer(const int seconds)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "startSingleShotHardwareTimer"      // method.
    );
    message << seconds;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

void SysStatus::setDefaultHardwareTimerInterval()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "setDefaultHardwareTimerInterval"      // method.
    );
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

void SysStatus::configKeyboard(unsigned int keys)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "configKeyboard"      // method.
    );
    message << keys;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

unsigned int SysStatus::keyboardConfiguration()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "keyboardConfiguration"      // method.
    );
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() > 0)
        {
            return reply.arguments().front().toUInt();
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return 0;
}


void SysStatus::setOfnThreshold(const int x,
                                const int y)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "setOfnThreshold"      // method.
    );
    message << x;
    message << y;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

bool SysStatus::ofnThreshold(int & x,
                             int & y)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "ofnThreshold"      // method.
    );
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() >= 2)
        {
            x = reply.arguments().at(0).toInt();
            y = reply.arguments().at(1).toInt();
            return true;
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::setBrightness(const unsigned char brightness)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "setBacklightBrightness"      // method.
    );

    message << brightness;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return true;
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

unsigned char SysStatus::brightness()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "backlightBrightness"      // method.
    );
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() > 0)
        {
            return reply.arguments().at(0).toInt();
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return 0;
}

void SysStatus::turnGlowLightOn(bool on, bool save)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "turnGlowLightOn"   // method.
    );

    message << on;
    message << save;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

bool SysStatus::glowLightOn()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "glowLightOn"       // method.
    );
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() > 0)
        {
            return reply.arguments().at(0).toInt() > 0;
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

/// Report user behavior. This function is used
/// by reader apps to notify user behavior to listeners.
void SysStatus::reportUserBehavior(const onyx::data::UserBehavior &behavior)
{
    QByteArray data;
    onyx::data::serialize(behavior, data);

    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "reportUserBehavior"      // method.
    );

    message << data;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

bool SysStatus::enableMultiTouch(bool enable)
{
    if (!sys::isIRTouch())
    {
        qWarning("enableMultiTouch can only be used on ir touch.\n");
        return false;
    }

    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "enableMultiTouch"      // method.
    );

    message << enable;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
        return false;
    }
    return true;
}

bool SysStatus::requestMultiTouch()
{
    if (!sys::isIRTouch())
    {
        qWarning("requestMultiTouch can only be used on ir touch.\n");
        return false;
    }

    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "requestMultiTouch"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
        return false;
    }
    return true;
}

bool SysStatus::queryLedSignal()
{
    if (!sys::isIRTouch())
    {
        qWarning("queryLedSignal can only be used on ir touch.\n");
        return false;
    }

    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "queryLedSignal"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
        return false;
    }
    return true;
}

void SysStatus::addTaskRecord(const QStringList & strings)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "addTaskRecord"      // method.
    );

    message << strings;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

void SysStatus::removeTaskRecord(const QString & name)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "removeTaskRecord"      // method.
    );

    message << name;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

QStringList SysStatus::allTasks()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "allTasks"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    QStringList result;
    foreach (QVariant v,  reply.arguments())
    {
        result.append(v.toString());
    }
    return result;
}

void SysStatus::dump()
{
    int left;
    int status;
    batteryStatus(left, status);
    qDebug("Bettery %d %d", left, status);
}

/// Handle mount tree signal.
void SysStatus::onMountTreeChanged(bool inserted, const QString &mount_point)
{
    if (mount_point == SDMMC_ROOT)
    {
        sd_mounted_ = inserted;
    }
    if (mount_point == USB_ROOT)
    {
        usb_mounted_ = inserted;
    }
    if (mount_point == LIBRARY_ROOT)
    {
        flash_mounted_ = inserted;
    }
    emit mountTreeSignal(inserted, mount_point);
}

void SysStatus::onMountTreeChanged2(bool inserted, const QString &mount_point, const QString &reason)
{
    if (mount_point == SDMMC_ROOT)
    {
        sd_mounted_ = inserted;
    }
    if (mount_point == USB_ROOT)
    {
        usb_mounted_ = inserted;
    }
    if (mount_point == LIBRARY_ROOT)
    {
        flash_mounted_ = inserted;
    }
    emit mountTreeSignal2(inserted, mount_point, reason);
}


/// The SD card hardware event handler. It does not mean the mount tree changed.
void SysStatus::onSdCardChanged(bool insert)
{
    emit sdCardChangedSignal(insert);
}

void SysStatus::onSdioChanged(bool on)
{
    emit sdioChangedSignal(on);
}

void SysStatus::onBatteryChanged(int current,
                                 int status)
{
    emit batterySignal(current, status);
}

void SysStatus::onConnectToPC(bool connected)
{
    emit connectToPC(connected);
}

void SysStatus::onInUSBSlaveMode()
{
    emit inUSBSlaveMode();
}

void SysStatus::onSystemIdle()
{
    emit systemIdleSignal();
}

void SysStatus::onScreenRotated(const int degree)
{
    emit screenRotated(degree);
}

void SysStatus::onAboutToSuspend()
{
    emit aboutToSuspend();
}

void SysStatus::onWakeup()
{
    emit wakeup();
}

void SysStatus::onAboutToShutdown()
{
    emit aboutToShutdown();
}

void SysStatus::onPppConnectionChanged(const QString &message, int state)
{
    emit pppConnectionChanged(message, state);
}

void SysStatus::onlineService()
{
    emit onlineServiceRequest();
}

void SysStatus::onForceQuit()
{
    emit forceQuit();
}

void SysStatus::onStylusChanged(bool inserted)
{
    emit stylusChanged(inserted);
}

void SysStatus::onMusicPlayerStateChanged(int state)
{
    emit musicPlayerStateChanged(state);
}

void SysStatus::onDownloadStateChanged(const QString &url,
                                       int state,
                                       bool open)
{
    emit downloadStateChanged(url, state, open);
}

void SysStatus::onVolumeChanged(int new_volume, bool is_mute)
{
    emit volumeChanged(new_volume, is_mute);
}

void SysStatus::onVolumeUpPressed()
{
    emit volumeUpPressed();
}

void SysStatus::onVolumeDownPressed()
{
    emit volumeDownPressed();
}

void SysStatus::onRequestDRMUserInfo(const QString &string, const QStringList & param)
{
    emit requestDRMUserInfo(string, param);
}

void SysStatus::onFulfillmentStart(const QString & url)
{
    emit fulfillmentStart(url);
}

void SysStatus::onFulfillmentFinished(const QString & url,
                                      const QString & string,
                                      int current,
                                      int total)
{
    emit fulfillmentFinished(url, string, current, total);
}

void SysStatus::onActivationFinished(bool succeeded)
{
    emit activationFinished(succeeded);
}

void SysStatus::onLoanReturnFinished(const QString & string)
{
    emit loanReturnFinished(string);
}

void SysStatus::onReportWorkflowError(const QString & workflow, const QString & error_code)
{
    emit reportWorkflowError(workflow, error_code);
}

void SysStatus::onReport3GNetwork(const int signal, const int total, const int network)
{
    emit report3GNetwork(signal, total, network);
}

void SysStatus::onHardwareTimerTimeout()
{
    emit hardwareTimerTimeout();
}

void SysStatus::onLowBatterySignal()
{
    emit lowBatterySignal();
}

void SysStatus::onMouseLongPress(int x, int y, int width, int height)
{
    emit mouseLongPress(QPoint(x, y), QSize(width, height));
}

void SysStatus::onMultiTouchPressDetected(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
{
    emit multiTouchPressDetected(QRect(x1, y1, width1, height1), QRect(x2, y2, width2, height2));
}

void SysStatus::onMultiTouchReleaseDetected(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
{
    emit multiTouchReleaseDetected(QRect(x1, y1, width1, height1), QRect(x2, y2, width2, height2));
}

void SysStatus::onMultiTouchHoldDetected(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2, int prev, int now)
{
    emit multiTouchHoldDetected(QRect(x1, y1, width1, height1), QRect(x2, y2, width2, height2), prev, now);
}

void SysStatus::onLedSignal(const QByteArray & x, const QByteArray & y)
{
    emit ledSignal(x, y);
}

void SysStatus::onConfigKeyboard()
{
    emit configKeyboard();
}

void SysStatus::onUserBehaviorSignal(const QByteArray &data)
{
    emit userBehaviorSignal(data);
}

}   // namespace sys

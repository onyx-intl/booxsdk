// -*- mode: c++; c-basic-offset: 4; -*-

#ifndef SYS_STATUS_H_
#define SYS_STATUS_H_

#include <string>
#ifdef BUILD_FOR_ARM
#include <QWSPointerCalibrationData>
#endif
#include "onyx/base/dbus.h"
#include "onyx/base/device.h"
#include "onyx/data/user_behavior.h"
#include "wpa_connection.h"
#include "wpa_connection_manager.h"

namespace sys
{


enum MusicPlayerState
{
    START_PLAYER = 0,   ///< To launch player or show the player.
    STOP_PLAYER,        ///< To stop player.
    SHOW_PLAYER,        ///< Show player. TODO, could be removed.
    HIDE_PLAYER,        ///< Play background.
    MUSIC_PLAYING,
    MUSIC_STOPPED,
    MUSIC_PAUSED,
    MUSIC_QUIT
};


/// SysStatus serves as a proxy between system manager and caller.
/// Caller uses SysStatus to get system status. Caller can also
/// change some system options through the methods provided by
/// SysStatus.
class SysStatus : public QObject
{
    Q_OBJECT;

  public:
    static SysStatus & instance()
    {
        static SysStatus instance_;
        return instance_;
    }
    ~SysStatus();

    void dbgUpdateBattery(int left, int status);
    void dump();

  public slots:
    bool batteryStatus(int& left, int& status);
    bool updateBatteryStatus();

    bool isUSBMounted();
    bool isSDMounted();
    /// return static mount state
    bool isFlashMounted();
    /// return false when connected to PC
    bool isFlashMountedDynamic();
    bool isMusicPlayerRunning();
    bool isProcessRunning(const QString & proc_name);

    bool umountUSB();
    bool umountSD();

    bool workInUSBSlaveMode();

    void rotateScreen();
    bool setScreenTransformation(int degree);
    int  screenTransformation();

#ifdef BUILD_FOR_ARM
    bool clearCalibration();
    bool calibrate(QWSPointerCalibrationData & data);
#endif

    void requestMusicPlayer(int cmd);
    void reportMusicPlayerState(int);

    bool setSuspendInterval(int ms);
    int  suspendInterval();
    bool suspend();

    bool setShutdownInterval(int ms);
    int  shutdownInterval();
    void shutdown(int reason = SHUTDOWN_REASON_USER_REQUEST);

    void resetIdle();
    void enableIdle(bool enable);
    bool isIdleEnabled();

    int volume();
    bool setVolume(int volume);
    bool mute(bool m);
    bool isMute();

    bool enableSdio(bool enable = true) const;
    bool sdioState() const ;
    bool enableSdio(bool enable = true);
    bool isWpaSupplicantRunning();
    bool startWpaSupplicant(const QString & conf_file_path);
    bool stopWpaSupplicant();

    WpaConnection & wpa_proxy(const QString & if_name = "wlan0");
    WpaConnectionManager & connectionManager();

    bool connect3g(const QString & chat_file, const QString & username, const QString & password);
    void disconnect3g();
    bool isPowerSwitchOn();

    QString currentConnection();
    QString connectionType();

    void setSystemBusy(bool busy = true, bool show_indicator = true);
    inline bool isSystemBusy() { return system_busy_; }

    bool setGrayScale(int colors);
    int grayScale();

    void reportDownloadState(const QString &path, int percentage, bool open_document);
    void triggerOnlineService();

    bool startDRMService(const QStringList &strings);
    bool stopDRMService();
    static void addDRMEnvironment();

    void initDRMService();
    bool startMessenger();
    bool stopMessenger();

    bool setAsScreensaver(const QString & path);
    void snapshot(const QString &path);

    bool hasTouchScreen();
    bool isTTSEnabled();
    bool isDictionaryEnabled();
    //bool isMusicPlayerRunning();

    void startSingleShotHardwareTimer(const int seconds);
    void setDefaultHardwareTimerInterval();

    void configKeyboard(unsigned int keys);
    unsigned int keyboardConfiguration();

    void setOfnThreshold(const int x, const int y);
    bool ofnThreshold(int & x, int & y);

    bool setBrightness(const unsigned char brightness);
    unsigned char brightness();
    void turnGlowLightOn(bool on, bool save);
    bool glowLightOn();

    void reportUserBehavior(const onyx::data::UserBehavior &behaviour);

    bool enableMultiTouch(bool enable = true);
    bool requestMultiTouch();
    bool queryLedSignal();

    void addTaskRecord(const QStringList & strings);
    void removeTaskRecord(const QString & path);
    void activateTask(const QString & path);
    QStringList allTasks();

    // The following signals must be the same with system manager.
    // Need a better way to sync them.
  signals:
    void mountTreeSignal(bool inserted, const QString &mount_point);
    void mountTreeSignal2(bool inserted, const QString &mount_point, const QString & reason);
    void sdCardChangedSignal(bool insert);
    void sdioChangedSignal(bool on);
    void batterySignal(int value, int status);
    void systemIdleSignal();
    void screenRotated(const int);

    /// connectToPC is emitted when the device is connected to PC.
    /// This signal is not emitted when user connects usb charger
    /// to device. This signal does not mean the device works in usb
    /// slave mode.
    void connectToPC(bool connected);

    /// inUSBSlaveMode signal is emitted when the device becomes USB
    /// disk. When received this signal, application should close
    /// all files opened on flash or sd.
    void inUSBSlaveMode();

    void volumeChanged(int new_volume, bool is_mute);
    void volumeUpPressed();
    void volumeDownPressed();

    void aboutToSuspend();
    void wakeup();
    void aboutToShutdown();

    void pppConnectionChanged(const QString &message, int);
    void onlineServiceRequest();
    void forceQuit();

    void stylusChanged(bool inserted);

    void musicPlayerStateChanged(int state);
    void downloadStateChanged(const QString &path, const int percentage, bool open);

    void requestDRMUserInfo(const QString & string, const QStringList & param);
    void fulfillmentStart(const QString & url);
    void fulfillmentFinished(const QString & url, const QString & string, int current, int total);
    void loanReturnFinished(const QString & string);
    void reportWorkflowError(const QString & workflow, const QString & error_code);
    void activationFinished(bool succeed);

    void report3GNetwork(const int signal, const int total, const int network);

    void hardwareTimerTimeout();
    void lowBatterySignal();

    void mouseLongPress(QPoint, QSize);
    void multiTouchPressDetected(QRect, QRect);
    void multiTouchReleaseDetected(QRect, QRect);
    void multiTouchHoldDetected(QRect, QRect, int, int);
    void ledSignal(const QByteArray & x, const QByteArray & y);

    void configKeyboard();

    void userBehaviorSignal(const QByteArray &data);

    void taskActivated(const QString & name);

  private slots:
    void onBatteryChanged(int, int);
    void onMountTreeChanged(bool mounted, const QString &mount_point);
    void onMountTreeChanged2(bool mounted, const QString &mount_point, const QString & reason);
    void onSdCardChanged(bool insert);
    void onSdioChanged(bool on);
    void onSystemIdle();
    void onScreenRotated(const int);

    void onVolumeChanged(int new_volume, bool is_mute);
    void onVolumeUpPressed();
    void onVolumeDownPressed();

    void onAboutToSuspend();
    void onWakeup();
    void onAboutToShutdown();

    void onConnectToPC(bool);
    void onInUSBSlaveMode();

    void onPppConnectionChanged(const QString &message, int);
    void onlineService();
    void onForceQuit();
    void onStylusChanged(bool inserted);

    void onMusicPlayerStateChanged(int);
    void onDownloadStateChanged(const QString &path, int percentage, bool);

    void onRequestDRMUserInfo(const QString &string, const QStringList & param);
    void onFulfillmentStart(const QString & url);
    void onFulfillmentFinished(const QString & url, const QString & string, int current, int total);
    void onLoanReturnFinished(const QString & string);
    void onReportWorkflowError(const QString & workflow, const QString & error_code);
    void onActivationFinished(bool succeeded);

    void onReport3GNetwork(const int signal, const int total, const int network);

    void onHardwareTimerTimeout();
    void onLowBatterySignal();

    void onMouseLongPress(int, int, int, int);    
    void onMultiTouchPressDetected(int, int, int, int, int, int, int, int);
    void onMultiTouchReleaseDetected(int, int, int, int, int, int, int, int);
    void onMultiTouchHoldDetected(int, int, int, int, int, int, int, int, int, int);
    void onLedSignal(const QByteArray & x, const QByteArray & y);

    void onTaskActivated(const QString & name);

    void onConfigKeyboard();

    void onUserBehaviorSignal(const QByteArray &data);

  private:
    SysStatus();
    SysStatus(SysStatus & ref);
    void installSlots();

    QDBusConnection connection_;    ///< Connection to system manager.
    bool usb_mounted_;
    bool sd_mounted_;
    bool flash_mounted_;
    bool system_busy_;
    scoped_ptr<WpaConnection> wpa_proxy_;
    scoped_ptr<WpaConnectionManager> connection_manager_;
};

};  // namespace sys

#endif  // SYS_STATUS_H_

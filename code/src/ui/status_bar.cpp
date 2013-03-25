#include "onyx/sys/sys.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/screen/screen_update_watcher.h"
#include "onyx/data/network_types.h"

#include "onyx/ui/status_bar.h"
#include "onyx/ui/status_bar_item_menu.h"
#include "onyx/ui/status_bar_item_power.h"
#include "onyx/ui/status_bar_item_slide.h"
#include "onyx/ui/status_bar_item_stylus.h"
#include "onyx/ui/status_bar_item_viewport.h"
#include "onyx/ui/status_bar_item_text.h"
#include "onyx/ui/status_bar_item_clock.h"
#include "onyx/ui/status_bar_item_refresh_screen.h"
#include "onyx/ui/status_bar_item_input_url.h"
#include "onyx/ui/status_bar_item_input.h"
#include "onyx/ui/status_bar_item_connection.h"
#include "onyx/ui/status_bar_item_3g_connection.h"
#include "onyx/ui/status_bar_item_volume.h"
#include "onyx/ui/status_bar_item_app.h"
#include "onyx/ui/status_bar_item_music_player.h"
#include "onyx/ui/number_dialog.h"
#include "onyx/ui/clock_dialog.h"
#include "onyx/ui/ui_utils.h"
#include "onyx/data/keys.h"
#include "onyx/ui/keyboard_config_dialog.h"
#include "onyx/sys/platform.h"
#include "task_list_dialog.h"

namespace ui
{

StatusBar::StatusBar(QWidget *parent, StatusBarItemTypes items)
    : QStatusBar(parent)
    , items_(0)
    , enable_jump_to_page_(true)
    , auto_connect_(false)
{
    createLayout();
    setupConnections();
    addItems(items);
    initUpdate();
}

StatusBar::~StatusBar(void)
{
}

void StatusBar::setupConnections()
{
    SysStatus & sys_status = SysStatus::instance();
    connect(&sys_status,
            SIGNAL(batterySignal(int, int)),
            this,
            SLOT(onBatterySignal(int, int)));
    connect(&sys_status,
            SIGNAL(aboutToSuspend()),
            this,
            SLOT(onAboutToSuspend()));
    connect(&sys_status,
            SIGNAL(lowBatterySignal()),
            this,
            SLOT(onLowBatterySignal()));
    connect(&sys_status,
            SIGNAL(wakeup()),
            this,
            SLOT(onWakeup()));
    connect(&sys_status,
            SIGNAL(aboutToShutdown()),
            this,
            SLOT(onAboutToShutdown()));
    connect(&sys_status,
            SIGNAL(sdioChangedSignal(bool)),
            this,
            SLOT(onWifiDeviceChanged(bool)));
    connect(&sys_status,
            SIGNAL(stylusChanged(bool)),
            this,
            SLOT(onStylusChanged(bool)));
    connect(&sys_status,
            SIGNAL(connectToPC(bool)),
            this,
            SLOT(onConnectToPC(bool)));
    connect(&sys_status,
            SIGNAL(volumeUpPressed()),
            this,
            SLOT(onVolumeButtonsPressed()));
    connect(&sys_status,
            SIGNAL(volumeDownPressed()),
            this,
            SLOT(onVolumeButtonsPressed()));

    connect(&sys_status,
            SIGNAL(report3GNetwork(const int, const int, const int)),
            this,
            SLOT(onReport3GNetwork(const int, const int, const int)));
    connect(&sys_status,
            SIGNAL(pppConnectionChanged(const QString &, int)),
            this,
            SLOT(onPppConnectionChanged(const QString &, int)));

    // connect configure keyboard signal
    connect(&sys_status,
            SIGNAL(configKeyboard()),
            this,
            SLOT(onConfigKeyboard()));
}

/// Update some status when it's created.
void StatusBar::initUpdate()
{
    SysStatus & sys_status = SysStatus::instance();

    int left = 100, status = BATTERY_STATUS_NORMAL;
    sys_status.batteryStatus(left, status);
    changeBatteryStatus(left, status, false);
}

void StatusBar::addItems(StatusBarItemTypes items)
{
    // Adjust the order if necessary.
    const StatusBarItemType all[] =
    {
        MENU, SCREEN_REFRESH, PROGRESS, MESSAGE, STYLUS, INPUT_TEXT, VOLUME, INPUT_URL,THREEG_CONNECTION,
        CONNECTION, VIEWPORT, MUSIC_PLAYER, APP_DEFINED, APP_CONFIG, CLOCK, BATTERY
    };
    const int size = sizeof(all)/sizeof(all[0]);
    for(int i = 0; i < size; ++i)
    {
        if (items.testFlag(all[i]))
        {
            item(all[i], true);
        }
    }
    items_ = items;
}

int StatusBar::itemState(const StatusBarItemType type)
{
    StatusBarItem *ptr = item(type, false);
    if (ptr)
    {
        return ptr->state();
    }
    return STATE_INVALID;
}

bool StatusBar::setItemState(const StatusBarItemType type,
                             const int state)
{
    return onItemStatusChanged(type, state);
}

bool StatusBar::setMessage(const QString & message)
{
    StatusBarItem *ptr = item(MESSAGE, false);
    if (ptr)
    {
        StatusBarItemMessage *text= static_cast<StatusBarItemMessage*>(ptr);
        text->setMessage(message);
        return true;
    }
    return false;
}

void StatusBar::showItem(StatusBarItemType id, bool show)
{
    StatusBarItem *ptr = item(id, false);
    if (ptr)
    {
        ptr->setVisible(show);
    }
}

bool StatusBar::setProgress(const int value,
                            const int total,
                            bool show_message,
                            const QString &message)
{
    if (total <= 0)
    {
        return false;
    }

    StatusBarItem *ptr = item(PROGRESS, false);
    if (ptr)
    {
        StatusBarItemProgress *wnd = static_cast<StatusBarItemProgress*>(ptr);
        wnd->setProgress(value, total, show_message, message);
    }

    if (show_message && (ptr = item(MESSAGE, false)))
    {
        StatusBarItemMessage *wnd = static_cast<StatusBarItemMessage*>(ptr);
        wnd->setMessage(value, total);
    }

    ptr = item(CLOCK, false);
    if (ptr)
    {
        ptr->update();
    }
    return true;
}

bool StatusBar::remove(const StatusBarItemType type)
{
    for(StatusBarIter iter = widgets_.begin(); iter != widgets_.end(); ++iter)
    {
        if ((*iter)->type() == type)
        {
            widgets_.erase(iter);
            return true;
        }
    }
    return false;
}

void StatusBar::clear()
{
}

void StatusBar::closeChildrenDialogs()
{
    closeUSBDialog();
    closeVolumeDialog();
    closeLowBatteryDialog();
}

void StatusBar::closeUSBDialog()
{
    USBConnectionDialog *dialog = usbConnectionDialog(false);
    if (dialog)
    {
        dialog->reject();
        usb_connection_dialog_.reset(0);
        onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GU);
    }
}

void StatusBar::closeLowBatteryDialog()
{
    LowBatteryDialog * dialog = lowBatteryDialog(false);
    if (dialog)
    {
        dialog->reject();
        low_battery_dialog_.reset(0);
        onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GU);
    }
}

void StatusBar::closeVolumeDialog()
{
    VolumeControlDialog *dialog = volumeDialog(false);
    if (dialog)
    {
        dialog->reject();
        volume_dialog_.reset(0);
    }
}

void StatusBar::mouseMoveEvent(QMouseEvent *me)
{
}

void StatusBar::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);

    painter.fillRect(rect(), QBrush(QColor(100, 100, 100)));

    QPen pen(QColor(0, 0, 0));
    pen.setWidth(1);

    painter.setPen(pen);
    painter.drawLine(QPoint(0, 0), QPoint(width(), 0));

    pen.setColor(QColor(255, 255, 255));
    painter.setPen(pen);
    painter.drawLine(QPoint(0, 1), QPoint(width(), 1));
}

void StatusBar::onProgressChanging(const int current,
                                   const int total)
{
    onyx::screen::instance().enableUpdate(false);
    StatusBarItem *ptr = item(MESSAGE, false);
    if (ptr)
    {
        StatusBarItemMessage *wnd = static_cast<StatusBarItemMessage*>(ptr);
        wnd->setMessage(current, total);
        wnd->repaint();
    }

    ptr = item(PROGRESS, false);
    if (ptr)
    {
        ptr->repaint();
    }
    QApplication::processEvents();
    onyx::screen::instance().enableUpdate(true);
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GC, false);
}

void StatusBar::onProgressChanged(const int percent,
                                  const int value)
{
    emit progressClicked(percent, value);
}

void StatusBar::onViewportChanged(const QRect & parent,
                                  const QRect & child,
                                  int column,
                                  int total)
{
    StatusBarItem *ptr = item(VIEWPORT, false);
    if (ptr)
    {
        StatusBarItemViewport *wnd = static_cast<StatusBarItemViewport*>(ptr);
        wnd->setViewport(parent, child, column, total);
        wnd->repaint();
    }
}

void StatusBar::onAppClicked(int id)
{
    emit appClicked(id);
}

void StatusBar::onMenuClicked()
{
    emit menuClicked();
}

void StatusBar::onStylusClicked()
{
    emit stylusClicked();
}

void StatusBar::onMessageAreaClicked()
{
    if (!isJumpToPageEnabled())
    {
        return;
    }

    int current = 1, total = 1;
    StatusBarItem * wnd = item(PROGRESS, false);
    if (wnd)
    {
        StatusBarItemProgress * p = static_cast<StatusBarItemProgress *>(wnd);
        p->progress(current, total);
    }

    if (total <= 1)
    {
        return;
    }

    // Popup page dialog.
    NumberDialog dialog(0);
    if (dialog.popup(current, total) != QDialog::Accepted)
    {
        onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GU);
        return;
    }

    current = dialog.value();
    emit progressClicked(current * 100/ total, current);
}

void StatusBar::onBatteryClicked()
{
    if (sys::isIMX31L())
    {
        LegacyPowerManagementDialog *legacy = legacyPMDialog(true);
        legacy->exec();
    }
    else
    {
        PowerManagementDialog *pm = pmDialog(true);
        pm->exec();
    }

    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GU);
}

void StatusBar::onMusicPlayerClicked()
{
    onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GU);
    sys::SysStatus::instance().requestMusicPlayer(sys::START_PLAYER);
}


void StatusBar::addAppItem(StatusBarItemType before, const int appId, const QImage & image)
{
    // check duplicated or not.
    for(size_t i = 0; i < widgets_.size(); ++i)
    {
        StatusBarItemPtr ptr = widgets_.at(i);
        if (ptr->type() == APP_DEFINED)
        {
            StatusBarItemApp * app = (StatusBarItemApp *)ptr.get();
            if (app->appId() == appId)
            {
                return;
            }
        }
    }

    size_t index = 0;
    for(index = 0; index < widgets_.size(); ++index)
    {
        StatusBarItemPtr ptr = widgets_.at(index);
        if (ptr->type() == before)
        {
            break;
        }
    }
    if (index >= widgets_.size())
    {
        index = 0;
    }

    StatusBarItemApp * item = new StatusBarItemApp(this);
    item->setAppId(appId);
    item->setImage(image);
    connect(item, SIGNAL(clicked(int)), this, SLOT(onAppClicked(int)));
    StatusBarItemPtr ptr(item);
    widgets_.insert(widgets_.begin() + index, ptr);
    insertPermanentWidget(index, item);
}

void StatusBar::removeAppItem(const int appId)
{
    StatusBarItem * item = 0;
    size_t index = 0;
    for(index = 0; index < widgets_.size(); ++index)
    {
        item = widgets_.at(index).get();
        if (item->type() == APP_DEFINED)
        {
            StatusBarItemApp * app = (StatusBarItemApp *)item;
            if (app->appId() == appId)
            {
                break;
            }
        }
    }
    if (index >= widgets_.size())
    {
        return;
    }

    widgets_.erase(widgets_.begin() + index);
    removeWidget(item);
}

void StatusBar::setAppIcon(const int appId, const QImage & image)
{
    StatusBarItemApp * app = (StatusBarItemApp*)item(ui::APP_DEFINED, false);
    app->setImage(image);
}

void StatusBar::setAppItemState(const int appId, const int state)
{
    for(size_t i = 0; i < widgets_.size(); ++i)
    {
        StatusBarItem * item = widgets_.at(i).get();
        if (item->type() == ui::APP_DEFINED)
        {
            StatusBarItemApp * app = (StatusBarItemApp*)item;
            if (app->appId() == appId)
            {
                app->setState(state);
                update();
                return;
            }
        }
    }
}

void StatusBar::showTaskManagementDialog()
{
    TaskListDialog dialog(this, sys::SysStatus::instance());
    dialog.exec();
}

void StatusBar::onClockClicked()
{
    StatusBarItem *ptr = item(CLOCK, false);
    StatusBarItemClock *clock = 0;
    if (ptr)
    {
        clock = static_cast<StatusBarItemClock*>(ptr);
        clockDialog(true, clock->startDateTime())->exec();
    }
}

void StatusBar::onVolumeButtonsPressed()
{
    QRegion region = visibleRegion();
    if (region.isEmpty())
    {
        return;
    }

    QRect visible_rect = region.boundingRect();
    if (visible_rect.width() < height() && visible_rect.height() < height())
    {
        return;
    }

    VolumeControlDialog * dialog = volumeDialog(true);
    if (!dialog->isVisible())
    {
        dialog->ensureVisible();
        onyx::screen::instance().updateWidget(0, onyx::screen::ScreenProxy::GU, false, onyx::screen::ScreenCommand::WAIT_COMMAND_FINISH);
    }
    else
    {
        dialog->resetTimer();
    }
}

void StatusBar::onHideVolumeDialog()
{
}

void StatusBar::onConfigKeyboard()
{
    if (!isActiveWindow())
    {
        // do not popup keyboard configure dialog when inactive.
        return;
    }

    SysStatus & sys_status = SysStatus::instance();
    unsigned int origin_config = sys_status.keyboardConfiguration();
    qDebug() << "origin value: " << origin_config;

    bool home_and_back_locked = (onyx::data::ENABLE_MENU_ESC & origin_config) == 0;
    bool page_turning_locked = (onyx::data::ENABLE_PAGE_UP_DOWN & origin_config) == 0;

    KeyboardConfigDialog config(home_and_back_locked,
            page_turning_locked, 0);
    int ret = config.popup();
    if (QDialog::Accepted == ret)
    {
        unsigned int new_value = 0;
        if (config.homeAndBackLocked())
        {
            new_value &= onyx::data::DISABLE_MENU_ESC;
        }
        else
        {
            new_value |= onyx::data::ENABLE_MENU_ESC;
        }

        if (config.pageTurningLocked())
        {
            new_value &= onyx::data::DISABLE_PAGE_UP_DOWN;
        }
        else
        {
            new_value |= onyx::data::ENABLE_PAGE_UP_DOWN;
        }
        qDebug() << "new value: " << new_value;
        sys_status.configKeyboard(new_value);
    }
    onyx::screen::instance().updateWidget(0, onyx::screen::ScreenProxy::GC,
            false, onyx::screen::ScreenCommand::WAIT_COMMAND_FINISH);

}

void StatusBar::onVolumeClicked()
{
    VolumeControlDialog * dialog = volumeDialog(true);
    dialog->ensureVisible();
}

void StatusBar::onScreenRefreshClicked()
{
    onyx::screen::instance().updateScreen(onyx::screen::ScreenProxy::GC);
}

void StatusBar::onInputUrlClicked()
{
    emit requestInputUrl();
}

void StatusBar::onInputTextClicked()
{
    emit requestInputText();
}

/// So far, we still leave this function, actually, it can be
/// merged with setItemState.
bool StatusBar::onItemStatusChanged(const StatusBarItemType type,
                                    const int state)
{
    switch (type)
    {
    case CONNECTION:
        changeConnectionStatus(state);
        return true;
    case STYLUS:
        changeStylus(state);
        return true;
    default:
        break;
    }
    return false;
}

void StatusBar::onBatterySignal(int value, int status)
{
    changeBatteryStatus(value, status, true);
}

void StatusBar::onLowBatterySignal()
{
    if (isActiveWindow() && ui::safeParentWidget(parentWidget())->isActiveWindow())
    {
        lowBatteryDialog(true)->exec();
    }
}

void StatusBar::onAboutToSuspend()
{
    qDebug("Status Bar handles about to suspend signal");
}

void StatusBar::onWakeup()
{
    qDebug("Status Bar handles wake up signal");
}

void StatusBar::onAboutToShutdown()
{
    qDebug("Status Bar handles about to shutdown signal");
    if (legacy_pm_dialog_.get())
    {
        legacyPMDialog(false)->reject();
        onyx::screen::watcher().enqueue(0, onyx::screen::ScreenProxy::GU,
                                        onyx::screen::ScreenCommand::WAIT_ALL);
    }
    if (pm_dialog_.get())
    {
        pmDialog(false)->reject();
        onyx::screen::watcher().enqueue(0, onyx::screen::ScreenProxy::GU,
                                        onyx::screen::ScreenCommand::WAIT_ALL);
    }
}

void StatusBar::onWifiDeviceChanged(bool enabled)
{
    qDebug("Status Bar handles wifi device changed signal");
    StatusBarItem *ptr = item(CONNECTION, false);
    if (ptr == 0)
    {
        return;
    }

    if (enabled)
    {
        ptr->show();
    }
    else
    {
        ptr->hide();
    }
}
void StatusBar::onReport3GNetwork(const int signal, const int total, const int network)
{

    StatusBarItem *ptr = item(THREEG_CONNECTION, false);
    if (ptr)
    {
        StatusBarItem3GConnection *wnd = static_cast<StatusBarItem3GConnection*>(ptr);
        onyx::screen::instance().enableUpdate(false);
        bool changed = wnd->signalStrengthChanged(signal, total,network);
        QApplication::processEvents();
        onyx::screen::instance().enableUpdate(true);
        if (changed && isVisible())
        {
            onyx::screen::instance().updateWidget(wnd, onyx::screen::ScreenProxy::GC, false);
        }
    }
}
void StatusBar::onPppConnectionChanged(const QString &message, int value)
{
    if(value == TG_DISCONNECTED)
    {
        onReport3GNetwork(0,5,5);
    }
    else if(value == TG_STOP)
    {
        onReport3GNetwork(-1,5,5);
    }

}


void StatusBar::onStylusChanged(bool inserted)
{
    qDebug("Status Bar handles stylus changed signal");
    StatusBarItem *ptr = item(STYLUS, false);
    if (ptr == 0)
    {
        return;
    }

    onyx::screen::instance().enableUpdate(false);
    if (inserted)
    {
        ptr->show();
    }
    else
    {
        ptr->hide();
    }
    QApplication::processEvents();
    onyx::screen::instance().enableUpdate(true);
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GC);
}

void StatusBar::onConnectToPC(bool connected)
{
    // Popup dialog.
    if (connected && isActiveWindow() && ui::safeParentWidget(parentWidget())->isActiveWindow())
    {
        if(auto_connect_)
        {
            autoSelect();
            return;
        }
        int ret = usbConnectionDialog(true)->exec();
        closeUSBDialog();
        if (ret != QMessageBox::Yes)
        {
            onyx::screen::instance().updateWidget(0, onyx::screen::ScreenProxy::GU);
            return;
        }
        onyx::screen::instance().flush();
        SysStatus::instance().workInUSBSlaveMode();
    }
    else if (!connected)
    {
        closeUSBDialog();
    }
}

void StatusBar::autoSelect()
{
    onyx::screen::instance().updateWidget(0, onyx::screen::ScreenProxy::GU);
    onyx::screen::instance().flush();
    SysStatus::instance().workInUSBSlaveMode();
}

/*
void StatusBar::onUSBCableChanged(bool connected)
{
    // Check if it's visible or not.
    if (!isVisible())
    {
        return;
    }

    // TODO, we need another dialog here to ask if user want to connect
    // to pc or not. After that, we show the semi transparent dialog.
    if (connected)
    {
        semiTransparentDialog(true)->popup();
    }
    else
    {
        transparent_dialog_.reset(0);
    }
}
*/

void StatusBar::changeConnectionStatus(const int conn_status)
{
    StatusBarItem *ptr = item(CONNECTION, false);
    if (ptr)
    {
        StatusBarItemConnection *conn_ptr = static_cast<StatusBarItemConnection*>(ptr);
        conn_ptr->setConnectionStatus(conn_status);
    }
}

void StatusBar::changeBatteryStatus(const int value,
                                    const int status,
                                    bool update_screen)
{
    StatusBarItem *ptr = item(BATTERY, false);
    if (ptr)
    {
        StatusBarItemBattery *wnd = static_cast<StatusBarItemBattery*>(ptr);
        onyx::screen::instance().enableUpdate(false);
        bool changed = wnd->setBatteryStatus(value, status);
        QApplication::processEvents();
        onyx::screen::instance().enableUpdate(true);
        if (update_screen && changed && isActiveWindow())
        {
            onyx::screen::instance().updateWidget(wnd, onyx::screen::ScreenProxy::GU, false);
        }
    }
}

void StatusBar::changeStylus(const int stylus)
{
    StatusBarItem *ptr = item(STYLUS, false);
    if (ptr)
    {
        StatusBarItemStylus *wnd = static_cast<StatusBarItemStylus*>(ptr);
        wnd->setStylusState(stylus);
    }
}

USBConnectionDialog * StatusBar::usbConnectionDialog(bool create)
{
    if (!usb_connection_dialog_ && create)
    {
        usb_connection_dialog_.reset(new USBConnectionDialog(0));
    }
    return usb_connection_dialog_.get();
}

LowBatteryDialog  * StatusBar::lowBatteryDialog(bool create)
{
    if (!low_battery_dialog_ && create)
    {
        low_battery_dialog_.reset(new LowBatteryDialog(0));
    }
    return low_battery_dialog_.get();
}

ClockDialog * StatusBar::clockDialog(bool create, const QDateTime & start)
{
    if (!clock_dialog_ && create)
    {
        clock_dialog_.reset(new ClockDialog(start, 0));
    }
    return clock_dialog_.get();
}

VolumeControlDialog *StatusBar::volumeDialog(bool create)
{
    if (!volume_dialog_ && create)
    {
        volume_dialog_.reset(new VolumeControlDialog(0));
    }
    return volume_dialog_.get();
}

LegacyPowerManagementDialog *StatusBar::legacyPMDialog(bool create)
{
    if (!legacy_pm_dialog_ && create)
    {
        legacy_pm_dialog_.reset(new LegacyPowerManagementDialog(0, sys::SysStatus::instance()));
    }
    return legacy_pm_dialog_.get();
}

PowerManagementDialog *StatusBar::pmDialog(bool create)
{
//    if (!pm_dialog_ && create)
    {
        pm_dialog_.reset(new PowerManagementDialog(0, sys::SysStatus::instance()));
    }
    return pm_dialog_.get();
}

void StatusBar::createLayout()
{
    setFixedHeight(ui::statusBarHeight());
    layout()->setSpacing(4);
    layout()->setContentsMargins(1, 2, 1, 0);
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
}

StatusBarItem *StatusBar::item(const StatusBarItemType type, bool create)
{
    for(StatusBarIter iter = widgets_.begin(); iter != widgets_.end(); ++iter)
    {
        if ((*iter)->type() == type)
        {
            return iter->get();
        }
    }

    if (!create)
    {
        return 0;
    }

    // Construct a new one.
    StatusBarItem *item = 0;
    switch (type)
    {
    case MENU:
        item = new StatusBarItemMenu(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onMenuClicked()));
        break;
    case BATTERY:
        item = new StatusBarItemBattery(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onBatteryClicked()));
        break;
    case MUSIC_PLAYER:
        item = new StatusBarItemMusicPlayer(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onMusicPlayerClicked()));
        break; 
    case CLOCK:
        item = new StatusBarItemClock(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onClockClicked()));
        break;
    case VOLUME:
        {
            // when music and tts both are disabled, do not show volume icon
            bool music_available = sys::SystemConfig::isMusicPlayerAvailable();
            bool disable_tts = qgetenv("DISABLE_TTS").toInt();
            if (!music_available && disable_tts)
            {
                item = 0;
            }
            else
            {
                item = new StatusBarItemVolume(this);
                connect(item, SIGNAL(clicked()), this, SLOT(onVolumeClicked()));
            }
            break;
        }
    case SCREEN_REFRESH:
        item = new StatusBarItemRefreshScreen(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onScreenRefreshClicked()));
        break;
    case INPUT_URL:
        item = new StatusBarItemInputUrl(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onInputUrlClicked()));
        break;
    case INPUT_TEXT:
        item = new StatusBarItemInput(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onInputTextClicked()));
        break;
    case CONNECTION:
        item = new StatusBarItemConnection(this);
        break;
    case THREEG_CONNECTION:
        item = new StatusBarItem3GConnection(this);
        break;
    case STYLUS:
        item = new StatusBarItemStylus(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onStylusClicked()));
        break;
    case PROGRESS:
        item = new StatusBarItemProgress(this);
        connect(item, SIGNAL(changing(const int, const int)),
                this, SLOT(onProgressChanging(const int, const int)));
        connect(item, SIGNAL(clicked(const int, const int)),
                this, SLOT(onProgressChanged(const int, const int)));
        break;
    case VIEWPORT:
        item = new StatusBarItemViewport(this);
        break;
    case APP_CONFIG:
        if (sys::SysStatus::instance().hasTouchScreen())
        {
            item = new StatusBarItemApp(this, APP_CONFIG);
            connect(item, SIGNAL(clicked(int)), this, SLOT(onAppClicked(int)));
        }
        break;
    case APP_DEFINED:
        item = new StatusBarItemApp(this, APP_DEFINED);
        connect(item, SIGNAL(clicked(int)), this, SLOT(onAppClicked(int)));
        break;
    default:
        break;
    }

    if (item == 0)
    {
        return 0;
    }

    StatusBarItemPtr ptr(item);
    widgets_.push_back(ptr);

    // Place this one into the layout.
    if (type == PROGRESS)
    {
        addWidget(ptr.get(), 1);
    }
    else if (type == SCREEN_REFRESH)
    {
        addWidget(ptr.get(), 0);
    }
    else if (type == MENU &&
             sys::isIRTouch())
    {
        OnyxLabel *label=new OnyxLabel(this);
        label->setFixedWidth(15);
        addWidget(label);
        addWidget(ptr.get());
    }
    else if (type == BATTERY &&
             sys::isIRTouch() && isHD())
    {
        OnyxLabel *label = new OnyxLabel(this);
        label->setFixedWidth(10);
        addPermanentWidget(ptr.get());
        addPermanentWidget(label);
    }
    else if (type != MENU && type != MESSAGE)
    {
        addPermanentWidget(ptr.get());
        int right_margin_width = qgetenv("STATUS_BAR_RIGHT_MARGIN").toInt();
        if (type == BATTERY && right_margin_width > 0)
        {
            right_margin_.reset(new OnyxLabel());
            right_margin_->setFixedWidth(right_margin_width);
            addPermanentWidget(right_margin_.get());
        }
    }
    else
    {
        addWidget(ptr.get());
    }
    return ptr.get();
}

}  // namespace ui

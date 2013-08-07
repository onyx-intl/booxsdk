
#include "onyx/wireless/wifi_dialog.h"
#include "onyx/screen/screen_update_watcher.h"
#include "onyx/data/data.h"
#include "onyx/data/data_tags.h"

namespace ui
{
const int SPACING = 2;
const int WIDGET_HEIGHT = 36;
static const int AP_ITEM_HEIGHT = 55;
static const int MARGINS = 10;
static const int DIALOG_SPACE = 30;

static const QString BUTTON_STYLE =    "\
QPushButton                             \
{                                       \
    background: transparent;            \
    font-size: 14px;                    \
    border-width: 1px;                  \
    border-color: transparent;          \
    border-style: solid;                \
    color: black;                       \
    padding: 0px;                       \
}                                       \
QPushButton:pressed                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    border-color: black;                \
    background-color: black;            \
}                                       \
QPushButton:checked                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    color: white;                       \
    border-color: black;                \
    background-color: black;            \
}                                       \
QPushButton:disabled                    \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    border-color: dark;                 \
    color: dark;                        \
    background-color: white;            \
}";

static const QString LEVEL_TAG = "level";

bool greaterBySignalLevel(ODataPtr a,  ODataPtr b)
{
    return (a->value(LEVEL_TAG).toInt() > b->value(LEVEL_TAG).toInt());
}

class WifiViewFactory : public ui::Factory
{
public:
    WifiViewFactory(){}
    ~WifiViewFactory(){}

public:
    virtual ContentView * createView(QWidget *parent, const QString &type = QString())
    {
        return new WifiAPItem(parent);
    }
};

static WifiViewFactory my_factory;

WifiDialog::WifiDialog(QWidget *parent,
                       SysStatus & sys)
    : QDialog(parent, Qt::FramelessWindowHint)
    , big_box_(this)
    , state_widget_layout_(0)
    , content_layout_(0)
    , ap_layout_(0)
    , buttons_layout_(0)
//    , hardware_address_("", 0)
    , close_button_("", 0)
    , state_widget_(this)
    , prev_button_(QPixmap(":/images/prev_page.png"), "", this)
    , next_button_(QPixmap(":/images/next_page.png"), "", this)
    , ap_view_(&my_factory, this)
    , sys_(sys)
    , proxy_(sys.connectionManager())
    , is_configuration_(false)
    , restart_for_auth_failed_(false)
{
    setGeometry(0, 0, ui::screenGeometry().width(), ui::screenGeometry().height());
    createLayout();
    setupConnections();
}

WifiDialog::~WifiDialog()
{
}

void WifiDialog::updateTr()
{
    update();
}

void WifiDialog::updateFonts()
{

}

void WifiDialog::runBackGround()
{
    scanResults(scan_results_);
    arrangeAPItems(scan_results_);
    triggerScan();
}

int WifiDialog::popup(bool scan, bool auto_connect)
{
    sys::SysStatus::instance().setSystemBusy(true);

    clicked_ssid_.clear();

    proxy_.enableAutoConnect(auto_connect);
    onScanReturned();

    sys::SysStatus::instance().setSystemBusy(false);

    onyx::screen::watcher().addWatcher(this);
    update();
    onyx::screen::watcher().enqueue(0, onyx::screen::ScreenProxy::GC);

    if (scan)
    {
        triggerScan();
    }

    state_widget_.dashBoard().setFocusTo(0, 1);

    update();
    onyx::screen::watcher().enqueue(0, onyx::screen::ScreenProxy::GU);

    bool ret = exec();
    update();
    onyx::screen::watcher().enqueue(0, onyx::screen::ScreenProxy::GC);
    onyx::screen::watcher().removeWatcher(this);
    return ret;
}

void WifiDialog::keyPressEvent(QKeyEvent *)
{
}

bool WifiDialog::event(QEvent *e)
{
    bool ret = QDialog::event(e);
    if(e->type() == QEvent::UpdateRequest)
    {
        onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::GU);
        e->accept();
        return true;
    }
    return ret;
}

void WifiDialog::keyReleaseEvent(QKeyEvent *ke)
{
    if (ke->key() == Qt::Key_Escape)
    {
        ke->accept();
        reject();
    }
    if(this->hasFocus())
    {
        state_widget_.dashBoard().setFocusTo(0, 1);
    }
}

void WifiDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    QRectF rc(0, state_widget_layout_.contentsRect().height(), width(), height()+4);
    QPainterPath p_path;
    p_path.addRect(rc);
    QBrush b(Qt::white);
    painter.setBrush(b);
    painter.drawPath(p_path);

    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(SPACING);
    painter.setPen(pen);
    int line_1_start_x = 0;
    int line_1_start_y = state_widget_layout_.contentsRect().y() + state_widget_layout_.contentsRect().height();
    int line_1_end_x = ui::screenGeometry().width();
    int line_1_end_y = line_1_start_y;
    painter.drawLine(line_1_start_x, line_1_start_y, line_1_end_x, line_1_end_y);
}

void WifiDialog::resizeEvent(QResizeEvent *re)
{
    QDialog::resizeEvent(re);
}

void WifiDialog::mousePressEvent(QMouseEvent *)
{
}

void WifiDialog::mouseReleaseEvent(QMouseEvent *)
{
}

void WifiDialog::createLayout()
{
    state_widget_.setStyleSheet("background: gray;");

    // big_box_.setSizeConstraint(QLayout::SetMinimumSize);
    big_box_.setContentsMargins(SPACING, 0, SPACING, SPACING);
    // content layout.
    big_box_.addLayout(&content_layout_);

    // Status.
    state_widget_layout_.addWidget(&state_widget_);
    content_layout_.addLayout(&state_widget_layout_);
    QObject::connect(&state_widget_, SIGNAL(refreshClicked()), this, SLOT(onRefreshClicked()));
    QObject::connect(&state_widget_, SIGNAL(customizedClicked()), this, SLOT(onCustomizedClicked()));
    QObject::connect(&state_widget_, SIGNAL(backClicked()), this, SLOT(onBackClicked()));
    QObject::connect(&prev_button_, SIGNAL(clicked()), &ap_view_, SLOT(goPrev()), Qt::QueuedConnection);
    QObject::connect(&next_button_, SIGNAL(clicked()), &ap_view_, SLOT(goNext()), Qt::QueuedConnection);

    // ap layout.
    ap_layout_.setContentsMargins(MARGINS, MARGINS, MARGINS, MARGINS);
    ap_layout_.setSpacing(5);
    content_layout_.addLayout(&ap_layout_);
    ap_layout_.addWidget(&ap_view_);

    QObject::connect(&ap_view_, SIGNAL(positionChanged(const int, const int)), this, SLOT(onPositionChanged(const int, const int)));
    QObject::connect(&ap_view_, SIGNAL(itemActivated(CatalogView*, ContentView*, int)), this, SLOT(onItemActivated(CatalogView*, ContentView*, int)));

    ap_view_.setFixedHeight(550);
    ap_view_.setPreferItemSize(QSize(-1, AP_ITEM_HEIGHT));
    ap_view_.setNeighbor(&state_widget_.dashBoard(), CatalogView::UP);
    content_layout_.addSpacing(50);

    // Buttons.
    content_layout_.addLayout(&buttons_layout_);
    buttons_layout_.setContentsMargins(MARGINS, 0, MARGINS, 0);
    prev_button_.setFocusPolicy(Qt::NoFocus);
    next_button_.setFocusPolicy(Qt::NoFocus);
    buttons_layout_.addWidget(&prev_button_);
    buttons_layout_.addStretch(0);
    buttons_layout_.addWidget(&next_button_);
    showPaginationButtons(true, true);

    // Hardware address.
   /* hardware_address_.setFixedHeight(WIDGET_HEIGHT);
    hardware_address_.setContentsMargins(MARGINS, 0, MARGINS, 0);
    content_layout_.addWidget(&hardware_address_);*/

    content_layout_.addSpacing(DIALOG_SPACE);
    big_box_.addStretch();
}

void WifiDialog::scanResults(WifiProfiles &aps)
{
    proxy_.scanResults(aps);

#ifdef _WINDOWS
    aps.clear();
    for(int i = 0; i < 10; ++i)
    {
        WifiProfile a;
        a.setSsid(QString::number(i));
        QByteArray d("aa:bb:cc:dd:ee:ff");
        a.setBssid(d);
        aps.push_back(a);
    }
#endif

}

void WifiDialog::connectAllAPItems(CatalogView &view)
{
    QVector<ContentView *> item_list = view.visibleSubItems();
    int size = item_list.size();
    for (int i=0; i<size; i++)
    {
        WifiAPItem * ap_item = static_cast<WifiAPItem *>(item_list.at(i));
        QObject::connect(ap_item, SIGNAL(config(WifiProfile &)),
                this, SLOT(onAPConfig(WifiProfile &)));
    }
}

void WifiDialog::appendStoredAPs(WifiProfiles & list)
{
    sys::SystemConfig conf;
    WifiProfiles stored_aps = records(conf);
    for(int i = 0; i < stored_aps.size(); i++)
    {
        if (!stored_aps[i].ssid().isEmpty())
        {
            bool found = false;
            foreach(WifiProfile profile, list)
            {
                if (stored_aps[i].ssid() == profile.ssid())
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                stored_aps[i].setPresent(false);
                ODataPtr d(new OData(stored_aps[i]));
                datas_.push_back(d);
            }
        }
    }
}

void WifiDialog::arrangeAPItems(WifiProfiles & profiles)
{
    clearDatas(datas_);
    foreach(WifiProfile profile, profiles)
    {
        // should not show access point with empty ssid (hidden networks)
        if (!profile.ssid().isEmpty())
        {
            ODataPtr d(new OData(profile));

            QString connecting_ap = connectingAccessPoint();
            if (!connecting_ap.isEmpty()
                    && connecting_ap == profile.ssid())
            {
                d->insert(TAG_CHECKED, true);
            }

            datas_.push_back(d);
        }
    }

    sort(datas_);

    appendStoredAPs(profiles);

    ap_view_.setData(datas_, true);
    if(datas_.size() > 0)
    {
        ap_view_.setFocusTo(0, 0);
    }
    showPaginationButtons(ap_view_.hasPrev(), ap_view_.hasNext());

    connectAllAPItems(ap_view_);
}

void WifiDialog::setupConnections()
{
    QObject::connect(&proxy_,
            SIGNAL(connectionChanged(WifiProfile,WpaConnection::ConnectionState)),
            this,
            SLOT(onConnectionChanged(WifiProfile, WpaConnection::ConnectionState)));
    QObject::connect(&proxy_, SIGNAL(passwordRequired(WifiProfile )),
            this, SLOT(onNeedPassword(WifiProfile )));
    QObject::connect(&proxy_, SIGNAL(noMatchedAP()),
            this, SLOT(onNoMatchedAP()));

    QObject::connect(&sys_, SIGNAL(sdioChangedSignal(bool)),
            this, SLOT(onSdioChanged(bool)));

    QObject::connect(&proxy_, SIGNAL(controlStateChanged(WpaConnectionManager::ControlState)),
        this, SLOT(onControlStateChanged(WpaConnectionManager::ControlState)));

    QObject::connect(&proxy_, SIGNAL(restartWpa()), this, SLOT(onRestartWpa()));
}

void WifiDialog::onRestartWpa()
{
    sys::SysStatus::instance().setSystemBusy(true);
    proxy_.stop();
    sys::SysStatus::instance().setSystemBusy(false);

//    clearDatas(datas_);
//    ap_view_.setData(datas_);

    restart_for_auth_failed_ = true;
    sys::SysStatus::instance().setSystemBusy(true);
    QTimer::singleShot(5000, this, SLOT(scan()));
}

void WifiDialog::clear()
{
}

void WifiDialog::scan()
{
    if (restart_for_auth_failed_)
    {
        restart_for_auth_failed_ = false;
        sys::SysStatus::instance().setSystemBusy(false);
    }
    proxy_.start();
}

void WifiDialog::triggerScan()
{
    proxy_.start();
}

void WifiDialog::connect(const QString & ssid, const QString &password)
{
    proxy_.scanResults(scan_results_);
    foreach(WifiProfile profile, scan_results_)
    {
        if (profile.ssid() == ssid)
        {
            setPassword(profile, password);
            onAPItemClicked(profile);
            return;
        }
    }
}

// So far, disable the auto connecting to best ap.
bool WifiDialog::connectToBestAP()
{
    if (!auto_connect_to_best_ap_)
    {
        return false;
    }
    auto_connect_to_best_ap_ = false;

    sys::SystemConfig conf;
    WifiProfiles all = records(conf);
    if (all.size() <= 0)
    {
        return false;
    }
    sortByCount(all);
    if (all.front().count() <= 0)
    {
        return false;
    }
    onAPItemClicked(all.front());
    return true;
}

bool WifiDialog::connectToDefaultAP()
{
    if (!auto_connect_to_default_ap_)
    {
        return false;
    }
    auto_connect_to_default_ap_ = false;

    QString ap = sys::SystemConfig::defaultAccessPoint();
    if (!ap.isEmpty())
    {
        for(int i = 0; i < scan_results_.size(); ++i)
        {
            if (scan_results_[i].ssid().contains(ap, Qt::CaseInsensitive))
            {
                onAPItemClicked(scan_results_[i]);
                return true;
            }
        }
    }
    return false;
}

QString WifiDialog::connectingAccessPoint()
{
    return proxy_.connectingAP().ssid();
}

void WifiDialog::enableIsConfiguration()
{
    is_configuration_ = true;
}

void WifiDialog::onScanTimeout()
{
    // If we can not connect to wpa supplicant before, we need to
    // scan now.
    scan();
}

void WifiDialog::onConnectionTimeout()
{
    // Need to clean up password.
    // Timeout in wifi dialog.
    updateStateLabel(WpaConnectionManager::CONTROL_CONNECTING_FAILED);
}

void WifiDialog::onAPItemClicked(WifiProfile & profile)
{
    clicked_ssid_ = profile.ssid();

    proxy_.connectTo(profile);
    update();
}

/// Refresh handler. We try to scan if wpa_supplicant has been launched.
/// Otherwise, we need to make sure wpa_supplicant is launched and it
/// acquired the system bus.
void WifiDialog::onRefreshClicked()
{
    proxy_.start();
}

void WifiDialog::onCustomizedClicked()
{
    sys::WifiProfile profile;
    if (showConfigurationDialog(profile))
    {
        onAPItemClicked(profile);
    }
}

void WifiDialog::onBackClicked()
{
    is_configuration_ = false;
    QTimer::singleShot(0, this, SLOT(onComplete()));
    update();
    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::GU);
}

void WifiDialog::onCloseClicked()
{
    reject();
}

void WifiDialog::onSdioChanged(bool on)
{
    if (on)
    {
        onRefreshClicked();
        enableChildren(on);
        //updateHardwareAddress();
    }
    else
    {
        updateStateLabel(WpaConnectionManager::CONTROL_STOP);
        enableChildren(on);
    }
}

void WifiDialog::enableChildren(bool enable)
{
    state_widget_.setEnabled(enable);
    ap_view_.setEnabled(enable);
}

void WifiDialog::onScanReturned()
{
    proxy_.scanResults(scan_results_);
    arrangeAPItems(scan_results_);
}

void WifiDialog::onConnectionChanged(WifiProfile profile, WpaConnection::ConnectionState state)
{
}

void WifiDialog::onControlStateChanged(WpaConnectionManager::ControlState state)
{
    updateStateLabel(state);
    if (state == WpaConnectionManager::CONTROL_CONNECTED)
    {
    }
    else if (state == WpaConnectionManager::CONTROL_COMPLETE)
    {
    }
    else if (state == WpaConnectionManager::CONTROL_ACQUIRING_ADDRESS_FAILED)
    {
    }
    else if (state == WpaConnectionManager::CONTROL_CONNECTING_FAILED)
    {
    }
    else if (state == WpaConnectionManager::CONTROL_SCANNED)
    {
        onScanReturned();
    }
    else if (state == WpaConnectionManager::CONTROL_CONNECTING)
    {
    }
    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::GU);
}

void WifiDialog::updateStateLabel(WpaConnectionManager::ControlState state)
{
    qDebug("WifiDialog::updateStateLabel %d", state);

    switch (state)
    {
    case WpaConnectionManager::CONTROL_INIT:
        state_widget_.setState(tr("Starting..."));
        break;
    case WpaConnectionManager::CONTROL_STOP:
        state_widget_.setState("");
        break;
    case WpaConnectionManager::CONTROL_SCANNING:
        state_widget_.setState(tr("Scanning..."));
        break;
    case WpaConnectionManager::CONTROL_SCANNED:
        state_widget_.setState(tr(""));
        onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GU);
        break;
    case WpaConnectionManager::CONTROL_CONNECTING:
        {
            QString msg = tr("Connecting to ");
            QString ssid = connectingAccessPoint();
            if (ssid.isEmpty() && !clicked_ssid_.isEmpty())
            {
                ssid = clicked_ssid_;
            }
            msg.append(ssid + " ...");
            state_widget_.setState(msg);
        }
        break;
    case WpaConnectionManager::CONTROL_CONNECTED:
        {
            QString msg = tr("Associated.");
            state_widget_.setState(msg);
        }
        break;
    case WpaConnectionManager::CONTROL_ACQUIRING_ADDRESS:
        {
            QString msg = tr("Acquiring IP Address...");
            state_widget_.setState(msg);
        }
        break;
    case WpaConnectionManager::CONTROL_ACQUIRING_ADDRESS_FAILED:
        state_widget_.setState(tr("Could not acquire address."));
        break;
    case WpaConnectionManager::CONTROL_COMPLETE:
        {
            QString text(tr("Connected to "));
            QString ssid = connectingAccessPoint();
            if(!ssid.isEmpty())
            {
                text.append(ssid);
            }
            else if(!clicked_ssid_.isEmpty())
            {
                text.append(clicked_ssid_);
            }
            else
            {
                text = tr("Connected.");
            }
            state_widget_.setState(text);

            if(!is_configuration_)
            {
                int millis = 200;
                QTimer::singleShot(millis, this, SLOT(onComplete()));
            }

            onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GU);
        }
        break;
    case WpaConnectionManager::CONTROL_CONNECTING_FAILED:
        state_widget_.setState(tr("Authentication failed."));
        break;
    case WpaConnectionManager::CONTROL_DISCONNECTED:
//        state_widget_.setState(tr("Disconnected."));
        break;
    default:
        state_widget_.setState(tr(""));
        break;
    }

    update();
    onyx::screen::watcher().enqueue(0, onyx::screen::ScreenProxy::GU,
                                    onyx::screen::ScreenCommand::WAIT_ALL);
}

/// Authentication handler. When the AP needs psk, we try to check
/// if the password is stored in database or not. If possible, we
/// can use the password remembered.
void WifiDialog::onNeedPassword(WifiProfile profile)
{
    qDebug("Need password now, password incorrect or not available.");

    // reserve the signal level value
    int level = profile.level();

    // No password remembered or incorrect.
    bool ok = showConfigurationDialog(profile);
    profile.setLevel(level);

    if (ok)
    {
        // We can store the AP here as user already updated password.
        storeAp(profile);

        // Connect again.
        proxy_.resetConnectRetry();
        proxy_.connectTo(profile);
    }
    else
    {
        clicked_ssid_.clear();
    }
}

void WifiDialog::onNoMatchedAP()
{
    proxy_.scanResults(scan_results_);
    arrangeAPItems(scan_results_);
}

void WifiDialog::onComplete()
{
    accept();
}

void WifiDialog::onItemActivated(CatalogView *catalog, ContentView *item, int user_data)
{
    if (!item || !item->data())
    {
        return;
    }
    WifiProfile * d = static_cast<WifiProfile *>(item->data());
    static_cast<WifiAPItem *>(item)->activateItem();
    proxy_.resetConnectRetry();
    onAPItemClicked(*d);
}

void WifiDialog::onPositionChanged(const int, const int)
{
    showPaginationButtons(ap_view_.hasPrev(), ap_view_.hasNext());
}

void WifiDialog::onAPConfig(WifiProfile &profile)
{
    if (showConfigurationDialog(profile))
    {
        onAPItemClicked(profile);
    }
}

void WifiDialog::sort(ODatas &list)
{
    // DescendingOrder
    qSort(list.begin(), list.end(), greaterBySignalLevel);
}

ApConfigDialogS *WifiDialog::apConfigDialog(WifiProfile &profile)
{
    if (0 == ap_config_dialog_)
    {
        ap_config_dialog_.reset(new ApConfigDialogS(this, profile));
    }
    return ap_config_dialog_.get();
}

void WifiDialog::setPassword(WifiProfile & profile,
                             const QString & password)
{
    if (profile.isWpa() || profile.isWpa2())
    {
        profile.setPsk(password);
    }
    else if (profile.isWep())
    {
        profile.setWepKey1(password);
    }
}

/// Store access point that successfully connected.
void WifiDialog::storeAp(WifiProfile & profile)
{
    sys::SystemConfig conf;
    WifiProfiles all = records(conf);

    profile.increaseCount();
    for(int i = 0; i < all.size(); ++i)
    {
        if (all[i].bssid() == profile.bssid())
        {
            profile.resetRetry();
            all.replace(i, profile);
            conf.saveWifiProfiles(all);
            return;
        }
    }

    all.push_front(profile);
    conf.saveWifiProfiles(all);
}

WifiProfiles WifiDialog::records(sys::SystemConfig &conf)
{
    WifiProfiles records;
    conf.loadWifiProfiles(records);
    return records;
}
/*
void WifiDialog::updateHardwareAddress()
{
    QString text(tr("Hardware Address: %1"));
    text = text.arg(proxy_.hardwareAddress());
    hardware_address_.setText(text);
}
*/
void WifiDialog::checkAndRestorePassword(WifiProfile &profile)
{
    sys::SystemConfig conf;
    WifiProfiles stored_aps = records(conf);
    for(int i = 0; i < stored_aps.size(); ++i)
    {
        if (stored_aps[i].bssid() == profile.bssid()
                && !profile.ssid().isEmpty() && profile.psk().isEmpty()
                && !stored_aps[i].psk().isEmpty())
        {
            profile.setPsk(stored_aps[i].psk());
            break;
        }
    }
}

bool WifiDialog::showConfigurationDialog(WifiProfile &profile)
{
    static bool denied_reentrance = false;
    if (denied_reentrance)
    {
        qDebug() << "denied reentrance";
        return false;
    }
    denied_reentrance = true;

    // load the stored password
    checkAndRestorePassword(profile);


    if (!isActiveWindow())
    {
        qDebug() << "WifiDialog, not active window, ignore showing config dialog";
        denied_reentrance = false;
        return false;
    }

    ap_config_dialog_.reset(0);
    int ret = apConfigDialog(profile)->popup();
    denied_reentrance = false;

    // Update screen.
    update();
    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::GC);

    // Check return value.
    if (ret == QDialog::Accepted)
    {
        return true;
    }
    return false;
}

void WifiDialog::showPaginationButtons(bool show_prev,
                                          bool show_next)
{
    prev_button_.setVisible(show_prev);
    next_button_.setVisible(show_next);
}

}   // namespace ui

///
/// \example wifi/wifi_gui_test.cpp
/// This is an example of how to use the WifiDialog class.
///

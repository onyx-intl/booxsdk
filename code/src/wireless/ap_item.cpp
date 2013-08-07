#include "onyx/wireless/ap_item.h"
#include "onyx/screen/screen_update_watcher.h"
#include "onyx/data/data_tags.h"

namespace ui
{

const QString TYPE_LABEL_STYLE = "      \
QLabel                                  \
{                                       \
     padding: 0px;                      \
     background: transparent;           \
     font: 24px ;                       \
     color: black;                      \
 }";

const QString LABEL_STYLE_WITH_SMALLER_TEXT = " \
QLabel                                  \
{                                       \
     padding: 0px;                      \
     background: transparent;           \
     font: 20px ;                       \
     color: black;                      \
 }";

/// Compiler issue.
#ifndef _WINDOWS
const int WifiAPItem::SIGNAL_ICONS;
#endif

static const int ID_CUSTOMIZE = 0;
static const int ID_REFRESH = 1;
static const int ID_BACK = 2;


static const int SPACING = 20;
static const int ICON_SIZE = 44 + SPACING;
static const QColor TITLE_BK_COLOR = QColor(210, 210, 210);
static const QColor NORMAL_BK_COLOR = QColor(255, 255, 255);
static const QColor SELECTED_BK_COLOR = QColor(164, 164, 164);

static const QString SIGNAL_PATH_BLACK = ":/images/wifi_%1_black.png";
static const QString SIGNAL_PATH_GRAY = ":/images/wifi_%1_gray.png";

WifiTitleItem::WifiTitleItem(QWidget *parent)
: QWidget(parent)
, layout_(this)
, icon_label_(0)
, title_label_(tr("Starting Wifi Device..."), 0)
, dash_board_(0, 0)
{
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Base);
    setFixedHeight(55);
    createLayout();
}

WifiTitleItem::~WifiTitleItem()
{
}

void WifiTitleItem::createLayout()
{
    layout_.setContentsMargins(SPACING, 0, SPACING, SPACING / 5);
    layout_.setSpacing(10);

    icon_label_.setPixmap(QPixmap(":/images/network_connection_icon.png"));
    layout_.addWidget(&icon_label_, 0, Qt::AlignVCenter);
    layout_.setSpacing(5);

    title_label_.setStyleSheet(LABEL_STYLE_WITH_SMALLER_TEXT);

    layout_.addWidget(&title_label_, 0, Qt::AlignVCenter);
    layout_.addStretch(0);

    // dash board
    dash_board_.setFixedWidth(75 * 2);
    layout_.addWidget(&dash_board_);
    createDashBoard();
    connect(&dash_board_,
            SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
            this,
            SLOT(onItemActivated(CatalogView *, ContentView *, int)));
}

void WifiTitleItem::createDashBoard()
{
    ODatas datas;
    ODataPtr customize (new OData);
    customize->insert(TAG_COVER, QPixmap(":/images/config_title.png"));
    customize->insert(TAG_ID, ID_CUSTOMIZE);
    datas.push_back(customize);

    ODataPtr refresh (new OData);
    refresh->insert(TAG_COVER, QPixmap(":/images/refresh_title.png"));
    refresh->insert(TAG_ID, ID_REFRESH);
    datas.push_back(refresh);

    ODataPtr back (new OData);
    back->insert(TAG_COVER, QPixmap(":/images/back_title.png"));
    back->insert(TAG_ID, ID_BACK);
    datas.push_back(back);


    dash_board_.setFixedGrid(1, datas.size());
    dash_board_.setData(datas);
    dash_board_.setSubItemBkColor(Qt::transparent);
}

void WifiTitleItem::setState(const QString & state)
{
    title_label_.setText(state);
}

void WifiTitleItem::onItemActivated(CatalogView *catalog, ContentView *item, int user_data)
{
    if (!item || !item->data())
    {
        return;
    }

    int id = item->data()->value(TAG_ID).toInt();
    if (id == ID_CUSTOMIZE)
    {
        emit customizedClicked();
    }
    else if (id == ID_REFRESH)
    {
        emit refreshClicked();
    }else if (id == ID_BACK)
    {
        emit backClicked();
    }
}

bool WifiTitleItem::event(QEvent *e)
{
    switch (e->type())
    {
    case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
        e->accept();
        return true;

    default:
        break;
    }
    return QWidget::event(e);
}

static QByteArray selected_bssid;
WifiAPItem *WifiAPItem::selected_item_ = 0;
static WifiAPItem *previous_selected_item_ = 0;

WifiAPItem::WifiAPItem(QWidget *parent)
    : ui::ContentView(parent)
    , hor_layout_(this)
    , status_icon_label_(0)
    , ssid_label_(0)
    , config_button_(QPixmap(":/images/config_gray.png"), "", 0)
    , lock_icon_label_(0)
    , signal_icon_label_(0)
    , signal_count_(0)
{
    createLayout();
    updateByProfile(profile_);
}

WifiAPItem::~WifiAPItem(void)
{
}

void WifiAPItem::updateView()
{
    // convert OData to profile
    if (data())
    {
        WifiProfile * d = static_cast<WifiProfile *>(data());
        profile_ = *d;
    }
    else
    {
        profile_.clear();
    }
    updateByProfile(profile_);
    selected_item_ = 0;
    previous_selected_item_ = 0;
    selected_bssid.clear();
}

void WifiAPItem::setProfile(WifiProfile & profile)
{
    if (!(profile_ == profile))
    {
        profile_ = profile;
        updateByProfile(profile_);
    }
    selected_item_ = 0;
    previous_selected_item_ = 0;
    selected_bssid.clear();
}

WifiProfile & WifiAPItem::profile()
{
    return profile_;
}

bool WifiAPItem::event(QEvent *e)
{
    switch (e->type())
    {
    case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
        e->accept();
        return true;

    default:
        break;
    }
    return QWidget::event(e);
}

void WifiAPItem::paintEvent(QPaintEvent *e)
{
    if (profile_.bssid().isEmpty() && profile_.isPresent())
    {
        return;
    }

    QPainter painter(this);
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(2, 2, -2, -2), 8, 8, Qt::AbsoluteSize);

    if (isSelected() || isAPChecked())
    {
        painter.fillPath(path, QBrush(SELECTED_BK_COLOR));
    }
    else
    {
        painter.fillPath(path, QBrush(NORMAL_BK_COLOR));
    }

    QPen pen;
    pen.setWidth(2);
    painter.setPen(pen);
    int start_x = 0;
    int start_y = height()-2;
    int end_x = width();
    int end_y = start_y;
    if (hasFocus())
    {
        painter.drawPath(path);
    }
    else
    {
        painter.drawLine(start_x, start_y, end_x, end_y);
    }
/*
    if (hasFocus())
    {
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.drawPath(path);
        status_icon_label_.setPixmap(QPixmap(":/images/status_black.png"));
        lock_icon_label_.setPixmap(QPixmap(":/images/lock_black.png"));
        config_button_.setIcon(QIcon(QPixmap(":/images/config_black.png")));
        signal_icon_label_.setPixmap(QPixmap(SIGNAL_PATH_BLACK.arg(signal_count_)));
    }
    else
    {
        pen.setColor(Qt::white);
        painter.setPen(pen);
        painter.drawPath(path);
        status_icon_label_.setPixmap(QPixmap(":/images/status_gray.png"));
        lock_icon_label_.setPixmap(QPixmap(":/images/lock_gray.png"));
        config_button_.setIcon(QIcon(QPixmap(":/images/config_gray.png")));
        signal_icon_label_.setPixmap(QPixmap(SIGNAL_PATH_GRAY.arg(signal_count_)));
    }
*/
}

void WifiAPItem::mousePressEvent(QMouseEvent *e)
{
    ContentView::mousePressEvent(e);
}

void WifiAPItem::activateItem()
{
    if (!profile_.bssid().isEmpty() && profile_.isPresent())
    {
        previous_selected_item_ = selected_item_;
        selected_item_ = this;
        selected_bssid = profile_.bssid();
        if (previous_selected_item_)
        {
            previous_selected_item_->repaint();
            onyx::screen::watcher().enqueue(previous_selected_item_, onyx::screen::ScreenProxy::GU);
        }
        repaint();
        onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::GU);
    }
}

void WifiAPItem::mouseReleaseEvent(QMouseEvent *e)
{
    activateItem();
    activate(0);
}

void WifiAPItem::focusInEvent(QFocusEvent * e)
{
    ContentView::focusInEvent(e);
}

void WifiAPItem::focusOutEvent(QFocusEvent * e)
{
    ContentView::focusOutEvent(e);
}

void WifiAPItem::createLayout()
{
    hor_layout_.setContentsMargins(SPACING / 2, SPACING / 2, SPACING, SPACING / 2);
    hor_layout_.setSpacing(0);
    status_icon_label_.setPixmap(QPixmap(":/images/status_gray.png"));
    ssid_label_.setContentsMargins(0, 0, 0, 0);
    ssid_label_.setStyleSheet(LABEL_STYLE_WITH_SMALLER_TEXT);
    hor_layout_.addWidget(&status_icon_label_, 0, Qt::AlignLeft);
    hor_layout_.addSpacing(10);
    hor_layout_.addWidget(&ssid_label_, 0, Qt::AlignVCenter);
    hor_layout_.addStretch(0);
    hor_layout_.addWidget(&lock_icon_label_, 0, Qt::AlignVCenter);
    hor_layout_.addSpacing(10);
    hor_layout_.addWidget(&signal_icon_label_, 0, Qt::AlignVCenter);

    hor_layout_.addSpacing(8);
    config_button_.setFocusPolicy(Qt::NoFocus);
    hor_layout_.addWidget(&config_button_, 0, Qt::AlignVCenter);
    connect(&config_button_, SIGNAL(clicked(bool)), this, SLOT(onConfigButtonClicked()));
}

/// return -1, 1~4 for the signal strength
int WifiAPItem::getSignalStrength(int level_in_profile)
{
    // signal level (dBM)
    int signal = -1;
    int signal_level = level_in_profile - 256;
    if (signal_level > -60)
    {
        signal = 4;
    }
    else if (signal_level <= -60 && signal_level > -86)
    {
        signal = 3;
    }
    else if (signal_level <= -86 && signal_level > -95)
    {
        signal = 2;
    }
    else if (signal_level <= -95 && signal_level > -100)
    {
        signal = 1;
    }
    else
    {
        signal = -1;
    }
    return signal;
}

void WifiAPItem::updateByProfile(WifiProfile & profile)
{
    if (!profile.isPresent())
    {
        status_icon_label_.setVisible(true);
        ssid_label_.setText(profile.ssid());
        ssid_label_.setVisible(true);
        lock_icon_label_.setVisible(false);
        config_button_.setVisible(true);

        signal_icon_label_.setVisible(false);
        update();
        return;
    }

    bool visible = !profile.bssid().isEmpty();
    status_icon_label_.setVisible(visible);
    ssid_label_.setVisible(visible);
    lock_icon_label_.setVisible(visible);
    config_button_.setVisible(visible);
    signal_icon_label_.setVisible(visible);

    if (profile.bssid().isEmpty())
    {
        update();
        return;
    }

    ssid_label_.setText(profile.ssid());
    if (profile.isWep() || profile.isWpa() || profile.isWpa2())
    {
        lock_icon_label_.setPixmap(QPixmap(":/images/lock_gray.png"));
    }
    else
    {
        lock_icon_label_.setVisible(false);
    }

    signal_count_ = getSignalStrength(profile.level());
    signal_icon_label_.setPixmap(SIGNAL_PATH_GRAY.arg(signal_count_));
    update();
}

bool WifiAPItem::isSelected()
{
    return (selected_bssid == profile_.bssid() && selected_item_ == this);
}

bool WifiAPItem::isAPChecked()
{
    if (data()->contains(TAG_CHECKED))
    {
        return data()->value(TAG_CHECKED).toBool();
    }
    return false;
}

void WifiAPItem::onConfigButtonClicked()
{
    emit config(profile_);
}

}

#include "onyx/ui/status_bar_item_wifi_connection.h"
#include "onyx/data/network_types.h"
#include "onyx/sys/sys.h"
#include "onyx/ui/ui_utils.h"

namespace ui
{

const int SPACING = 2;
const int MAX = 6;

StatusBarItemWifiConnection::StatusBarItemWifiConnection(QWidget *parent)
    : StatusBarItem(CONNECTION, parent)
    , strength_(-1)
    , total_(MAX)
    , text_width_(0)
{
    setConnectionInfomation(-1, MAX, NO_SERVICE);
    queryWifiStatus();

    //in case of ...
    QTimer::singleShot(2000, this, SLOT(queryWifiStatus()));
}

void StatusBarItemWifiConnection::queryWifiStatus()
{
    if (sys::SysStatus::instance().isWpaSupplicantRunning())
    {
        sys::SysStatus::instance().queryWifiStatus();
    }
}

StatusBarItemWifiConnection::~StatusBarItemWifiConnection(void)
{
}

bool StatusBarItemWifiConnection::setConnectionInfomation(const int strength,
                                                      const int total,
                                                      const int type)
{
    bool dirty = false;

    setFixedWidth(30 + SPACING);

    if (strength_ != strength || total_ != total)
    {
        strength_ = strength;
        total_ = total;
        dirty = true;
    }

    if (dirty) 
    {
        update();
    }
    return dirty;
}


void StatusBarItemWifiConnection::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QString image_path(":/images/wifi_%1.png");
    int level = strength_;
    if (strength_ < 0)
    {
        level = 0;
    }
    image_path = image_path.arg(level);
    painter.drawImage(QPoint(SPACING, 3), QImage(image_path));
}

void StatusBarItemWifiConnection::paintBars(QPainter & p,
                                        const QRect & rc,
                                        QBrush brush,
                                        int bars)
{
    int x = rc.left();
    int y = rc.top();
    int w = (rc.width() - (MAX - 1) * SPACING) / MAX;
    int h = rc.height() / MAX;
    for(int i = 0; i <= bars; ++i)
    {
        QRect r(x, y + (MAX -i) * h, w, h * i);
        p.fillRect(r, brush);
        x += w + SPACING;
    }
}

void StatusBarItemWifiConnection::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void StatusBarItemWifiConnection::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    emit clicked();
}

QString StatusBarItemWifiConnection::networkType(int type)
{
    static const QString types[] =
    {
        "WIFI",
        "WIFI",
        "WIFI", 
        "WIFI",
        "WIFI",
        "WIFI",
        "WIFI",
        "WIFI",
        "WIFI",
        "WIFI"
    };
    static const int size = sizeof(types)/sizeof(types[0]);
    if (type >=0 && type < size)
    {
        return types[type];
    }
    return QString();
}


}

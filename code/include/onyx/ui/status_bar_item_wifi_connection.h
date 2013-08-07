#ifndef ONYX_STATUS_BAR_ITEM_WIFI_CONNECTION_H_
#define ONYX_STATUS_BAR_ITEM_WIFI_CONNECTION_H_

#include "status_bar_item.h"

namespace ui
{

/// Represent wifi connection.
class StatusBarItemWifiConnection : public StatusBarItem
{
    Q_OBJECT

public:
    StatusBarItemWifiConnection(QWidget *parent);
    virtual ~StatusBarItemWifiConnection(void);

public Q_SLOTS:
    bool setConnectionInfomation(const int strength,
                                 const int total,
                                 const int type);

    void queryWifiStatus();

Q_SIGNALS:
    void clicked();

private:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

    void paintBars(QPainter & p, const QRect & rc, QBrush brush, int bars);

    QString networkType(int);

private:
    int strength_;

    int total_;
    int text_width_;
};

}      // namespace ui

#endif      // ONYX_STATUS_BAR_ITEM_WIFI_CONNECTION_H_

#ifndef ONYX_STATUS_BAR_ITEM_MENU_H_
#define ONYX_STATUS_BAR_ITEM_MENU_H_

#include "status_bar_item.h"

namespace ui
{

class StatusBarItemMenu : public StatusBarItem
{
    Q_OBJECT

public:
    StatusBarItemMenu(QWidget *parent);
    virtual ~StatusBarItemMenu(void);

Q_SIGNALS:
    void clicked();

public Q_SLOTS:
    void setImage(const QString &path);

private:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

    void paintMenu(QPainter &painter);
    void createLayout();

private:
    scoped_ptr<QImage> menu_icon_;
};

};  // namespace ui

#endif      // ONYX_STATUS_BAR_ITEM_MENU_H_

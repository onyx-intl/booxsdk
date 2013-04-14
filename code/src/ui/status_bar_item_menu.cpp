#include "onyx/ui/status_bar_item_menu.h"
#include "onyx/screen/screen_proxy.h"

namespace ui
{

StatusBarItemMenu::StatusBarItemMenu(QWidget *parent)
    : StatusBarItem(MENU, parent)
{
    createLayout();
}

StatusBarItemMenu::~StatusBarItemMenu(void)
{
}

void StatusBarItemMenu::createLayout()
{
    menu_icon_.reset(new QImage(getIconPath(":/images/menu.png")));
    setFixedWidth(menu_icon_->width() + 4);
}

void StatusBarItemMenu::setImage(const QString &path)
{
    menu_icon_.reset(new QImage(getIconPath(path)));
}

void StatusBarItemMenu::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);

    if (state() == STATE_SELECTED)
    {
        p.fillRect(rect(), Qt::black);
    }

    QImage *ptr = menu_icon_.get();
    QPoint point;
    point.setX((rect().width() - ptr->width()) >> 1);
    point.setY((rect().height() - ptr->height()) >> 1);
    p.drawImage(point, *(menu_icon_.get()));
}

void StatusBarItemMenu::mousePressEvent(QMouseEvent *me)
{
    me->accept();
    setState(STATE_SELECTED);
    onyx::screen::instance().enableUpdate(false);
    repaint();
    onyx::screen::instance().updateWidget(
        this,
        onyx::screen::ScreenProxy::DW,
        false,
        onyx::screen::ScreenCommand::WAIT_ALL);
    onyx::screen::instance().enableUpdate(true);
}

void StatusBarItemMenu::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    setState(STATE_NORMAL);
    onyx::screen::instance().enableUpdate(false);
    repaint();
    onyx::screen::instance().enableUpdate(true);
    emit clicked();
}

}

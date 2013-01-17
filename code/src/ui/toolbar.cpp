
#include <QtGui/QtGui>
#include "onyx/ui/toolbar.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/ui/keyboard_navigator.h"

static const int STATE_NORMAL = 0;
static const int STATE_PRESSED = 1;

namespace ui
{

OnyxToolBarItem::OnyxToolBarItem(QWidget *parent, QAction &action)
: QWidget(parent)
, action_(action)
, state_(STATE_NORMAL)
{
    connect(&action_, SIGNAL(changed()), this, SLOT(onActionChanged()));
    QSize size = iconSize();
    size += QSize(2, 2);
    setFixedSize(size);
    setAutoFillBackground(false);
}

OnyxToolBarItem::~OnyxToolBarItem()
{
}

QAction *OnyxToolBarItem::ownAction()
{
    return &action_;
}

void OnyxToolBarItem::onActionChanged()
{
    update();
}

QSize OnyxToolBarItem::iconSize()
{
    return action_.icon().actualSize(QSize(56, 56));
}

void OnyxToolBarItem::mousePressEvent(QMouseEvent *me)
{
    me->accept();
    if (!action_.isEnabled())
    {
        return;
    }

    state_ = STATE_PRESSED;
    onyx::screen::instance().enableUpdate(false);
    repaint();
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::DW, false);
    onyx::screen::instance().enableUpdate(true);
}

void OnyxToolBarItem::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    if (!action_.isEnabled())
    {
        return;
    }

    action_.trigger();
    state_ = STATE_NORMAL;
    onyx::screen::instance().enableUpdate(false);
    repaint();
    onyx::screen::instance().enableUpdate(true);
}

void OnyxToolBarItem::keyPressEvent(QKeyEvent *ke)
{
    ke->accept();
}

void OnyxToolBarItem::keyReleaseEvent(QKeyEvent *ke)
{
    ke->accept();
    if (!action_.isEnabled())
    {
        return;
    }

    int key = ke->key();
    if (key == Qt::Key_Return)
    {
        action_.trigger();
        state_ = STATE_NORMAL;
        onyx::screen::instance().enableUpdate(false);
        repaint();
        onyx::screen::instance().enableUpdate(true);
    }
    else if (key == Qt::Key_Up)
    {
        emit focusUp(&action_);
    }
    else if (key == Qt::Key_Down)
    {
        emit focusDown(&action_);
    }
    else if (key == Qt::Key_Left || key == Qt::Key_Right)
    {
        emit focusSibling(this, key);
    }
}

void OnyxToolBarItem::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);
    if (state_ == STATE_PRESSED)
    {
        p.fillRect(rect(), Qt::black);
    }

    QRect icon_rect(QPoint(1, 1), iconSize());
    if (action_.isEnabled())
    {
        action_.icon().paint(&p, icon_rect, Qt::AlignCenter, QIcon::Normal, QIcon::On);
    }
    else
    {
        action_.icon().paint(&p, icon_rect, Qt::AlignCenter, QIcon::Disabled, QIcon::On);
    }

    if (this->hasFocus())
    {
        QPen pen;
        int pen_width = 3;
        pen.setWidth(pen_width);
        p.setPen(pen);
        p.drawRoundedRect(rect().adjusted(pen_width, pen_width, -pen_width,
                                                -pen_width), 5, 5);
    }
}


OnyxToolBar::OnyxToolBar(QWidget *parent)
    : QWidget(parent)
    , layout_(this)
{
    setAutoFillBackground(false);
    setFixedHeight(60);
    layout_.setSpacing(8);
    layout_.setContentsMargins(2, 1, 2, 1);
    layout_.insertStretch(0, 0);
}

OnyxToolBar::~OnyxToolBar()
{
    clear();
}

void OnyxToolBar::addAction(QAction *action)
{
    OnyxToolBarItemPtr ptr(new OnyxToolBarItem(this, *action));
    connect(ptr.get(), SIGNAL(focusDown(QAction *)), this, SIGNAL(focusDown(QAction *)));
    connect(ptr.get(), SIGNAL(focusUp(QAction *)), this, SIGNAL(focusUp(QAction *)));
    connect(ptr.get(), SIGNAL(focusSibling(OnyxToolBarItem *, int)),
            this, SLOT(onFocusSibling(OnyxToolBarItem *, int)));
    items_.push_back(ptr);
    layout_.insertWidget(items_.size() - 1, ptr.get());
}

void OnyxToolBar::clear()
{
    for(size_t i = 0; i < items_.size(); ++i)
    {
        layout_.removeWidget(items_[i].get());
        items_[i].reset(0);
    }
    items_.clear();
}

std::vector<OnyxToolBarItemPtr> OnyxToolBar::subItems()
{
    return items_;
}

bool OnyxToolBar::event(QEvent *e)
{
    switch (e->type())
    {
    //case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::Enter:
    case QEvent::Leave:
        e->accept();
        return true;
    default:
        break;
    }
    return QWidget::event(e);
}

void OnyxToolBar::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);
    QRect rc(rect());

    painter.fillRect(rc, QBrush(QColor(255, 255, 255)));
    //painter.setPen(QColor(255, 255, 255));
    //painter.drawLine(rc.left(), rc.bottom() - 1, rc.right(), rc.bottom() - 1);

    painter.setPen(QColor(0, 0, 0));
    painter.drawLine(rc.left(), rc.bottom(), rc.right(), rc.bottom());
}

void OnyxToolBar::onFocusSibling(OnyxToolBarItem *current, int key)
{
    int size = items_.size();
    if (size <= 1)
    {
        return;
    }

    int i=0;
    for (; i<size; ++i)
    {
        if (items_[i].get() == current)
        {
            break;
        }
    }


    if (key == Qt::Key_Right)
    {
        if (i < size-1)
        {
            if (items_[i+1]->ownAction()->isEnabled())
            {
                items_[i+1]->setFocus();
            }
            else
            {
                onFocusSibling(items_[i+1].get(), key);
            }
        }
    }
    else if (key == Qt::Key_Left)
    {
        if (i > 0 && i < size)
        {
            if (items_[i-1]->ownAction()->isEnabled())
            {
                items_[i-1]->setFocus();
            }
            else
            {
                onFocusSibling(items_[i-1].get(), key);
            }
        }
    }
}

}

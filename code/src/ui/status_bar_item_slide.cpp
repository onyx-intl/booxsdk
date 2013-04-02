#include "onyx/ui/status_bar_item_slide.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/sys/platform.h"
#include "onyx/ui/ui_utils.h"

namespace ui
{

static const int MARGIN = 1;
static const int Y_POS  = 18;
static const int HEIGHT = 6;

StatusBarItemProgress::StatusBarItemProgress(QWidget *parent)
    : StatusBarItem(PROGRESS, parent)
    , current_(1)
    , total_(1)
    , pressing_value_(-1)
    , show_message_(true)
    , message_("")
{
    initFont();
    createLayout();

    timer_.setSingleShot(true);
    connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

StatusBarItemProgress::~StatusBarItemProgress(void)
{
}

void StatusBarItemProgress::setProgress(const int current, const int total,
        const bool show_message, const QString &message)
{
    show_message_ = show_message;
    message_ = message;
    if (current == current_ && total == total_)
    {
        return;
    }
    current_ = current;
    total_ = total;

    updatefgPath(current_);
    update();
}

void StatusBarItemProgress::progress(int & current,
                                     int & total)
{
    current = current_;
    total = total_;
}

void StatusBarItemProgress::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillPath(bk_path_, Qt::white);
    p.fillPath(fg_path_, Qt::black);

    if (show_message_)
    {
        drawMessage(p);
    }
}

void StatusBarItemProgress::drawMessage(QPainter &painter)
{
    QString message("%1/%2");
    message = message.arg(current_).arg(total_);
    if (!message_.isEmpty())
    {
        message = message_;
    }
    QFontMetrics metrics(font_);

    // Jim: add redundace space for displaying full message at x86
    static int REDUNDANCE = 40;
    int text_width = metrics.width(message) + REDUNDANCE;
    int x = rect().width() / 2 - text_width / 2;
    int y = 0;
    int text_height = rect().height() / 2 + 5;
    painter.setPen(Qt::white);
    painter.drawText(QRect(x, y, text_width, text_height),
            Qt::AlignCenter, message);
}

void StatusBarItemProgress::initFont()
{
    font_.setPointSize(17);
    if(ui::isHD() && sys::isIRTouch())
    {
        font_.setPointSize(22);
    }
    font_.setBold(true);
    setFont(font_);
}

void StatusBarItemProgress::resizeEvent(QResizeEvent * event)
{
    int height_to_set = HEIGHT;
    if(ui::isHD() && sys::isIRTouch())
    {
        height_to_set += 2;
    }

    QRect rc(0, Y_POS, width(), height_to_set);
    updatePath(bk_path_, rc);

    if (pressing_value_ > 0)
    {
        updatefgPath(pressing_value_);
    }
    else
    {
        updatefgPath(current_);
    }
}

void StatusBarItemProgress::mousePressEvent(QMouseEvent *me)
{
    // Check position.
    me->accept();
    int x = me->x() < 0 ? 0 : me->x();
    int value = x * total_ / (width() - 2) + 1;
    if (value > total_)
    {
        value = total_;
    }

    pressing_value_ = current_;
    if (value != pressing_value_)
    {
        pressing_value_ = value;
        updatefgPath(pressing_value_);
        onyx::screen::instance().enableUpdate(false);
        repaint();
        onyx::screen::instance().updateWidget(
            this,
            onyx::screen::ScreenProxy::DW,
            false,
            onyx::screen::ScreenCommand::WAIT_COMMAND_FINISH);
        onyx::screen::instance().enableUpdate(true);
        timer_.stop();
        timer_.start(300);
    }
}

void StatusBarItemProgress::mouseMoveEvent(QMouseEvent *me)
{
    me->accept();
    int x = me->x() < 0 ? 0 : me->x();
    int value = x * total_ / (width() - 2) + 1;
    if (value > total_)
    {
        value = total_;
    }

    if (value != pressing_value_)
    {
        pressing_value_ = value;
        updatefgPath(pressing_value_);
        onyx::screen::instance().enableUpdate(false);
        repaint();
        onyx::screen::instance().updateWidget(
            this,
            onyx::screen::ScreenProxy::DW,
            false,
            onyx::screen::ScreenCommand::WAIT_COMMAND_FINISH);
        onyx::screen::instance().enableUpdate(true);
        timer_.stop();
        timer_.start(300);
    }
}

void StatusBarItemProgress::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    timer_.stop();
    if (current_ != pressing_value_)
    {
        emit clicked(pressing_value_ * 100 / total_, pressing_value_);
    }
    pressing_value_ = -1;
}

void StatusBarItemProgress::onTimeout()
{
    static int count = 0;
    qDebug("timeout %d value %d total %d", ++count, pressing_value_, total_);
    emit changing(pressing_value_, total_);
    timer_.stop();
}


void StatusBarItemProgress::createLayout()
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
}

void StatusBarItemProgress::updatefgPath(int value)
{
    int w = width();

    int height_to_set = HEIGHT;
    if(ui::isHD() && sys::isIRTouch())
    {
        height_to_set += 2;
    }

    QRect rc(MARGIN, Y_POS + MARGIN, (w - MARGIN * 2) * value / total_, height_to_set - 2 * MARGIN);
    updatePath(fg_path_, rc);
}

void StatusBarItemProgress::updatePath(QPainterPath & result,
                                       const QRect & rect)
{
    int height = rect.height();
    int x_start = rect.left();
    int x_end = rect.right();
    int y = rect.top();

    if (sys::isIRTouch())
    {
        y += 2;
        if(ui::isHD())
        {
            y += 8;
        }
    }

    const int ARC_RADIUS = 2;
    int diameter = (ARC_RADIUS << 1);

    QPainterPath path;
    path.moveTo(x_end, y + ARC_RADIUS);
    path.arcTo((x_end - diameter), y, diameter, diameter, 0.0, 90.0);
    path.lineTo(x_start + ARC_RADIUS, y);
    path.arcTo(x_start, y, diameter, diameter, 90.0, 90.0);
    path.lineTo(x_start, y + (height - ARC_RADIUS));
    path.arcTo(x_start, y + (height - diameter), diameter, diameter, 180.0, 90.0);
    path.lineTo((x_end - ARC_RADIUS), y + height);
    path.arcTo((x_end - diameter), y + (height - diameter), diameter, diameter, 270.0, 90.0);
    path.closeSubpath();

    result = path;
}

}   // namespace ui

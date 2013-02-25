#include "onyx/ui/volume_control.h"
#include "onyx/screen/screen_update_watcher.h"
#include "onyx/sys/sys.h"
#include "math.h"

namespace ui
{

static const int TIMEOUT = 3000;
static QVector<int> volumes;

// VolumeControlDialog
VolumeControlDialog::VolumeControlDialog(QWidget *parent, int time_out)
    : QDialog(parent, static_cast<Qt::WindowFlags>(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint))
    , layout_(this)
    , image_(":/images/volume_strength.png")
    , current_(0)
    , min_(0)
    , max_(1)
    , pressing_value_(-1)
    , label_(0)
    , time_out_(time_out)
{
    SysStatus & sys_status = SysStatus::instance();
    SystemConfig conf;
    min_ = conf.minVolume();
    max_ = conf.maxVolume();
    volumes = conf.volumes().mid(0);
    conf.close();

    current_ = sys_status.volume() - min_;

    // connect the signals with sys_state_
    connect(&sys_status, SIGNAL(volumeChanged(int, bool)), this, SLOT(setVolume(int, bool)));

    createLayout();
    setModal(true);
    setFixedSize(300, 300);
    //setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    setFocusPolicy(Qt::NoFocus);

    timer_.setSingleShot(true);
    connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

VolumeControlDialog::~VolumeControlDialog()
{
}

void VolumeControlDialog::createLayout()
{
    // hbox to layout line edit and buttons.
    layout_.setContentsMargins(4, 4, 4, 4);
    layout_.setSpacing(2);
    layout_.addWidget(&label_, 0, Qt::AlignHCenter|Qt::AlignTop);
    label_.setAlignment(Qt::AlignCenter);

    QPixmap pixmap=QPixmap::fromImage(image_);
    label_.setPixmap(pixmap);
}

void VolumeControlDialog::done(int r)
{
    sys::SysStatus::instance().enableIdle(true);
    stopTimer();
    QDialog::done(r);
}

void VolumeControlDialog::resetTimer()
{
    timer_.stop();
    if (0 != time_out_)
    {
        timer_.start(time_out_);
    }
    else
    {
        timer_.start(TIMEOUT);
    }
}

int VolumeControlDialog::ensureVisible()
{
    show();
    const QRect screen = QApplication::desktop()->screenGeometry();
    move( screen.center() - this->rect().center() );
   
    resetTimer();
    sys::SysStatus::instance().enableIdle(false);
    onyx::screen::watcher().addWatcher(this);
    int ret = exec();
    onyx::screen::watcher().removeWatcher(this);
    return ret;
}

void VolumeControlDialog::moveEvent(QMoveEvent *e)
{
}

void VolumeControlDialog::resizeEvent(QResizeEvent *e)
{
    QPainterPath p;
    p.addRoundedRect(rect(), 10, 10);
    QRegion maskedRegion(p.toFillPolygon().toPolygon());
    setMask(maskedRegion);
}

void VolumeControlDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QBrush(QColor(190, 190, 190)));

    for (int i = 1; i < volumes.size(); ++i)
    {
        painter.fillRect(rectForVolume(i-1), current_ >= volumes[i] ? Qt::black : Qt::white);
    }
}

void VolumeControlDialog::mouseMoveEvent(QMouseEvent *me)
{
    me->accept();
}

void VolumeControlDialog::mousePressEvent(QMouseEvent *me)
{
    // Check position.
    resetTimer();
    me->accept();

    QRect rc;
    int value=current_;
    for(int i = 1; i < volumes.size(); ++i)
    {
        rc = rectForVolume(i-1);
        if (rc.contains(me->pos()))
        {
            value = volumes[i];
            break;
        }
    }

    sys::SysStatus::instance().setVolume(value);
#ifndef BUILD_FOR_ARM
    setVolume(value, false);
#endif

}

void VolumeControlDialog::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    if (!rect().contains(me->pos()))
    {
        reject();
    }
}

void VolumeControlDialog::onScreenUpdateRequest()
{
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU, false, onyx::screen::ScreenCommand::WAIT_COMMAND_FINISH);
}


bool VolumeControlDialog::event(QEvent *e)
{
    int ret = QDialog::event(e);
    return ret;
}

void VolumeControlDialog::keyPressEvent(QKeyEvent *ke)
{
    ke->accept();
}

int VolumeControlDialog::getVolumeIndex(int volume_value)
{
    int volume_index = -1;
    int size = volumes.size();
    for(int i = 0; i < size; ++i)
    {
        if (volume_value == volumes[i])
        {
            volume_index = i;
            break;
        }
    }
    return volume_index;
}

void VolumeControlDialog::manipulateVolume(bool increase)
{
    resetTimer();
    int volume_index = getVolumeIndex(current_);
    int size = volumes.size();
    if (increase && ++volume_index >= size)
    {
        volume_index = size-1;
    }

    if (!increase && --volume_index < 0)
    {
        volume_index = 0;
    }

    sys::SysStatus::instance().setVolume(volumes[volume_index]);
    setVolume(volumes[volume_index], false);
}

void VolumeControlDialog::keyReleaseEvent(QKeyEvent *ke)
{
    int key = ke->key();
    if (key == Qt::Key_Escape)
    {
        done(QDialog::Rejected);
        ke->accept();
        return;
    }

    switch (key)
    {
    case Qt::Key_Left:
        manipulateVolume(false);
        break;
    case Qt::Key_Right:
        manipulateVolume(true);
        break;
    default:
        break;
    }
    ke->accept();
}

void VolumeControlDialog::stopTimer()
{
    timer_.stop();
}

void VolumeControlDialog::setVolume(int volume, bool is_mute)
{
    if (current_ == volume)
    {
        return;
    }
    current_ = volume;
    repaint();
    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::DW, onyx::screen::ScreenCommand::WAIT_NONE);
}

void VolumeControlDialog::onTimeout()
{
    stopTimer();
    accept();
}

QRect VolumeControlDialog::rectForVolume(int index)
{
    int left = 0, right = 0, bottom = 10;
    int spacing = 4 * layout_.spacing();
    layout_.getContentsMargins(&left, 0, &right, 0);

    int x = spacing + left;
    int delta = 8;
    int my_width = (width() - left - right) / (volumes.size()-1) - spacing;

    x += (my_width + spacing) * index;
    int h = 30 + index * delta;
    return QRect(x, height() - h - bottom, my_width, h);
}

}   // namespace ui

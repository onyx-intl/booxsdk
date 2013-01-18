
#include "onyx/sys/sys.h"
#include "onyx/ui/ui_utils.h"
#include "onyx/ui/glow_light_control_dialog.h"
#include "onyx/data/data_tags.h"
#include "onyx/screen/screen_update_watcher.h"


namespace ui
{

static const int ITEM_HEIGHT = 50;
static const int MY_WIDTH = 20;
static const int MY_HEIGHT = 240;
static const int TOTAL_RECT = 50;

MoonLightProgressBar::MoonLightProgressBar(QWidget *parent)
    : QWidget(parent)
    , max_value_(100)
    , min_value_(1)
    , value_(1)
    , step_value_(10)
{
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Base);
    update();
    onyx::screen::watcher().addWatcher(this);
}

MoonLightProgressBar::~MoonLightProgressBar()
{

}

void MoonLightProgressBar::setRange(int min, int max)
{
    min_value_ = min;
    max_value_ = max;
    step_value_ = width()/max_value_;
}

void MoonLightProgressBar::setValue(int value)
{
    if(value <1 || value > maximum())
    {
        return;
    }

    if(value != value_)
    {
        emit valueChanged(value);
    }
    value_ = value;
    update();
    onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::DW);
}

void MoonLightProgressBar::addValue()
{
    int value = value_ +maximum()/TOTAL_RECT;
    if(value > maximum())
    {
        value = maximum();
    }
    setValue(value);
}

void MoonLightProgressBar::subValue()
{
    int value = value_ - maximum()/TOTAL_RECT;
    if(value < 0)
    {
        value = 1;
    }
    setValue(value);
}

void MoonLightProgressBar::changePoint(QPoint &pos)
{
    step_value_ = width()/max_value_;
    if (rect().contains(pos))
    {
        double x = (double)pos.x();
        if(pos.x()%step_value_==0 || pos.x()<=30 || pos.x()>=(width()-80))
        {
            double percentage = x / width();
            percentage = percentage+0.005;
            if(percentage >= 1.0)
            {
                percentage=1.0;
            }
            setValue(percentage*maximum());
        }
    }
}

void MoonLightProgressBar::mousePressEvent(QMouseEvent *event)
{
    event->accept();
}

void MoonLightProgressBar::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    QPoint pt = me->pos();
    if (rect().contains(pt))
    {
        double x = (double)pt.x();
        double percentage = x / width();
        percentage = percentage+0.005;
        if(percentage >= 1.0)
        {
            percentage=1.0;
        }
        setValue(percentage*maximum());
    }
}

void MoonLightProgressBar::mouseMoveEvent(QMouseEvent *me)
{
    QPoint pt = me->pos();
    changePoint(pt);
}

void MoonLightProgressBar::paintEvent(QPaintEvent *event)
{
    double index = (double)(value_*TOTAL_RECT*1.0/max_value_);
    index = index+0.5;
    qDebug() << "index=" << index << "\n";
    QWidget::paintEvent(event);
    QPainter painter(this);
    QPen draw_pen= painter.pen();
    draw_pen.setWidth(draw_pen.width()+1);
    painter.setPen(draw_pen);

    painter.fillRect(this->rect(), QBrush(Qt::white));
    int i = 0;
    for(  ; i<=(int)index; i++)
    {
        painter.fillRect(QRect(rect().x()+i*width()/TOTAL_RECT, rect().y()+2,
                      width()/TOTAL_RECT-4, rect().height()-4), QBrush(Qt::black));
    }

    for(; i<=TOTAL_RECT; i++)
    {
        painter.drawRect(QRect(rect().x()+i*width()/TOTAL_RECT, rect().y()+2,
                               width()/TOTAL_RECT-4,rect().height()-4));
    }
}



GlowLightControlDialog::GlowLightControlDialog(QWidget *parent)
: QDialog(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
, layout_(this)
, slider_h_layout_(0)
, h_layout_(0)
, ok_h_layout_(0)
, title_(0)
, slider_(0)
, switch_view_(0, this)
, ok_view_(0, this)
, add_light_view_(0, this)
, sub_light_view_(0, this)
{
    setModal(true);
    createLayout();
    updateText();

    onyx::screen::watcher().addWatcher(this);
}

GlowLightControlDialog::~GlowLightControlDialog(void)
{
}

int GlowLightControlDialog::exec()
{
    int margin = 20;
    int offset = 40;
    int width = QApplication::desktop()->screenGeometry().width() - 2 * margin;
    int height = QApplication::desktop()->screenGeometry().height();
    setFixedWidth(width);
    setFixedHeight(MY_HEIGHT);
    show();
    move(margin, height - MY_HEIGHT - offset);
    onyx::screen::watcher().enqueue(0, onyx::screen::ScreenProxy::GC);
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC);
    return QDialog::exec();
}

void GlowLightControlDialog::createLayout()
{
    layout_.setContentsMargins(MY_WIDTH, 2, MY_WIDTH, 30);

    createSwitchView();
    createOKView();
    createSubLightView();
    createAddLightView();

    h_layout_.addWidget(&switch_view_);
    layout_.addLayout(&h_layout_);
    layout_.addSpacing(4);

    title_.setAlignment(Qt::AlignCenter);
    layout_.addWidget(&title_);


//    slider_h_layout_.setContentsMargins(30, 0, 30, 0);
    slider_h_layout_.setContentsMargins(0, 0, 0, 0);
    slider_h_layout_.addWidget(&sub_light_view_);
    slider_h_layout_.addWidget(&slider_);
    slider_h_layout_.addWidget(&add_light_view_);

    // TODO may need different range
    slider_.setRange(1, 130);
    slider_.setFixedHeight(55);
    slider_.setValue(sys::SysStatus::instance().brightness());

    layout_.addLayout(&slider_h_layout_);
    layout_.addSpacing(10);

    ok_h_layout_.setContentsMargins(450, 0, 0, 0);
    ok_h_layout_.addStretch(0);
    ok_h_layout_.addWidget(&ok_view_);
    layout_.addStretch(0);
    layout_.addLayout(&ok_h_layout_);

    connect(&slider_, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));

}

void GlowLightControlDialog::createSwitchView()
{
    switch_view_.setSubItemType(ui::CheckBoxView::type());
    switch_view_.setPreferItemSize(QSize(360, ITEM_HEIGHT));

    bool checked = sys::SysStatus::instance().glowLightOn();

    ODatas d;

    OData * item = new OData;
    item->insert(TAG_TITLE, tr("Turn on MOON Light"));
    item->insert(TAG_CHECKED, checked);
    d.push_back(item);

    switch_view_.setMinimumHeight( ITEM_HEIGHT );
    switch_view_.setMinimumWidth(360);
    switch_view_.setData(d, true);

    QObject::connect(&switch_view_, SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
                     this, SLOT(onSwitchClicked(CatalogView*, ContentView*, int)));

}

void GlowLightControlDialog::createOKView()
{
    ok_view_.setSubItemType(ui::CoverView::type());
    ok_view_.setPreferItemSize(QSize(-1, 40));
    ODatas d;

    OData * item = new OData;
    item->insert(TAG_TITLE, tr("OK"));
    d.push_back(item);

    ok_view_.setMinimumHeight( 42 );
    ok_view_.setFixedGrid(1, 1);
    ok_view_.setFixedWidth(160);
    ok_view_.setData(d, true);

    QObject::connect(&ok_view_, SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
                     this, SLOT(onOkClicked()));
}

void GlowLightControlDialog::createAddLightView()
{
    add_light_view_.setSubItemType(ui::CoverView::type());
    add_light_view_.setPreferItemSize(QSize(-1, 40));

    ODatas d;
    OData * item = new OData;
    QPixmap pixmap(":/images/add.png");
    item->insert(TAG_COVER,  pixmap);
    d.push_back(item);

    add_light_view_.setMinimumHeight( 42 );
    add_light_view_.setFixedGrid(1, 1);
    add_light_view_.setFixedWidth(60);
    add_light_view_.setData(d, true);

    QObject::connect(&add_light_view_, SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
                     &slider_, SLOT(addValue()));
}

void GlowLightControlDialog::createSubLightView()
{
    sub_light_view_.setSubItemType(ui::CoverView::type());
    sub_light_view_.setPreferItemSize(QSize(-1, 40));

    ODatas d;
    OData * item = new OData;
    QPixmap pixmap(":/images/sub.png");
    item->insert(TAG_COVER,  pixmap);
    d.push_back(item);

    sub_light_view_.setMinimumHeight( 42 );
    sub_light_view_.setFixedGrid(1, 1);
    sub_light_view_.setFixedWidth(60);
    sub_light_view_.setData(d, true);

    QObject::connect(&sub_light_view_, SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
                     &slider_, SLOT(subValue()));
}


void GlowLightControlDialog::onSwitchClicked(CatalogView *catalog, ContentView *item, int user_data)
{
    if (!item || !item->data())
    {
        return;
    }

    bool checked = item->data()->value(TAG_CHECKED).toBool();
    item->data()->insert(TAG_CHECKED, !checked);

    sys::SysStatus::instance().turnGlowLightOn(!checked, true);

    update();
    onyx::screen::watcher().enqueue(catalog, onyx::screen::ScreenProxy::GU);
}

void GlowLightControlDialog::onOkClicked()
{
    qDebug() << "on ok clicked";
    accept();

    onyx::screen::watcher().removeWatcher(this);
    onyx::screen::watcher().enqueue(0, onyx::screen::ScreenProxy::GC);
}

void GlowLightControlDialog::updateText()
{
    title_.setText(tr("Brightness"));
}

void GlowLightControlDialog::onValueChanged(int v)
{
    qDebug() << "Value changed " << v;
    if (switch_view_.data().size() > 0 && !switch_view_.data().at(0)->value(TAG_CHECKED).toBool())
    {
        ODatas d;
        OData * item = new OData;
        item->insert(TAG_CHECKED, true);
        item->insert(TAG_TITLE, tr("Turn on MOON Light"));
        d.push_back(item);
        switch_view_.data().clear();
        switch_view_.setData(d, true);
    }
    onyx::screen::watcher().enqueue(&switch_view_, onyx::screen::ScreenProxy::GU);
    sys::SysStatus::instance().setBrightness(v);
}

void GlowLightControlDialog::keyPressEvent(QKeyEvent * ke)
{
    ke->accept();
    if (ke->key() == Qt::Key_Escape)
    {
        return;
    }
}

void GlowLightControlDialog::keyReleaseEvent(QKeyEvent * ke)
{
    ke->accept();
    if (ke->key() == Qt::Key_Escape)
    {
        reject();
        return;
    }
}

}  // namespace ui




#include "onyx/ui/onyx_dialog.h"
#include "onyx/screen/screen_proxy.h"

namespace ui
{

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

OnyxDialog::OnyxDialog(QWidget *parent, bool show_shadow)
    : QDialog(parent, Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint)
    , show_shadows_(show_shadow)
    , shadows_(parent)
    , vbox_(this)
    , title_widget_(this)
    , title_vbox_(&title_widget_)
    , title_hbox_(0)
    , top_separator_(this)
    , title_icon_label_(this)
    , title_text_label_(this)
    , close_button_("", this)
{
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Button);

    createDefaultLayout();
}

OnyxDialog::~OnyxDialog(void)
{
}

void OnyxDialog::createDefaultLayout()
{
    vbox_.setSizeConstraint(QLayout::SetMinAndMaxSize);
    vbox_.setSpacing(SPACING);
    vbox_.setContentsMargins(0, 0, 0, 0);

    title_vbox_.setSpacing(0);
    title_vbox_.setContentsMargins(SPACING, 0, SPACING*5, 0);

    // title hbox.
    title_icon_label_.setPixmap(QPixmap(":/images/dialog.png"));
    title_text_label_.setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    title_icon_label_.setFixedHeight(defaultItemHeight());
    title_text_label_.setFixedHeight(defaultItemHeight());
    title_text_label_.useTitleBarStyle();

    close_button_.setStyleSheet(BUTTON_STYLE);
    QPixmap close_pixmap(":/images/close.png");
    close_button_.setIconSize(close_pixmap.size());
    close_button_.setIcon(QIcon(close_pixmap));
    close_button_.setFocusPolicy(Qt::NoFocus);
    connect(&close_button_, SIGNAL(clicked()), this, SLOT(onCloseClicked()), Qt::QueuedConnection);
    connect(&close_button_, SIGNAL(pressed()), this, SLOT(onClosePressed()), Qt::QueuedConnection);

    title_hbox_.addWidget(&title_icon_label_);
    title_hbox_.addSpacing(SPACING * 4);
    title_hbox_.addWidget(&title_text_label_, 500);
    title_hbox_.addWidget(&close_button_);
    title_hbox_.setContentsMargins(SPACING << 1, SPACING, SPACING << 1, SPACING);
    title_vbox_.addLayout(&title_hbox_);

    // title seperator
    top_separator_.setFocusPolicy(Qt::NoFocus);
    top_separator_.setFixedHeight(1);
    top_separator_.setFrameShape(QFrame::HLine);
    top_separator_.setAutoFillBackground(true);
    top_separator_.setBackgroundRole(QPalette::Light);
    title_vbox_.addWidget(&top_separator_);

    // title widget.
    title_widget_.setAutoFillBackground(true);
    title_widget_.setBackgroundRole(QPalette::Text);
    title_widget_.setContentsMargins(0, 0, 0, 0);
    title_widget_.setFixedHeight(defaultItemHeight() + SPACING * 2);
    vbox_.addWidget(&title_widget_);

    // content widget.
    content_widget_.setAutoFillBackground(true);
    content_widget_.setBackgroundRole(QPalette::Mid);
    content_widget_.setContentsMargins(2 * SPACING, SPACING, 2 * SPACING, SPACING);
    vbox_.addWidget(&content_widget_);
}

void OnyxDialog::updateTitle(const QString &message)
{
    title_text_label_.setText(message);
}

void OnyxDialog::updateTitleIcon(const QPixmap& pixmap)
{
    title_icon_label_.setPixmap(pixmap);
}

void OnyxDialog::showCloseButton(bool show)
{
    close_button_.setVisible(show);
}

QRect OnyxDialog::outbounding(QWidget *parent)
{
    QRect rc(rect());
    if (show_shadows_)
    {
        rc.adjust(0, 0, Shadows::PIXELS, Shadows::PIXELS);
    }
    rc.moveTo(mapToGlobal(rc.topLeft()));
    return rc;
}

void OnyxDialog::done(int r)
{
    onyx::screen::instance().ensureUpdateFinished();
    return QDialog::done(r);
}

void OnyxDialog::onClosePressed()
{
    QPixmap close_pressed_pixmap(":/images/close_pressed.png");
    close_button_.setIcon(QIcon(close_pressed_pixmap));
    onyx::screen::instance().updateWidget(&close_button_, onyx::screen::ScreenProxy::GU);
}

void OnyxDialog::onCloseClicked()
{
    shadows_.show(false);
    done(QDialog::Rejected);
}

void OnyxDialog::closeEvent(QCloseEvent * event)
{
    QDialog::closeEvent(event);
}


void OnyxDialog::moveEvent(QMoveEvent *e)
{
    if (show_shadows_)
    {
        shadows_.onWidgetMoved(this);
    }
}

void OnyxDialog::resizeEvent(QResizeEvent *e)
{
    /*
    QPainterPath p;
    p.addRoundedRect(rect(), 10, 10);
    QRegion maskedRegion(p.toFillPolygon().toPolygon());
    setMask(maskedRegion);
    */

    if (show_shadows_)
    {
        shadows_.onWidgetResized(this);
    }
}

void OnyxDialog::hideEvent(QHideEvent * event)
{
    QDialog::hideEvent(event);
    shadows_.show(false);
}

void OnyxDialog::keyPressEvent(QKeyEvent * event)
{
    event->accept();
}

void OnyxDialog::keyReleaseEvent(QKeyEvent * ke)
{
    ke->accept();
    if (ke->key() == Qt::Key_Escape)
    {
        reject();
    }
}

int OnyxDialog::spacing()
{
    return SPACING;
}

int OnyxDialog::defaultItemHeight()
{
    return 36;
}

}   // namespace ui


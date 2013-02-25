#include <QFont>
#include "onyx/ui/screen_rotation_dialog.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/sys/sys_status.h"


static const QString TAG_INDEX = "lang_index";
static const int HEIGHT = 252;
static const int WIDTH = 252;

namespace ui
{
    ScreenRotationDialog::ScreenRotationDialog(QWidget *parent)
        :OnyxDialog(parent)
    {
        setFixedSize(WIDTH, HEIGHT);
        createLayout();
        setModal(true);
    }

    ScreenRotationDialog::~ScreenRotationDialog()
    {
        //
    }

    void ScreenRotationDialog::closeEvent(QCloseEvent *)
    {
        //onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GU);
    }

    void ScreenRotationDialog::keyPressEvent(QKeyEvent *ke)
    {
        ke->accept();
    }

    void ScreenRotationDialog::keyReleaseEvent(QKeyEvent *ke)
    {
        ke->accept();
        switch (ke->key())
        {
        case Qt::Key_Up:
            accept();
            rotation_ = UP;
            break;
        case Qt::Key_Left:
            accept();
            rotation_ = LEFT;
            break;
        case Qt::Key_Right:
            accept();
            rotation_ = RIGHT;
            break;
        case Qt::Key_Down:
            accept();
            rotation_ = DOWN;
            break;
        case Qt::Key_Escape:
            reject();
            break;
        case Qt::Key_Return:
            break;
        }
    }

    void ScreenRotationDialog::mousePressEvent(QMouseEvent *e)
    {
        e->accept();
    }

    void ScreenRotationDialog::mouseReleaseEvent(QMouseEvent *e)
    {
        e->accept();
        int x= e->pos().x();
        int y= e->pos().y();
        int w=width();
        int h=height();


        if((x < y) &&
            y < (h-h*x/w))
        {
            accept();
            rotation_ = LEFT;
        }
        else if((x > y) &&
                (y > (h-h*x/w)))
        {
            accept();
            rotation_ = RIGHT;
        }
        else if((x < y) &&
                (y > (h-h*x/w)))
        {
            accept();
            rotation_ = DOWN;
        }
        else if((x > y) &&
                (y < (h-h*x/w)))
        {
            accept();
            rotation_ = UP;
        }
    }

    int ScreenRotationDialog::popup()
    {
        show();
        onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC, true, onyx::screen::ScreenCommand::WAIT_ALL);
        int ret = QDialog::exec();
        if (ret == QDialog::Accepted)
        {
            rotate(rotation_);
        }
        return ret;
    }

    void ScreenRotationDialog::createLayout()
    {
        title_text_label_.setText(QApplication::tr("Screen Rotation"));
        vbox_.addWidget(&rotation_widget_);
        vbox_.addStretch();
    }

    void ScreenRotationDialog::rotate(Type t)
    {
        int degree = sys::SysStatus::instance().screenTransformation();
        switch(t)
        {
        case UP:
            degree = (degree+180)%360;
            sys::SysStatus::instance().setScreenTransformation(degree);
            break;
        case DOWN:
            //degree = (degree+180)%360;
            //sys::SysStatus::instance().setScreenTransformation(0);
            break;
        case LEFT:
            degree = (degree+270)%360;
            sys::SysStatus::instance().setScreenTransformation(degree);
            break;
        case RIGHT:
            degree = (degree+90)%360;
            sys::SysStatus::instance().setScreenTransformation(degree);
            break;
        }

       return;
    }

    RotationWidget::RotationWidget(QWidget *parent)
        :QWidget(parent)
    {
        setFixedSize(250, 250);
        if (findFont("Droid Serif") ||
            findFont("Droid Sans") ||
            findFont("DejaVu Sans"))
            return;
    }

    bool RotationWidget::findFont(QString name)
    {
        fontName = "";
        QFont font(name);
        if (font.family() != name)
            return false;
        fontName = name;
        return true;
    }

    void RotationWidget::paintEvent(QPaintEvent *)
    {
        int n = width() > height() ?height() :width();

        QPainter painter(this);
        painter.setPen(QPen(Qt::black, 5.0));
        QFont font(fontName, 32, QFont::Normal);
        painter.setFont(font);

        painter.drawRect(width()/2-n/4, height()/2-n/4, n/2, n/2);
        QBrush brush(Qt::black);
        painter.setBrush(brush);
        painter.drawRect(width()/2-n/4+6, height()/2-n/4+6, n/2-12, n/2-12);
        brush.setColor(Qt::white);
        painter.setBrush(brush);
        painter.drawEllipse(width()/2-n/4+12, height()/2-n/4+12, n/2-24, n/2-24);

        int triangle[3][2];
        triangle[0][0]=width()/2;
        triangle[0][1]=height()/2+n/4-12;
        triangle[1][0]=width()/2+10;
        triangle[1][1]=height()/2+n/4-25;
        triangle[2][0]=width()/2-10;
        triangle[2][1]=height()/2+n/4-25;
        brush.setColor(Qt::black);
        painter.setBrush(brush);
        painter.setPen(QPen(Qt::black, 1.0));
        painter.drawPolygon(QPolygon(3,&triangle[0][0]));


        QTransform transform;
        transform.translate(width()/2, height()/2);
        transform.rotate(0);
        painter.setWorldTransform(transform);
        painter.drawText(-n/4, n/8, n/2, n/2, Qt::AlignVCenter | Qt::AlignHCenter, "ABC");

        transform.reset();
        transform.translate(width()/2, height()/2);
        transform.rotate(90.0);
        painter.setWorldTransform(transform);
        painter.drawText(-n/4, n/8, n/2, n/2, Qt::AlignVCenter | Qt::AlignHCenter, "ABC");

        transform.reset();
        transform.translate(width()/2, height()/2);
        transform.rotate(180.0);
        painter.setWorldTransform(transform);
        painter.drawText(-n/4, n/8, n/2, n/2, Qt::AlignVCenter | Qt::AlignHCenter, "ABC");

        transform.reset();
        transform.translate(width()/2, height()/2);
        transform.rotate(270.0);
        painter.setWorldTransform(transform);
        painter.drawText(-n/4, n/8, n/2, n/2, Qt::AlignVCenter | Qt::AlignHCenter, "ABC");
    }
}

#ifndef ONYX_TEXT_BROWSER_H_
#define ONYX_TEXT_BROWSER_H_

#include "onyx/base/base.h"
#include <QtGui/QtGui>

namespace ui
{

/// Text browser for eink device.
class OnyxTextBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    OnyxTextBrowser(QWidget *parent);
    ~OnyxTextBrowser();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent ( QMouseEvent * event );
    virtual void resizeEvent ( QResizeEvent * event );
    virtual void keyPressEvent(QKeyEvent * ev );
    virtual void keyReleaseEvent(QKeyEvent * ev );
    virtual void paintEvent(QPaintEvent *event);
    void    updateScreen();

private:
    scoped_ptr<QKeyEvent> key_event_;
    QPoint pressed_point_;
};

};

#endif  // ONYX_TEXT_BROWSER_H_

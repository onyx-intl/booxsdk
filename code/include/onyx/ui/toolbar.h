#ifndef ONYX_TOOL_BAR_H_
#define ONYX_TOOL_BAR_H_

#include "onyx/base/base.h"
#include <QtGui/QtGui>

namespace ui
{

class OnyxToolBarItem : public QWidget
{
    Q_OBJECT
public:
    OnyxToolBarItem(QWidget *parent, QAction &action);
    ~OnyxToolBarItem();

    QAction *ownAction();

Q_SIGNALS:
    void clicked(QAction *action);
    void focusDown(QAction *action);
    void focusUp(QAction *action);
    void focusSibling(OnyxToolBarItem *current, int key);

private Q_SLOTS:
    void onActionChanged();

private:
    QSize iconSize();
    void mousePressEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);
    void keyPressEvent(QKeyEvent *ke);
    void keyReleaseEvent(QKeyEvent *ke);
    void paintEvent(QPaintEvent *pe);

private:
    QAction &action_;
    int state_;
};
typedef shared_ptr<OnyxToolBarItem> OnyxToolBarItemPtr;

/// Tool bar for eink device. Remove unnecessary updates.
class OnyxToolBar : public QWidget
{
    Q_OBJECT

public:
    explicit OnyxToolBar(QWidget *parent = 0);
    ~OnyxToolBar();

public:
    void addAction(QAction *action);
    void clear();

    std::vector<OnyxToolBarItemPtr> subItems();

Q_SIGNALS:
    void focusDown(QAction *action);
    void focusUp(QAction *action);

protected:
    bool event(QEvent *e);
    void paintEvent(QPaintEvent *pe);

private Q_SLOTS:
    // only supports horizontally
    void onFocusSibling(OnyxToolBarItem *current, int key);

private:
    std::vector<OnyxToolBarItemPtr> items_;
    QHBoxLayout layout_;

};

}; // namespace ui

#endif  // ONYX_TOOL_BAR_H_

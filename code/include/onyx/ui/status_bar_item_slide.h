#ifndef ONYX_STATUS_BAR_ITEM_PROGRESS_H_
#define ONYX_STATUS_BAR_ITEM_PROGRESS_H_

#include "status_bar_item.h"

namespace ui
{

class StatusBarItemProgress : public StatusBarItem
{
    Q_OBJECT

public:
    StatusBarItemProgress(QWidget *parent);
    virtual ~StatusBarItemProgress(void);

public Q_SLOTS:
    void setProgress(const int current, const int total,
            const bool show_message = true,
            const QString &message = "");
    void progress(int & current, int & total);

Q_SIGNALS:
    void clicked(const int percent, const int value);
    void changing(const int value, const int total);

private Q_SLOTS:
    void onTimeout();

private:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void resizeEvent(QResizeEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent *);

    void onSlideBarClicked(const int percent, const int value);
    void resetSlideBar(const int current, const int total);

    void createLayout();
    void updatefgPath(int value);
    void updatePath(QPainterPath & path, const QRect & rect);

    void drawMessage(QPainter &painter);
    void initFont();

private:
    int current_;
    int total_;
    int pressing_value_;
    bool show_message_;
    QString message_;
    QTimer timer_;

    QPainterPath bk_path_;
    QPainterPath fg_path_;

    QFont font_;
};

};  // namespace ui

#endif    // ONYX_STATUS_BAR_ITEM_PROGRESS_H_

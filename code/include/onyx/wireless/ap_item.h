#ifndef WIFI_AP_ITEM_H_
#define WIFI_AP_ITEM_H_

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include "onyx/ui/ui.h"
#include "onyx/sys/sys.h"
#include "onyx/ui/catalog_view.h"

using namespace sys;

namespace ui
{

class WifiTitleItem : public QWidget
{
    Q_OBJECT

public:
    WifiTitleItem(QWidget *parnet);
    ~WifiTitleItem();

public Q_SLOTS:
    void setState(const QString &);
    void onItemActivated(CatalogView *catalog, ContentView *item, int user_data);
    CatalogView& dashBoard() { return dash_board_; }

Q_SIGNALS:
    void refreshClicked();
    void customizedClicked();
    void backClicked();

protected:
    virtual bool event(QEvent *e);

private:
    void createLayout();
    void createDashBoard();

private:
    QHBoxLayout layout_;
    OnyxLabel icon_label_;
    OnyxLabel title_label_;
    ui::CatalogView dash_board_;
};


/// Represents a wifi access point. It displays the access point
/// state to end user.
class WifiAPItem : public ui::ContentView
{
    Q_OBJECT

public:
    WifiAPItem(QWidget *parent);
    ~WifiAPItem(void);

public:
    virtual void updateView();
    void activateItem();

public:
    void setProfile(WifiProfile & profile);
    WifiProfile & profile();

Q_SIGNALS:
    void clicked(WifiProfile & profile);
    void config(WifiProfile & profile);

protected:
    virtual bool event(QEvent *e);
    virtual void paintEvent(QPaintEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    void focusInEvent(QFocusEvent * event);
    void focusOutEvent(QFocusEvent * event);

private:
    void createLayout();
    void updateByProfile(WifiProfile & profile);
    bool isSelected();
    bool isAPChecked();

    int getSignalStrength(int level_in_profile);

private Q_SLOTS:
    void onConfigButtonClicked();

private:
    QHBoxLayout hor_layout_;
    OnyxLabel status_icon_label_;
    OnyxLabel ssid_label_;
    OnyxPushButton config_button_;
    OnyxLabel lock_icon_label_;
    OnyxLabel signal_icon_label_;
    int signal_count_;

    static const int SIGNAL_ICONS = 4;
    OnyxLabel signal_labels_[SIGNAL_ICONS];
    WifiProfile profile_;


    static WifiAPItem *selected_item_;
};

typedef QVector<WifiAPItem *> WifiAPItems;
typedef WifiAPItems::iterator WifiAPItemsIter;

}    // namespace ui

#endif

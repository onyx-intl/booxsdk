#ifndef ONYX_STATUS_BAR_ITEM_H_
#define ONYX_STATUS_BAR_ITEM_H_

#include "onyx/base/base.h"
#include "onyx/ui/ui_global.h"

namespace ui
{

enum StatusBarItemType
{
    INVALID_ITEM    = 0x00,
    MENU            = 0x01,
    PROGRESS        = 0x02,
    MESSAGE         = 0x04,
    BATTERY         = 0x08,
    CONNECTION      = 0x10,
    STYLUS          = 0x20,
    CLOCK           = 0x40,
    SCREEN_REFRESH  = 0x80,
    INPUT_URL       = 0x100,
    INPUT_TEXT      = 0x200,
    THREEG_CONNECTION  = 0x400,
    VOLUME          = 0x800,
    MUSIC_PLAYER    = 0X1000,
    VIEWPORT        = 0X2000,
    APP_DEFINED     = 0x4000,
    APP_CONFIG      = 0x8000,
    TASK_MANAGEMENT = 0x10000,
};

enum StatusBarItemState
{
    STATE_INVALID = -1,
    STATE_NORMAL,
    STATE_SELECTED,
    STATE_DISABLED,
    STATE_COUNT
};

Q_DECLARE_FLAGS(StatusBarItemTypes, StatusBarItemType)
Q_DECLARE_OPERATORS_FOR_FLAGS(StatusBarItemTypes)

/// The status bar item widget. The caller does not
/// need to know the item type.
class StatusBarItem  : public QWidget
{
    Q_OBJECT
public:
    StatusBarItem(const StatusBarItemType type, QWidget *parent);
    virtual ~StatusBarItem(void);

public:
    StatusBarItemType type() const { return type_; }
    int state() const { return state_; }
    void setState(const int state);
    QString getIconPath(QString str);

private:
    StatusBarItemType type_;
    int state_;
};

typedef QMap<int, QImage> Images;
typedef Images::iterator ImagesIter;

}   // namespace ui

#endif  // ONYX_STATUS_BAR_ITEM_H_

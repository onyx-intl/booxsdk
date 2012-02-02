#ifndef BROWSER_NAVIGATION_ACTIONS_H_
#define BROWSER_NAVIGATION_ACTIONS_H_

#include <QtWebKit/QWebHistory>

#include "onyx/base/base.h"
#include "onyx/ui/context_dialog_base.h"


namespace ui
{

enum NavigationType
{
    NAVIGATE_NONE,
    NAVIGATE_FORWARD,
    NAVIGATE_BACKWARD,
    NAVIGATE_SHOW_HISTORY,
    NAVIGATE_HOME,
    NAVIGATE_CLEAR_HISTORY,
    NAVIGATE_HYPER_LINK_VIA_KEYBOARD,
};

class BrowserNavigationActions : public BaseActions
{
public:
    BrowserNavigationActions(void);
    ~BrowserNavigationActions(void);

public:
    /// Generate or re-generate the view type actions group.
    void generateActions(QWebHistory *history,
            bool enable_hyperlink_navigation = false,
            bool hyperlink_navigation_mode = false);

    /// Retrieve the selected action.
    NavigationType selected();

    void enableHistory(bool enable);

private:
    bool enable_history_ ;
};

};

#endif //  BROWSER_NAVIGATION_ACTIONS_H_

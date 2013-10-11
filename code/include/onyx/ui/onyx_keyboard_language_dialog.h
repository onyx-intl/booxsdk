#ifndef ONYX_KEYBOARD_LANGUAGE_DIALOG_H_
#define ONYX_KEYBOARD_LANGUAGE_DIALOG_H_

#include "onyx_keyboard_utils.h"
#include "onyx/ui/onyx_dialog.h"
#include "onyx/ui/catalog_view.h"

namespace ui
{

class OnyxKeyboardLanguageDialog: public OnyxDialog
{
    Q_OBJECT

public:
    OnyxKeyboardLanguageDialog(QLocale language, QWidget *parent = 0);
    ~OnyxKeyboardLanguageDialog();

    inline QLocale & selectedLocale() { return language_; }

private Q_SLOTS:
    void onItemActivated(CatalogView *catalog, ContentView *item, int user_data);

private:
    void createLayout();
    void createLanguageGroup();

    QLocale getLocale(const OData *data);

    bool enableGeorgian();

private:
    QVBoxLayout layout_;
    CatalogView language_group_;
    ODatas language_group_datas_;
    QLocale language_;
};

}

#endif

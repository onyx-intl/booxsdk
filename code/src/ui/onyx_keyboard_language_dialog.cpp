#include "onyx/ui/onyx_keyboard_language_dialog.h"
#include "onyx/ui/factory.h"
#include "onyx/ui/content_view.h"
#include "onyx/data/data.h"
#include "onyx/ui/ui_utils.h"

namespace ui
{

static const QString TAG_INDEX = "lang_index";

/// Always use UTF-8.
struct LocaleItem
{
    const char * text;
    QLocale locale;
};

// Define all supported languages.
static const char* SCOPE = "LocalePage";
static const LocaleItem LANGUAGES[] =
{
    {QT_TRANSLATE_NOOP("LocalePage", "English"),  QLocale(QLocale::English)},
    {QT_TRANSLATE_NOOP("LocalePage", "Russian"),  QLocale(QLocale::Russian)},
    {QT_TRANSLATE_NOOP("LocalePage", "Spanish"),  QLocale(QLocale::Spanish)},
    {QT_TRANSLATE_NOOP("LocalePage", "Poland"),  QLocale(QLocale::Polish)},
    {QT_TRANSLATE_NOOP("LocalePage", "Swedish"),  QLocale(QLocale::Swedish)},
    {QT_TRANSLATE_NOOP("LocalePage", "Hungarian"),  QLocale(QLocale::Hungarian)},
    {QT_TRANSLATE_NOOP("LocalePage", "Georgian"),  QLocale(QLocale::Georgian)},
    {QT_TRANSLATE_NOOP("LocalePage", "German"),  QLocale(QLocale::German)},
    {QT_TRANSLATE_NOOP("LocalePage", "Hebrew"),  QLocale(QLocale::Hebrew)},
    {QT_TRANSLATE_NOOP("LocalePage", "French"),  QLocale(QLocale::French)},
    {QT_TRANSLATE_NOOP("LocalePage", "Bulgarian"),  QLocale(QLocale::Bulgarian)},
    {QT_TRANSLATE_NOOP("LocalePage", "Czech"),  QLocale(QLocale::Czech)},
    {QT_TRANSLATE_NOOP("LocalePage", "Catalan"),  QLocale(QLocale::Catalan)},
};
static const int LANGUAGE_COUNT = sizeof(LANGUAGES) / sizeof(LANGUAGES[0]);

static Factory factory;

OnyxKeyboardLanguageDialog::OnyxKeyboardLanguageDialog(QLocale language,
        QWidget *parent)
    : OnyxDialog(parent)
    , layout_(&content_widget_)
    , language_group_(&factory, this)
    , language_(language)
{
    createLayout();
    resize(200, 580);
}

OnyxKeyboardLanguageDialog::~OnyxKeyboardLanguageDialog()
{
    clearDatas(language_group_datas_);
}

void OnyxKeyboardLanguageDialog::createLayout()
{
    content_widget_.setBackgroundRole(QPalette::Button);

    updateTitle(QApplication::tr("Switch Language"));
    updateTitleIcon(QPixmap(":/images/locale_small.png"));
    layout_.setContentsMargins(SPACING, 0, SPACING, 0);
    createLanguageGroup();
    layout_.addWidget(&language_group_);

    connect(&language_group_,
            SIGNAL(itemActivated(CatalogView *, ContentView *, int)),
            this,
            SLOT(onItemActivated(CatalogView *, ContentView *, int)));
}

bool OnyxKeyboardLanguageDialog::enableGeorgian()
{
    return qgetenv("ENABLE_GEORGIAN_LANGUAGE").toInt() > 0;
}

void OnyxKeyboardLanguageDialog::createLanguageGroup()
{
    language_group_.setSubItemType(CheckBoxView::type());
    language_group_.setPreferItemSize(QSize(120, defaultItemHeight()));

    for (int i=0; i<LANGUAGE_COUNT; i++)
    {
        if (LANGUAGES[i].locale.name() == QLocale(QLocale::Georgian).name())
        {
            if (!enableGeorgian())
            {
                continue;
            }
        }
        ODataPtr dd(new OData);
        dd->insert(TAG_TITLE, qApp->translate(SCOPE, LANGUAGES[i].text));
        dd->insert(TAG_INDEX, i);
        if (LANGUAGES[i].locale.name() == language_.name())
        {
            dd->insert(TAG_CHECKED, true);
        }
        language_group_datas_.push_back(dd);
    }
    language_group_.setData(language_group_datas_);

    language_group_.setFixedGrid(LANGUAGE_COUNT, 1);
    language_group_.setSearchPolicy(CatalogView::AutoVerRecycle);
}

QLocale OnyxKeyboardLanguageDialog::getLocale(const OData *data)
{
    int index = data->value(TAG_INDEX).toInt();
    QLocale selected_locale = LANGUAGES[index].locale;
    return selected_locale;
}

void OnyxKeyboardLanguageDialog::onItemActivated(CatalogView *catalog,
        ContentView *item, int user_data)
{
    if (!item || !item->data())
    {
        return;
    }

    OData * item_data = item->data();
    language_ = getLocale(item_data);
    accept();
}

}

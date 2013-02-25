#include "onyx/ui/keyboard_data_factory.h"
#include "onyx/ui/english_keyboard_data.h"
#include "onyx/ui/russian_keyboard_data.h"
#include "onyx/ui/polish_keyboard_data.h"
#include "onyx/ui/swedish_keyboard_data.h"
#include "onyx/ui/georgian_keyboard_data.h"
#include "onyx/ui/german_keyboard_data.h"
#include "onyx/ui/hebrew_keyboard_data.h"
#include "onyx/ui/french_keyboard_data.h"
#include "onyx/ui/bulgarian_keyboard_data.h"
#include "onyx/ui/czech_keyboard_data.h"
#include "onyx/ui/hungarian_keyboard_data.h"
#include "onyx/ui/turkish_keyboard_data.h"
#include "onyx/ui/keyboard_data.h"

namespace ui
{

KeyboardDataFactory::KeyboardDataFactory()
{
    registerKeyboardData(QLocale::English, new EnglishKeyboardData());
    registerKeyboardData(QLocale::Russian, new RussianKeyboardData());
    registerKeyboardData(QLocale::Polish, new PolishKeyboardData());
    registerKeyboardData(QLocale::Swedish, new SwedishKeyboardData());
    registerKeyboardData(QLocale::Georgian, new GeorgianKeyboardData());
    registerKeyboardData(QLocale::German, new GermanKeyboardData());
    registerKeyboardData(QLocale::Hebrew, new HebrewKeyboardData());
    registerKeyboardData(QLocale::French, new FrenchKeyboardData());
    registerKeyboardData(QLocale::Bulgarian, new BulgarianKeyboardData());
    registerKeyboardData(QLocale::Czech, new CzechKeyboardData());
    registerKeyboardData(QLocale::Hungarian, new HungarianKeyboardData());
    registerKeyboardData(QLocale::Turkish, new TurkishKeyboardData());
}

KeyboardDataFactory::~KeyboardDataFactory()
{
    foreach (KeyboardData *kdata, data_map_)
    {
        delete kdata;
    }
    data_map_.clear();
}

bool KeyboardDataFactory::registerKeyboardData(const QLocale & locale,
        KeyboardData *data)
{
    data_map_.insert(locale.name(), data);
    return true;
}

KeyboardData * KeyboardDataFactory::getKeyboardData(const QLocale & locale)
{
    QString name = locale.name();
    KeyboardData * default_data = data_map_.value(
            QLocale(QLocale::English).name());
    return data_map_.value(name, default_data);
}

}   // namespace ui

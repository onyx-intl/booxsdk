/*
 * french_keyboard_data.cpp
 *
 *  Created on: 2012-2-9
 *      Author: weihuahuang
 */


#include "onyx/ui/french_keyboard_data.h"

namespace ui
{

static const QChar FRENCH_LEFT[] = {'a', 'z', 'e', 'q', 's', 'd', 'w', 'x', 'c' };
static const QChar FRENCH_MIDDLE[] = {'r', 't', 'y', 'f', 'g', 'h', 'v', 'b', 'n'};
static const QChar FRENCH_RIGHT[] = {'u', 'i', 'o',  'j', 'k', 'l', 'm', 'p', '.'};


FrenchKeyboardData::FrenchKeyboardData()
    : KeyboardData()
{
    initTopKeyCode();
    initLeftKeyCode();
    initMiddleKeyCode();
    initRightKeyCode();
    initBottomKeyCode();

    initTopKeyShiftCode();
    initLeftKeyShiftCode();
    initMiddleKeyShiftCode();
    initRightKeyShiftCode();
    initBottomKeyShiftCode();

    initLeftKeySymbolCode();
    initMiddleKeySymbolCode();
    initRightKeySymbolCode();
}

FrenchKeyboardData::~FrenchKeyboardData()
{
}

// for initialization
void FrenchKeyboardData::initTopKeyCode()
{
    const QChar chs[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_codes_.push_back(dd);
    }
}

void FrenchKeyboardData::initLeftKeyCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(FRENCH_LEFT[i])));
        left_codes_.push_back(dd);
    }
}

void FrenchKeyboardData::initMiddleKeyCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(FRENCH_MIDDLE[i])));
        middle_codes_.push_back(dd);
    }
}

void FrenchKeyboardData::initRightKeyCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(FRENCH_RIGHT[i])));
        right_codes_.push_back(dd);
    }
}

void FrenchKeyboardData::initBottomKeyCode()
{
    const QChar chs[] = {',', ';', QChar(0x00e9), QChar(0x00e8),
            QChar(0x00e7)};
    for (int i=0; i<5; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        bottom_codes_.push_back(dd);
    }

    bottom_codes_.push_back(ODataPtr(createSpaceData()));
    ODataPtr dd(createBackspaceData());
    bottom_codes_.push_back(dd);

    const QChar chs_next[] = {QChar(0x00e0), QChar(0x00f9)};
    for (int i=0; i<2; i++)
    {
        ODataPtr dd(createData(QString(chs_next[i])));
        bottom_codes_.push_back(dd);
    }

    bottom_codes_.push_back(ODataPtr(createEnterData()));
}

// for shift key
void FrenchKeyboardData::initTopKeyShiftCode()
{
    const QChar chs[] = { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_shift_codes_.push_back(dd);
    }
}

void FrenchKeyboardData::initLeftKeyShiftCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(FRENCH_LEFT[i].toUpper())));
        left_shift_codes_.push_back(dd);
    }
}

void FrenchKeyboardData::initMiddleKeyShiftCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(FRENCH_MIDDLE[i].toUpper())));
        middle_shift_codes_.push_back(dd);
    }
}

void FrenchKeyboardData::initRightKeyShiftCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(FRENCH_RIGHT[i].toUpper())));
        right_shift_codes_.push_back(dd);
    }
}

void FrenchKeyboardData::initBottomKeyShiftCode()
{
    const QChar chs[] = {',', ';', QChar(0x00e9), QChar(0x00e8),QChar(0x00e7)};

   for (int i=0; i<5; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        bottom_shift_codes_.push_back(dd);
    }

    bottom_shift_codes_.push_back(ODataPtr(createSpaceData()));
    ODataPtr dd(createBackspaceData());
    bottom_shift_codes_.push_back(dd);

    const QChar chs_next[] = {QChar(0x00e0), QChar(0x00f9)};
    for (int i=0; i<2; i++)
    {
        ODataPtr dd(createData(QString(chs_next[i])));
        bottom_shift_codes_.push_back(dd);
    }

    bottom_shift_codes_.push_back(ODataPtr(createEnterData()));
}

// for symbol key
void FrenchKeyboardData::initLeftKeySymbolCode()
{
    const QChar chs[] = {'[', ']', '\\',
                         QChar(0x00B5), '/', '`',
                         QChar(0x002D), QChar(0x00AF), QChar(0x00B7), };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        left_symbol_codes_.push_back(dd);
    }
}

void FrenchKeyboardData::initMiddleKeySymbolCode()
{
    const QChar chs[] = {QChar(0x0027), '"', '?',
                         ':', '+', '-',
                         '=', '{', '}', };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        middle_symbol_codes_.push_back(dd);
    }
}

void FrenchKeyboardData::initRightKeySymbolCode()
{
    const QChar chs[] = {'_',  '<', '>',
                         '~', '|', QChar(0x00A3),
                         QChar(0x00A5),QChar(0x00A7) };
    for (int i = 0; i < 8; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        right_symbol_codes_.push_back(dd);
    }
}

}


/*
 * czech_keyboard_data.cpp
 *
 *  Created on: 2012-2-13
 *      Author: weihuahuang
 */

#include "onyx/ui/czech_keyboard_data.h"

namespace ui
{

static const QChar CZECH_LEFT[] = { QChar(0x0071), QChar(0x0077), QChar(0x0065),
                                    QChar(0x0061), QChar(0x0073), QChar(0x0064),
                                    QChar(0x007A), QChar(0x0078), QChar(0x0063)};

static const QChar CZECH_MIDDLE[] = { QChar(0x0072), QChar(0x0074), QChar(0x0079),
                                      QChar(0x0066), QChar(0x0067), QChar(0x0068),
                                      QChar(0x0076), QChar(0x0062), QChar(0x06E)};

static const QChar CZECH_RIGHT[] = { QChar(0x0075), QChar(0x0069), QChar(0x006F),
                                     QChar(0x006A), QChar(0x006B), QChar(0x006C),
                                     QChar(0x006D), QChar(0x0070), QChar(0x00FA)};

CzechKeyboardData::CzechKeyboardData()
    :KeyboardData()
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

    initTopKeySymbolCode();
    initLeftKeySymbolCode();
    initMiddleKeySymbolCode();
    initRightKeySymbolCode();
};


CzechKeyboardData::~CzechKeyboardData()
{
}


void CzechKeyboardData::initTopKeyCode()
{
   const QChar chs[] = {  QChar(0x011B), QChar(0x0161), QChar(0x010D),
                          QChar(0x0159), QChar(0x017E), QChar(0x00FD),
                          QChar(0x00E1), QChar(0x00ED), QChar(0x00E9), QChar(0x016F)};
   for(int i = 0; i < 10; i++)
   {
       ODataPtr dd(createData(QString(chs[i])));
       top_codes_.push_back(dd);
   }
}


void CzechKeyboardData::initLeftKeyCode()
{
    for(int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(CZECH_LEFT[i])));
        left_codes_.push_back(dd);
    }
}

void CzechKeyboardData::initMiddleKeyCode()
{
    for(int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(CZECH_MIDDLE[i])));
        middle_codes_.push_back(dd);
    }
}

void CzechKeyboardData::initRightKeyCode()
{
    for(int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(CZECH_RIGHT[i])));
        right_codes_.push_back(dd);
    }
}

void CzechKeyboardData::initBottomKeyCode()
{
    const QChar chs[] = {',', ';', '.', QChar(0x0027), '"'};
    for (int i=0; i<5; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        bottom_codes_.push_back(dd);
    }

    bottom_codes_.push_back(ODataPtr(createSpaceData()));
    ODataPtr dd(createBackspaceData());
    bottom_codes_.push_back(dd);

    const QChar chs_next[] = {'+', '-'};
    for (int i=0; i<2; i++)
    {
        ODataPtr dd(createData(QString(chs_next[i])));
        bottom_codes_.push_back(dd);
    }

    bottom_codes_.push_back(ODataPtr(createEnterData()));
}

//  Shift key
void CzechKeyboardData::initTopKeyShiftCode()
{
    const QChar chs[] = {  QChar(0x011B), QChar(0x0161), QChar(0x010D),
                           QChar(0x0159), QChar(0x017E), QChar(0x00FD),
                           QChar(0x00E1), QChar(0x00ED), QChar(0x00E9), QChar(0x016F)};
    for(int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i].toUpper())));
        top_shift_codes_.push_back(dd);
    }
}

void CzechKeyboardData::initLeftKeyShiftCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(CZECH_LEFT[i].toUpper())));
        left_shift_codes_.push_back(dd);
    }
}

void CzechKeyboardData::initMiddleKeyShiftCode()
{
    for(int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(CZECH_MIDDLE[i].toUpper())));
        middle_shift_codes_.push_back(dd);
    }
}

void CzechKeyboardData::initRightKeyShiftCode()
{
    for(int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(CZECH_RIGHT[i].toUpper())));
        right_shift_codes_.push_back(dd);
    }
}

void CzechKeyboardData::initBottomKeyShiftCode()
{
    const QChar chs[] = {',', ';', '.', QChar(0x0027), '"'};
    for (int i=0; i<5; i++)
    {
        ODataPtr dd(createData(QString(chs[i].toUpper())));
        bottom_shift_codes_.push_back(dd);
    }

    bottom_shift_codes_.push_back(ODataPtr(createSpaceData()));
    ODataPtr dd(createBackspaceData());
    bottom_shift_codes_.push_back(dd);

    const QChar chs_next[] = {'+', '-'};
    for (int i=0; i<2; i++)
    {
        ODataPtr dd(createData(QString(chs_next[i].toUpper())));
        bottom_shift_codes_.push_back(dd);
    }

    bottom_shift_codes_.push_back(ODataPtr(createEnterData()));
}

// Symbol key

void CzechKeyboardData::initTopKeySymbolCode()
{
    top_symbol_codes_.clear();
    const QChar chs[] = {'1','2','3','4','5','6','7','8','9','0'};

    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_symbol_codes_.push_back(dd);
    }
}

void CzechKeyboardData::initLeftKeySymbolCode()
{
    const QChar chs[] = {'[', ']', '\\',
                         QChar(0x00B5), '/', '`',
                         QChar(0x002D), QChar(0x00AF), '%' };

    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        left_symbol_codes_.push_back(dd);
    }
}

void CzechKeyboardData::initMiddleKeySymbolCode()
{
    const QChar chs[] = {'(', ')', '?',
                         ':', QChar(0x00A8), '!',
                         QChar(0x02C7), QChar(0x00B4), QChar(0x00B0)};
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        middle_symbol_codes_.push_back(dd);
    }
}

void CzechKeyboardData::initRightKeySymbolCode()
{
    const QChar chs[] = {'=', '{', '}',
                         '_',  '<', '>',
                         '~', '|', QChar(0x00A7)};
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        right_symbol_codes_.push_back(dd);
    }
}

}

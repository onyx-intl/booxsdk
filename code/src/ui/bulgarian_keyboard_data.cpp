/*
 * bulgarian_keyboard_data.cpp
 *
 *  Created on: 2012-2-10
 *      Author: weihuahuang
 */


#include "onyx/ui/bulgarian_keyboard_data.h"

namespace ui
{

static const QChar BULGARIAN_LEFT[] = { QChar(0x0443), QChar(0x0435), QChar(0x0438),
                                        QChar(0x044C), QChar(0x044F), QChar(0x0430),
                                        QChar(0x044E), QChar(0x0439), QChar(0x044A)};

static const QChar BULGARIAN_MIDDLE[] = { QChar(0x0448), QChar(0x0449), QChar(0x043A),
                                          QChar(0x043E), QChar(0x0436), QChar(0x0433),
                                          QChar(0x044D), QChar(0x0444), QChar(0x0445)};

static const QChar BULGARIAN_RIGHT[] = { QChar(0x0441), QChar(0x0434), QChar(0x0437),
                                         QChar(0x0442), QChar(0x043D), QChar(0x0432),
                                         QChar(0x043F), QChar(0x0440), QChar(0x043B)};

BulgarianKeyboardData::BulgarianKeyboardData()
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

    initLeftKeySymbolCode();
    initMiddleKeySymbolCode();
    initRightKeySymbolCode();

};


BulgarianKeyboardData::~BulgarianKeyboardData()
{
}


void BulgarianKeyboardData::initTopKeyCode()
{
   const QChar chs[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
   for(int i = 0; i < 10; i++)
   {
       ODataPtr dd(createData(QString(chs[i])));
       top_codes_.push_back(dd);
   }
}


void BulgarianKeyboardData::initLeftKeyCode()
{
    for(int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(BULGARIAN_LEFT[i])));
        left_codes_.push_back(dd);
    }
}

void BulgarianKeyboardData::initMiddleKeyCode()
{
    for(int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(BULGARIAN_MIDDLE[i])));
        middle_codes_.push_back(dd);
    }
}

void BulgarianKeyboardData::initRightKeyCode()
{
    for(int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(BULGARIAN_RIGHT[i])));
        right_codes_.push_back(dd);
    }
}

void BulgarianKeyboardData::initBottomKeyCode()
{
    const QChar chs[] = {',', ';', '.', QChar(0x0446), QChar(0x043c)};
    for (int i=0; i<5; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        bottom_codes_.push_back(dd);
    }

    bottom_codes_.push_back(ODataPtr(createSpaceData()));
    ODataPtr dd(createBackspaceData());
    bottom_codes_.push_back(dd);

    const QChar chs_next[] = {QChar(0x0447), QChar(0x0431)};
    for (int i=0; i<2; i++)
    {
        ODataPtr dd(createData(QString(chs_next[i])));
        bottom_codes_.push_back(dd);
    }

    bottom_codes_.push_back(ODataPtr(createEnterData()));
}

//  Shift key
void BulgarianKeyboardData::initTopKeyShiftCode()
{
    const QChar chs[] = { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_shift_codes_.push_back(dd);
    }
}


void BulgarianKeyboardData::initLeftKeyShiftCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(BULGARIAN_LEFT[i].toUpper())));
        left_shift_codes_.push_back(dd);
    }
}

void BulgarianKeyboardData::initMiddleKeyShiftCode()
{
    for(int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(BULGARIAN_MIDDLE[i].toUpper())));
        middle_shift_codes_.push_back(dd);
    }
}

void BulgarianKeyboardData::initRightKeyShiftCode()
{
    for(int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(BULGARIAN_RIGHT[i].toUpper())));
        right_shift_codes_.push_back(dd);
    }
}

void BulgarianKeyboardData::initBottomKeyShiftCode()
{
    const QChar chs[] = {',', ';', '.', QChar(0x0446), QChar(0x043c)};
    for (int i=0; i<5; i++)
    {
        ODataPtr dd(createData(QString(chs[i].toUpper())));
        bottom_shift_codes_.push_back(dd);
    }

    bottom_shift_codes_.push_back(ODataPtr(createSpaceData()));
    ODataPtr dd(createBackspaceData());
    bottom_shift_codes_.push_back(dd);

    const QChar chs_next[] = {QChar(0x0447), QChar(0x0431)};
    for (int i=0; i<2; i++)
    {
        ODataPtr dd(createData(QString(chs_next[i].toUpper())));
        bottom_shift_codes_.push_back(dd);
    }

    bottom_shift_codes_.push_back(ODataPtr(createEnterData()));
}

// Symbol key
void BulgarianKeyboardData::initLeftKeySymbolCode()
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

void BulgarianKeyboardData::initMiddleKeySymbolCode()
{
    const QChar chs[] = {QChar(0x0027), '"', '?',
                         ':', '+', '-',
                         QChar(0x2116),QChar(0x0056),QChar(0x044B)};
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        middle_symbol_codes_.push_back(dd);
    }
}

void BulgarianKeyboardData::initRightKeySymbolCode()
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

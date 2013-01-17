#include "onyx/base/base.h"
#include "onyx/sys/sys.h"
#include "onyx/ui/english_keyboard_data.h"
#include "onyx/ui/onyx_keyboard_utils.h"

namespace ui
{

EnglishKeyboardData::EnglishKeyboardData()
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

EnglishKeyboardData::~EnglishKeyboardData()
{
}

void EnglishKeyboardData::initTopKeyCode()
{
    const QChar chs[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(chs[i]));
        top_codes_.push_back(dd);
    }
}

void EnglishKeyboardData::initLeftKeyCode()
{
    const QChar chs[] = { 'q', 'w', 'e', 'a', 's', 'd', 'z', 'x', 'c' };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        left_codes_.push_back(dd);
    }
}

void EnglishKeyboardData::initMiddleKeyCode()
{
    const QChar chs[] = {'r', 't', 'y', 'f', 'g', 'h', 'v', 'b', 'n'};
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        middle_codes_.push_back(dd);
    }
}

void EnglishKeyboardData::initRightKeyCode()
{
    const QChar chs[] = {'u', 'i', 'o', 'j', 'k', 'l', 'm', 'p', '.'};
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        right_codes_.push_back(dd);
    }
}

void EnglishKeyboardData::initBottomKeyCode()
{
    const QChar chs[] = {'+', '-', '_', '"', ','};
    for (int i=0; i<5; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        bottom_codes_.push_back(dd);
    }

    bottom_codes_.push_back(ODataPtr(createSpaceData()));
    bottom_codes_.push_back(ODataPtr(createBackspaceData()));

    const QChar chs_next[] = {'\'', ';'};
    for (int i=0; i<2; i++)
    {
        ODataPtr dd(createData(QString(chs_next[i])));
        bottom_codes_.push_back(dd);
    }

    bottom_codes_.push_back(ODataPtr(createEnterData()));
}

void EnglishKeyboardData::initTopKeyShiftCode()
{
    const QChar chs[] = { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_shift_codes_.push_back(dd);
    }
}

void EnglishKeyboardData::initLeftKeyShiftCode()
{
    const QChar chs[] = { 'Q', 'W', 'E', 'A', 'S', 'D', 'Z', 'X', 'C' };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        left_shift_codes_.push_back(dd);
    }
}

void EnglishKeyboardData::initMiddleKeyShiftCode()
{
    const QChar chs[] = {'R', 'T', 'Y', 'F', 'G', 'H', 'V', 'B', 'N'};
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        middle_shift_codes_.push_back(dd);
    }
}

void EnglishKeyboardData::initRightKeyShiftCode()
{
    const QChar chs[] = { 'U', 'I', 'O', 'J', 'K', 'L', 'M', 'P', ',' };
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        right_shift_codes_.push_back(dd);
    }
}

void EnglishKeyboardData::initBottomKeyShiftCode()
{
    const QChar chs[] = {'\\', '/', '[', ']', '='};
    for (int i=0; i<5; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        bottom_shift_codes_.push_back(dd);
    }

    bottom_shift_codes_.push_back(ODataPtr(createSpaceData()));
    bottom_shift_codes_.push_back(ODataPtr(createBackspaceData()));

    const QChar chs_next[] = {'?', ':'};
    for (int i=0; i<2; i++)
    {
        ODataPtr dd(createData(QString(chs_next[i])));
        bottom_shift_codes_.push_back(dd);
    }

    bottom_shift_codes_.push_back(ODataPtr(createEnterData()));
}

void EnglishKeyboardData::initLeftKeySymbolCode()
{
    const QString chs[] = {"...", "{", "}",
                           "www.", ".com", QString(QChar(0x00A2)),
                           QString(QChar(0x00B9)), QString(QChar(0x00B2)),
                           QString(QChar(0x00B3)), };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(chs[i]));
        if (chs[i].size() > 2)
        {
            dd->insert(TAG_FONT_SIZE, 16);
        }
        left_symbol_codes_.push_back(dd);
    }
}

void EnglishKeyboardData::initMiddleKeySymbolCode()
{
    const QChar chs[] = {'~', '<', '>',
                         '|', '`', QChar(0x00A3),
                         QChar(0x00A5), QChar(0x00A7), QChar(0x00A9), };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(chs[i]));
        middle_symbol_codes_.push_back(dd);
    }
}

void EnglishKeyboardData::initRightKeySymbolCode()
{
    const QChar chs[] = {QChar(0x00B1), QChar(0x00B5), QChar(0x00AC),
                         QChar(0x002D), QChar(0x00AF), QChar(0x00B7),
                         QChar(0x00AB), QChar(0x00BB), QChar(0x00AE), };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(chs[i]));
        right_symbol_codes_.push_back(dd);
    }
}

}

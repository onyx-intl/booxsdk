#include "onyx/base/base.h"
#include "onyx/sys/sys.h"
#include "onyx/ui/german_keyboard_data.h"
#include "onyx/ui/onyx_keyboard_utils.h"

namespace ui
{

static const QChar German_Character[4][2]={
  {QChar(0x00E4), QChar(0x00C4)},   //German character 'ä(Ä)'
  {QChar(0x00F6), QChar(0x00D6)},   //German character 'ö(Ö)'
  {QChar(0x00FC), QChar(0x00DC)},   //German character 'ü(Ü)'
  {QChar(0x00DF), QChar(0x00DF)},   //German character 'ß'
};

GermanKeyboardData::GermanKeyboardData()
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

GermanKeyboardData::~GermanKeyboardData()
{
}

void GermanKeyboardData::initTopKeyCode()
{
    const QChar chs[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_codes_.push_back(dd);
    }
}

void GermanKeyboardData::initLeftKeyCode()
{
    const QChar chs[] = { 'q', 'w', 'e', 'a', 's', 'd', 'y', 'x', 'c' };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        left_codes_.push_back(dd);
    }
}

void GermanKeyboardData::initMiddleKeyCode()
{
    const QChar chs[] = {'r', 't', 'z', 'f', 'g', 'h', 'v', 'b', 'n'};
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        middle_codes_.push_back(dd);
    }
}

void GermanKeyboardData::initRightKeyCode()
{
    const QChar chs[] = {'u', 'i', 'o', 'j', 'k', 'l', 'm', 'p', German_Character[0][0] };
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        right_codes_.push_back(dd);
    }
}

void GermanKeyboardData::initBottomKeyCode()
{
    const QChar chs[] = {German_Character[1][0], German_Character[2][0],
            German_Character[3][0], '.', '-', ';', '"'};
    for (int i=0; i<7; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        bottom_codes_.push_back(dd);
    }

    bottom_codes_.push_back(ODataPtr(createSpaceData()));
    bottom_codes_.push_back(ODataPtr(createBackspaceData()));
    bottom_codes_.push_back(ODataPtr(createEnterData()));
}

void GermanKeyboardData::initTopKeyShiftCode()
{
    const QChar chs[] = { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_shift_codes_.push_back(dd);
    }
}

void GermanKeyboardData::initLeftKeyShiftCode()
{
    const QChar chs[] = { 'Q', 'W', 'E', 'A', 'S', 'D', 'Y', 'X', 'C' };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        left_shift_codes_.push_back(dd);
    }
}

void GermanKeyboardData::initMiddleKeyShiftCode()
{
    const QChar chs[] = {'R', 'T', 'Z', 'F', 'G', 'H', 'V', 'B', 'N'};
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        middle_shift_codes_.push_back(dd);
    }
}

void GermanKeyboardData::initRightKeyShiftCode()
{
    const QChar chs[] = { 'U', 'I', 'O', 'J', 'K', 'L', 'M', 'P', German_Character[0][1]};//',' };
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        right_shift_codes_.push_back(dd);
    }
}

void GermanKeyboardData::initBottomKeyShiftCode()
{
    const QChar chs[] = {German_Character[1][1], German_Character[2][1],
            German_Character[3][1], ',', '_', ':', '\''};
    for (int i=0; i<7; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        bottom_shift_codes_.push_back(dd);
    }

    bottom_shift_codes_.push_back(ODataPtr(createSpaceData()));
    bottom_shift_codes_.push_back(ODataPtr(createBackspaceData()));
    bottom_shift_codes_.push_back(ODataPtr(createEnterData()));
}

void GermanKeyboardData::initLeftKeySymbolCode()
{
    const QString chs[] = {"...", "{", "}", "?", "[", "]", "=", "/", "\\"};
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

void GermanKeyboardData::initMiddleKeySymbolCode()
{
    const QChar chs[] = {'~', '<', '>',
                         '|', '`', '+',
                         QChar(0x00A5), QChar(0x00A7), QChar(0x00A9), };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(chs[i]));
        middle_symbol_codes_.push_back(dd);
    }
}

void GermanKeyboardData::initRightKeySymbolCode()
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

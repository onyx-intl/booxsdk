#include "onyx/base/base.h"
#include "onyx/sys/sys.h"
#include "onyx/ui/polish_keyboard_data.h"
#include "onyx/ui/onyx_keyboard_utils.h"

namespace ui
{

static const QChar Polish_Character[9][2]={
  {QChar(0x0104), QChar(0x0105)},
  {QChar(0x0106), QChar(0x0107)},
  {QChar(0x0118), QChar(0x0119)},
  {QChar(0x0143), QChar(0x0144)},
  {QChar(0x015A), QChar(0x015B)},
  {QChar(0x0179), QChar(0x017A)},
  {QChar(0x017B), QChar(0x017C)},
  {QChar(0x0141), QChar(0x0142)},
  {QChar(0x00D3), QChar(0x00F3)}
};

PolishKeyboardData::PolishKeyboardData()
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

PolishKeyboardData::~PolishKeyboardData()
{
}

void PolishKeyboardData::initTopKeyCode()
{
    const QChar chs[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_codes_.push_back(dd);
    }
}

void PolishKeyboardData::initLeftKeyCode()
{
    const QChar chs[] = { 'q', 'w', 'e', 'a', 's', 'd', 'z', 'x', 'c' };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        left_codes_.push_back(dd);
    }
}

void PolishKeyboardData::initMiddleKeyCode()
{
    const QChar chs[] = {'r', 't', 'y', 'f', 'g', 'h', 'v', 'b', 'n'};
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        middle_codes_.push_back(dd);
    }
}

void PolishKeyboardData::initRightKeyCode()
{
    const QChar chs[] = {'u', 'i', 'o', 'j', 'k', 'l', 'm', 'p', Polish_Character[0][1]};//};
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        right_codes_.push_back(dd);
    }
}

void PolishKeyboardData::initBottomKeyCode()
{
    const QChar chs[] = {Polish_Character[1][1], Polish_Character[2][1],
            Polish_Character[3][1], Polish_Character[4][1],
            Polish_Character[5][1]};
    for (int i=0; i<5; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        bottom_codes_.push_back(dd);
    }

    bottom_codes_.push_back(ODataPtr(createSpaceData()));
    bottom_codes_.push_back(ODataPtr(createBackspaceData()));

    const QChar chs_next[] = {Polish_Character[7][1], Polish_Character[8][1], Polish_Character[6][1]}; //{};
    for (int i=0; i<3; i++)
    {
        ODataPtr dd(createData(QString(chs_next[i])));
        bottom_codes_.push_back(dd);
    }

    //dd = createEnterData();
    //bottom_codes_.push_back(dd);
}

void PolishKeyboardData::initTopKeyShiftCode()
{
    const QChar chs[] = { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_shift_codes_.push_back(dd);
    }
}

void PolishKeyboardData::initLeftKeyShiftCode()
{
    const QChar chs[] = { 'Q', 'W', 'E', 'A', 'S', 'D', 'Z', 'X', 'C' };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        left_shift_codes_.push_back(dd);
    }
}

void PolishKeyboardData::initMiddleKeyShiftCode()
{
    const QChar chs[] = {'R', 'T', 'Y', 'F', 'G', 'H', 'V', 'B', 'N'};
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        middle_shift_codes_.push_back(dd);
    }
}

void PolishKeyboardData::initRightKeyShiftCode()
{
    const QChar chs[] = { 'U', 'I', 'O', 'J', 'K', 'L', 'M', 'P', Polish_Character[0][1]};//',' };
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        right_shift_codes_.push_back(dd);
    }
}

void PolishKeyboardData::initBottomKeyShiftCode()
{
    const QChar chs[] = {Polish_Character[1][0], Polish_Character[2][0],
            Polish_Character[3][0], Polish_Character[4][0],
            Polish_Character[5][0]};
    for (int i=0; i<5; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        bottom_shift_codes_.push_back(dd);
    }

    bottom_shift_codes_.push_back(ODataPtr(createSpaceData()));
    bottom_shift_codes_.push_back(ODataPtr(createBackspaceData()));

    const QChar chs_next[] = {Polish_Character[7][0], Polish_Character[8][0], Polish_Character[6][0]};//{'?', ':'};
    for (int i=0; i<3; i++)
    {
        ODataPtr dd(createData(QString(chs_next[i])));
        bottom_shift_codes_.push_back(dd);
    }

    
    //dd = createEnterData();
    //bottom_shift_codes_.push_back(dd);
}

void PolishKeyboardData::initLeftKeySymbolCode()
{
    const QString chs[] = {"~", "_", "+",
                           "`", "-", "=",
                           "www.", ".com", "..."};
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

void PolishKeyboardData::initMiddleKeySymbolCode()
{
    const QChar chs[] = {'{', '}', '|',
                         '[', ']', '\\',
                         '<', '>', ',', };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        middle_symbol_codes_.push_back(dd);
    }
}

void PolishKeyboardData::initRightKeySymbolCode()
{
    const QChar chs[] = {':', '"', '?',
                         ';', '\'', '/',
                         '.', QChar(0x002D), QChar(0x00BB)};
    for (int i = 0; i < 8; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        right_symbol_codes_.push_back(dd);
    }
    right_symbol_codes_.push_back(ODataPtr(createEnterData()));
}

}

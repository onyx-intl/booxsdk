#include "onyx/base/base.h"
#include "onyx/sys/sys.h"
#include "onyx/ui/hungarian_keyboard_data.h"
#include "onyx/ui/onyx_keyboard_utils.h"

namespace ui
{

static const QChar Hungarian_Character[9][2]={
  {QChar(0x00C1), QChar(0x00E1)},
  {QChar(0x00C9), QChar(0x00E9)},
  {QChar(0x00CD), QChar(0x00ED)},
  {QChar(0x00D3), QChar(0x00F3)},
  {QChar(0x00D6), QChar(0x00F6)},
  {QChar(0x0150), QChar(0x0151)},
  {QChar(0x00DA), QChar(0x00FA)},
  {QChar(0x00DC), QChar(0x00FC)},
  {QChar(0x0170), QChar(0x0171)}
};

HungarianKeyboardData::HungarianKeyboardData()
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

HungarianKeyboardData::~HungarianKeyboardData()
{
}

void HungarianKeyboardData::initTopKeyCode()
{
    const QChar chs[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_codes_.push_back(dd);
    }
}

void HungarianKeyboardData::initLeftKeyCode()
{
    const QChar chs[] = { 'q', 'w', 'e', 'a', 's', 'd', 'z', 'x', 'c' };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        left_codes_.push_back(dd);
    }
}

void HungarianKeyboardData::initMiddleKeyCode()
{
    const QChar chs[] = {'r', 't', 'y', 'f', 'g', 'h', 'v', 'b', 'n'};
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        middle_codes_.push_back(dd);
    }
}

void HungarianKeyboardData::initRightKeyCode()
{
    const QChar chs[] = {'u', 'i', 'o', 'j', 'k', 'l', 'm', 'p', Hungarian_Character[0][1]};//};
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        right_codes_.push_back(dd);
    }
}

void HungarianKeyboardData::initBottomKeyCode()
{
    const QChar chs[] = {Hungarian_Character[1][1], Hungarian_Character[2][1],
            Hungarian_Character[3][1], Hungarian_Character[4][1],
            Hungarian_Character[5][1]};
    for (int i=0; i<5; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        bottom_codes_.push_back(dd);
    }

    bottom_codes_.push_back(ODataPtr(createSpaceData()));
    bottom_codes_.push_back(ODataPtr(createBackspaceData()));

    const QChar chs_next[] = {Hungarian_Character[7][1], Hungarian_Character[8][1], Hungarian_Character[6][1]}; //{};
    for (int i=0; i<3; i++)
    {
        ODataPtr dd(createData(QString(chs_next[i])));
        bottom_codes_.push_back(dd);
    }

    //dd = createEnterData();
    //bottom_codes_.push_back(dd);
}

void HungarianKeyboardData::initTopKeyShiftCode()
{
    const QChar chs[] = { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_shift_codes_.push_back(dd);
    }
}

void HungarianKeyboardData::initLeftKeyShiftCode()
{
    const QChar chs[] = { 'Q', 'W', 'E', 'A', 'S', 'D', 'Z', 'X', 'C' };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        left_shift_codes_.push_back(dd);
    }
}

void HungarianKeyboardData::initMiddleKeyShiftCode()
{
    const QChar chs[] = {'R', 'T', 'Y', 'F', 'G', 'H', 'V', 'B', 'N'};
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        middle_shift_codes_.push_back(dd);
    }
}

void HungarianKeyboardData::initRightKeyShiftCode()
{
    const QChar chs[] = { 'U', 'I', 'O', 'J', 'K', 'L', 'M', 'P', Hungarian_Character[0][0]};//',' };
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        right_shift_codes_.push_back(dd);
    }
}

void HungarianKeyboardData::initBottomKeyShiftCode()
{
    const QChar chs[] = {Hungarian_Character[1][0], Hungarian_Character[2][0],
            Hungarian_Character[3][0], Hungarian_Character[4][0],
            Hungarian_Character[5][0]};
    for (int i=0; i<5; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        bottom_shift_codes_.push_back(dd);
    }

    bottom_shift_codes_.push_back(ODataPtr(createSpaceData()));
    bottom_shift_codes_.push_back(ODataPtr(createBackspaceData()));

    const QChar chs_next[] = {Hungarian_Character[7][0], Hungarian_Character[8][0], Hungarian_Character[6][0]};//{'?', ':'};
    for (int i=0; i<3; i++)
    {
        ODataPtr dd(createData(QString(chs_next[i])));
        bottom_shift_codes_.push_back(dd);
    }


    //dd = createEnterData();
    //bottom_shift_codes_.push_back(dd);
}

void HungarianKeyboardData::initLeftKeySymbolCode()
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

void HungarianKeyboardData::initMiddleKeySymbolCode()
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

void HungarianKeyboardData::initRightKeySymbolCode()
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

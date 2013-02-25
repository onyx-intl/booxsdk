#include "onyx/base/base.h"
#include "onyx/sys/sys.h"
#include "onyx/ui/turkish_keyboard_data.h"
#include "onyx/ui/onyx_keyboard_utils.h"

namespace ui
{

static const QChar Turkish_Character[6][2]={
  {QChar(0x00C7), QChar(0x00E7)}, /*Ç*/
  {QChar(0x011E), QChar(0x011F)}, /*Ğ*/
  {QChar(0x0130), QChar(0x0131)}, /*İ*/
  {QChar(0x00D6), QChar(0x00F6)}, /*Ö*/
  {QChar(0x015E), QChar(0x015F)}, /*Ş*/
  {QChar(0x00DC), QChar(0x00FC)} /*Ü*/
};

TurkishKeyboardData::TurkishKeyboardData()
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

TurkishKeyboardData::~TurkishKeyboardData()
{
}

void TurkishKeyboardData::initTopKeyCode()
{
    const QChar chs[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_codes_.push_back(dd);
    }
}

void TurkishKeyboardData::initLeftKeyCode()
{
    const QChar chs[] = { 'q', 'w', 'e', 'a', 's', 'd', 'z', 'x', 'c' };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        left_codes_.push_back(dd);
    }
}

void TurkishKeyboardData::initMiddleKeyCode()
{
    const QChar chs[] = {'r', 't', 'y', 'f', 'g', 'h', 'v', 'b', 'n'};
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        middle_codes_.push_back(dd);
    }
}

void TurkishKeyboardData::initRightKeyCode()
{
    const QChar chs[] = {'u', 'i', 'o', 'j', 'k', 'l', 'm', 'p', Turkish_Character[0][1]};//};
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        right_codes_.push_back(dd);
    }
}

void TurkishKeyboardData::initBottomKeyCode()
{
    const QChar chs[] = {Turkish_Character[1][1], Turkish_Character[2][1], Turkish_Character[3][1], Turkish_Character[4][1], Turkish_Character[5][1], ' '};//{'+', '-', '_', '"', ',', };
    for (int i=0; i<6; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        bottom_codes_.push_back(dd);
    }

    bottom_codes_.push_back(ODataPtr(createBackspaceData()));

    const QChar chs_next[] = {' ', ' ', ' '}; //{};
    for (int i=0; i<3; i++)
    {
        ODataPtr dd(createData(QString(chs_next[i])));
        bottom_codes_.push_back(dd);
    }

    //dd = createEnterData();
    //bottom_codes_.push_back(dd);
}

void TurkishKeyboardData::initTopKeyShiftCode()
{
    const QChar chs[] = { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_shift_codes_.push_back(dd);
    }
}

void TurkishKeyboardData::initLeftKeyShiftCode()
{
    const QChar chs[] = { 'Q', 'W', 'E', 'A', 'S', 'D', 'Z', 'X', 'C' };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        left_shift_codes_.push_back(dd);
    }
}

void TurkishKeyboardData::initMiddleKeyShiftCode()
{
    const QChar chs[] = {'R', 'T', 'Y', 'F', 'G', 'H', 'V', 'B', 'N'};
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        middle_shift_codes_.push_back(dd);
    }
}

void TurkishKeyboardData::initRightKeyShiftCode()
{
    const QChar chs[] = { 'U', 'I', 'O', 'J', 'K', 'L', 'M', 'P', Turkish_Character[0][1]};//',' };
    for (int i=0; i<9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        right_shift_codes_.push_back(dd);
    }
}

void TurkishKeyboardData::initBottomKeyShiftCode()
{
    const QChar chs[] = {Turkish_Character[1][0], Turkish_Character[2][0], Turkish_Character[3][0], Turkish_Character[4][0], Turkish_Character[5][0], ' '};//{'\\', '/', '[', ']', '=', };
    for (int i=0; i<6; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        bottom_shift_codes_.push_back(dd);
    }

    bottom_shift_codes_.push_back(ODataPtr(createBackspaceData()));

    const QChar chs_next[] = {' ', ' ', ' '};//{'?', ':'};
    for (int i=0; i<3; i++)
    {
        ODataPtr dd(createData(QString(chs_next[i])));
        bottom_shift_codes_.push_back(dd);
    }

    
    //dd = createEnterData();
    //bottom_shift_codes_.push_back(dd);
}

void TurkishKeyboardData::initLeftKeySymbolCode()
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

void TurkishKeyboardData::initMiddleKeySymbolCode()
{
    const QChar chs[] = {'~', '<', '>',
                         '|', '`', QChar(0x20AC),
                         '.', '+', '-', };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        middle_symbol_codes_.push_back(dd);
    }
}

void TurkishKeyboardData::initRightKeySymbolCode()
{
    const QChar chs[] = {'_', '"', ',',
                         QChar(0x00AD), '\'', ';',
                         QChar(0x00AB), QChar(0x00BB)};
    for (int i = 0; i < 8; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        right_symbol_codes_.push_back(dd);
    }
    right_symbol_codes_.push_back(ODataPtr(createEnterData()));
}

}


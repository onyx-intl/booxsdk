#include "onyx/ui/hebrew_keyboard_data.h"

namespace ui
{

static const QChar Hebrew_LEFT[] = {
        QChar(0x05E7), QChar(0x05E8), QChar(0x05D0),
        QChar(0x05D3), QChar(0x05D2), QChar(0x05DB),
        QChar(0x05D6), QChar(0x05E1), QChar(0x05D1),};

static const QChar Hebrew_MIDDLE[] = {
        QChar(0x05D8), QChar(0x05D5), QChar(0x05DF),
        QChar(0x05E2), QChar(0x05D9), QChar(0x05D7),
        QChar(0x05D4), QChar(0x05E0), QChar(0x05DE), };

static const QChar Hebrew_RIGHT[] = {
        QChar(0x05DD), QChar(0x05E4), QChar(0x05E9),
        QChar(0x05DC), QChar(0x05DA), QChar(0x05E3),
        QChar(0x05E6), QChar(0x05EA), QChar(0x05E5), };

HebrewKeyboardData::HebrewKeyboardData()
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

HebrewKeyboardData::~HebrewKeyboardData()
{
}

// for initialization
void HebrewKeyboardData::initTopKeyCode()
{
    const QChar chs[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_codes_.push_back(dd);
    }
}

void HebrewKeyboardData::initLeftKeyCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(Hebrew_LEFT[i])));
        left_codes_.push_back(dd);
    }
}

void HebrewKeyboardData::initMiddleKeyCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(Hebrew_MIDDLE[i])));
        middle_codes_.push_back(dd);
    }
}

void HebrewKeyboardData::initRightKeyCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(Hebrew_RIGHT[i])));
        right_codes_.push_back(dd);
    }
}

void HebrewKeyboardData::initBottomKeyCode()
{
    const QChar chs[] = {'+', '-', '.', '"', ','};
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

// for shift key
void HebrewKeyboardData::initTopKeyShiftCode()
{
    const QChar chs[] = { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_shift_codes_.push_back(dd);
    }
}

void HebrewKeyboardData::initLeftKeyShiftCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(Hebrew_LEFT[i])));
        left_shift_codes_.push_back(dd);
    }
}

void HebrewKeyboardData::initMiddleKeyShiftCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(Hebrew_MIDDLE[i])));
        middle_shift_codes_.push_back(dd);
    }
}

void HebrewKeyboardData::initRightKeyShiftCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(Hebrew_RIGHT[i])));
        right_shift_codes_.push_back(dd);
    }
}

void HebrewKeyboardData::initBottomKeyShiftCode()
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

void HebrewKeyboardData::initLeftKeySymbolCode()
{
    const QString chs[] = {"...", "{", "}",
                           "www.", ",", "_",
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

void HebrewKeyboardData::initMiddleKeySymbolCode()
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

void HebrewKeyboardData::initRightKeySymbolCode()
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

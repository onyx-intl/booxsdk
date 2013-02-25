#include "onyx/ui/georgian_keyboard_data.h"

namespace ui
{

static const QChar Georgian_LEFT[] = {
        QChar(0x10E5), QChar(0x10EC), QChar(0x10D4),
        QChar(0x10D0), QChar(0x10E1), QChar(0x10D3),
        QChar(0x10D6), QChar(0x10EE), QChar(0x10EA), };

static const QChar Georgian_MIDDLE[] = {
        QChar(0x10E0), QChar(0x10E2), QChar(0x10E7),
        QChar(0x10E4), QChar(0x10D2), QChar(0x10F0),
        QChar(0x10D5), QChar(0x10D1), QChar(0x10DC), };

static const QChar Georgian_RIGHT[] = {
        QChar(0x10E3), QChar(0x10D8), QChar(0x10DD),
        QChar(0x10EF), QChar(0x10D9), QChar(0x10DA),
        QChar(0x10DB), QChar(0x10DE), '.', };

GeorgianKeyboardData::GeorgianKeyboardData()
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

GeorgianKeyboardData::~GeorgianKeyboardData()
{
}

// for initialization
void GeorgianKeyboardData::initTopKeyCode()
{
    const QChar chs[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_codes_.push_back(dd);
    }
}

void GeorgianKeyboardData::initLeftKeyCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(Georgian_LEFT[i])));
        left_codes_.push_back(dd);
    }
}

void GeorgianKeyboardData::initMiddleKeyCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(Georgian_MIDDLE[i])));
        middle_codes_.push_back(dd);
    }
}

void GeorgianKeyboardData::initRightKeyCode()
{
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd(createData(QString(Georgian_RIGHT[i])));
        right_codes_.push_back(dd);
    }
}

void GeorgianKeyboardData::initBottomKeyCode()
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

// for shift key
void GeorgianKeyboardData::initTopKeyShiftCode()
{
    const QChar chs[] = { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')'};
    for (int i = 0; i < 10; i++)
    {
        ODataPtr dd(createData(QString(chs[i])));
        top_shift_codes_.push_back(dd);
    }
}

void GeorgianKeyboardData::initLeftKeyShiftCode()
{
    const QChar Georgian_SHITT_LEFT[] = {
            QChar(0x10E5), QChar(0x10ED), QChar(0x10D4),
            QChar(0x10D0), QChar(0x10E8), QChar(0x10D3),
            QChar(0x10EB), QChar(0x10EE), QChar(0x10E9), };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd (createData(QString(Georgian_SHITT_LEFT[i])));
        left_shift_codes_.push_back(dd);
    }
}

void GeorgianKeyboardData::initMiddleKeyShiftCode()
{
    const QChar Georgian_SHIFT_MIDDLE[] = {
            QChar(0x10E6), QChar(0x10D7), QChar(0x10E7),
            QChar(0x10E4), QChar(0x10D2), QChar(0x10F0),
            QChar(0x10D5), QChar(0x10D1), QChar(0x10DC), };
    for (int i = 0; i < 9; i++)
    {
        ODataPtr dd (createData(QString(Georgian_SHIFT_MIDDLE[i])));
        middle_shift_codes_.push_back(dd);
    }
}

void GeorgianKeyboardData::initRightKeyShiftCode()
{
    const QChar Georgian_SHIFT_RIGHT[] = {
            QChar(0x10E3), QChar(0x10D8), QChar(0x10DD),
            QChar(0x10DF), QChar(0x10D9), QChar(0x10DA),
            QChar(0x10DB), QChar(0x10DE), '.', };
    for (int i = 0; i < 8; i++)
    {
        ODataPtr dd(createData(QString(Georgian_SHIFT_RIGHT[i])));
        right_shift_codes_.push_back(dd);
    }
}

void GeorgianKeyboardData::initBottomKeyShiftCode()
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

void GeorgianKeyboardData::initLeftKeySymbolCode()
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

void GeorgianKeyboardData::initMiddleKeySymbolCode()
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

void GeorgianKeyboardData::initRightKeySymbolCode()
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

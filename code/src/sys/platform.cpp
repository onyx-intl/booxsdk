
#include "onyx/sys/platform.h"
#include <QtCore/qglobal.h>

namespace sys
{
static const QString IM31L = "imx31L";
static const QString MARVELL = "166e";
static const QString IMX508 = "imx508";
static const QString AK98 = "ak98";

QString platform()
{
    static QString p;
#ifndef BUILD_WITH_TFT
    if (p.isEmpty())
    {
        p = qgetenv("PLATFORM");
        if (p.isEmpty())
        {
            p = IM31L;
        }
    }
#else
    p = AK98;
#endif
    return p;
}

QString touchType()
{
    return qgetenv("TOUCH_TYPE");
}

bool isIMX31L()
{
    return platform() == IM31L;
}

bool is166E()
{
    return platform() == MARVELL;
}

bool isImx508()
{
    return platform() == IMX508;
}

bool isAk98()
{
    return platform() == AK98;
}

bool isIRTouch()
{
    return (touchType().compare("ir", Qt::CaseInsensitive) == 0);
}

QString soundModule()
{
    static QString mod = qgetenv("SOUND_MODULE");
    if (mod.isEmpty())
    {
        if (isIMX31L())
        {
            mod = "snd-soc-imx-3stack-wm8711";
        }
        else if (is166E())
        {
            mod = "snd_soc_booxe";
        }
        else if (isImx508())
        {
            mod = "snd_soc_imx_3stack_wm8976";
        }
        else
        {
            qDebug("No sound module found, check profile.");
        }
    }
    return mod;
}

int defaultRotation()
{
    static int rotation = -1;
    if (rotation < 0)
    {
        rotation = qgetenv("DEFAULT_ROTATION").toInt();
    }
    return rotation;
}

bool collectUserBehavior()
{
    static bool enable_collection = false;
    if (qgetenv("ENABLE_COLLECT_USER_BEHAVIOR").toInt() > 0)
    {
        enable_collection = true;
    }
    return enable_collection;
}

int batteryPercentageThreshold()
{
    static int threshold = -1;
    if (threshold < 0)
    {
        threshold = qgetenv("BATTERY_THRESHOLD").toInt();
    }
    if (threshold <= 0)
    {
        threshold = 5;
    }
    return threshold;
}

bool isNoTouch()
{
    return qgetenv("NO_TOUCH").toInt() > 0;
}

bool hasGlowLight()
{
    return qgetenv("GLOW_LIGHT").toInt() > 0;
}

bool isNoTouchByWithWifi()
{
    return qgetenv("NO_TOUCH_BUT_WITH_WIFI").toInt() > 0;
}

}    // namespace sys


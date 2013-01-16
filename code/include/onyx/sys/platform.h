// Authors: John

/// Public header of the system configuration library.

#ifndef ONYX_SYSTEM_PLATFORM_H__
#define ONYX_SYSTEM_PLATFORM_H__

#include <QString>

namespace sys
{

QString platform();

bool isIMX31L();

bool is166E();

bool isImx508();

bool isAk98();

bool isIRTouch();

QString soundModule();

int defaultRotation();

bool collectUserBehavior();

int batteryPercentageThreshold();

bool isNoTouch();

bool hasGlowLight();

bool isNoTouchButWithWifi();

}

#endif  // ONYX_SYSTEM_PLATFORM_H__

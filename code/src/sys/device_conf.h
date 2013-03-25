// -*- mode: c++; c-basic-offset: 4; -*-
// Authors: John


#ifndef SYS_DEVICE_CONF_H__
#define SYS_DEVICE_CONF_H__

#include "onyx/base/base.h"
#include <QString>
#include <QtSql/QtSql>

namespace sys
{

class DeviceConfig
{
  public:
    static bool makeSureTableExist(QSqlDatabase& db);
    static QString deviceId(QSqlDatabase& db);
    static QString serialNumber(QSqlDatabase& db);
    static QString version();
    static QString otaPathName();

    static QString deviceIdFromDatabase(QSqlDatabase &db);
    static QString serialNumberFromDatabase(QSqlDatabase& db);
    static QString setDeviceIdToDatabase(QSqlDatabase &db, const QString &string);
    static QString setSerialNumberToDatabase(QSqlDatabase& db, const QString &string);
    static QString wifiHardwareAddrFromDatabase(QSqlDatabase &db);
    static QString setWifiHardwareAddrToDatabase(QSqlDatabase &db, const QString &string);

private:
    DeviceConfig() {}
    ~DeviceConfig() {}
};

};

#endif  // SYS_DEVICE_CONF_H__


#include "onyx/base/base.h"
#include "onyx/sys/volume_conf.h"
#include "onyx/sys/platform.h"


namespace sys
{


VolumeConfig::VolumeConfig()
{
}

VolumeConfig::~VolumeConfig()
{
}

bool VolumeConfig::makeSureTableExist(QSqlDatabase &database)
{
    QSqlQuery query(database);
    bool ok = query.exec("create table if not exists volume ("
                         "key text primary key, "
                         "value text "
                         ")");
    if (ok)
    {
        return query.exec("create index if not exists key_index on volume (key) ");
    }
    return false;
}

int VolumeConfig::volume(QSqlDatabase &database)
{
    QSqlQuery query(database);
    query.prepare( "select value from volume where key = ? ");
    query.addBindValue("volume");
    int volume = -1;
    if (query.exec() && query.next())
    {
        volume = query.value(0).toInt();
    }
    return volume;
}

bool VolumeConfig::setVolume(QSqlDatabase &database, int volume)
{
    QSqlQuery query(database);
    query.prepare( "INSERT OR REPLACE into volume (key, value) values(?, ?)");
    query.addBindValue("volume");
    query.addBindValue(volume);
    return query.exec();
}

QVector<int> VolumeConfig::volumes()
{
    static QVector<int> VOLUMES;
    if (VOLUMES.size() <= 0)
    {

        if(sys::is166E())
        {
            VOLUMES.push_back(0);
            VOLUMES.push_back(2);
            VOLUMES.push_back(8);
            VOLUMES.push_back(16);
            VOLUMES.push_back(30);//5
            VOLUMES.push_back(40);
            VOLUMES.push_back(50);
            VOLUMES.push_back(60);
            VOLUMES.push_back(80);
            VOLUMES.push_back(90);//10
            VOLUMES.push_back(100);
        }
        else if (sys::isImx508())
        {
            if (sys::isIRTouch())
            {
                qDebug() << "is imx508, is ir touch";
                VOLUMES.push_back(0);
                VOLUMES.push_back(2);
                VOLUMES.push_back(8);
                VOLUMES.push_back(16);
                VOLUMES.push_back(30);//5
                VOLUMES.push_back(45);
                VOLUMES.push_back(55);
                VOLUMES.push_back(65);
                VOLUMES.push_back(70);
                VOLUMES.push_back(85);//10
                VOLUMES.push_back(100);
            }
            else
            {
                VOLUMES.push_back(0);
                VOLUMES.push_back(60);
                VOLUMES.push_back(64);
                VOLUMES.push_back(68);
                VOLUMES.push_back(72); //5
                VOLUMES.push_back(76);
                VOLUMES.push_back(80);
                VOLUMES.push_back(85);
                VOLUMES.push_back(90);
                VOLUMES.push_back(95); //10
                VOLUMES.push_back(100);
            }
        }

        else
        {
            VOLUMES.push_back(0);
            VOLUMES.push_back(60);
            VOLUMES.push_back(64);
            VOLUMES.push_back(68);
            VOLUMES.push_back(72);//5
            VOLUMES.push_back(76);
            VOLUMES.push_back(80);
            VOLUMES.push_back(85);
            VOLUMES.push_back(90);
            VOLUMES.push_back(95);//10
            VOLUMES.push_back(100);
        }
    }

    return VOLUMES;
}

int VolumeConfig::minVolume()
{
    return 0;
}

int VolumeConfig::maxVolume()
{
    return 100;
}

bool VolumeConfig::mute(QSqlDatabase &database, bool m)
{
    QSqlQuery query(database);
    query.prepare( "INSERT OR REPLACE into volume (key, value) values(?, ?)");
    query.addBindValue("mute");
    query.addBindValue(m);
    return query.exec();
}

bool VolumeConfig::isMute(QSqlDatabase &database)
{
    QSqlQuery query(database);
    query.prepare( "select value from volume where key = ? ");
    query.addBindValue("mute");
    bool mute = false;
    if (query.exec() && query.next())
    {
        mute = query.value(0).toBool();
    }
    return mute;
}

}

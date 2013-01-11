// Author: John

#include "onyx/cms/download_db.h"
#include "onyx/cms/cms_utils.h"

namespace cms
{
static const QString TAG_URL = "url";
static const QString TAG_PATH = "path";
static const QString TAG_SIZE = "size";
static const QString TAG_STATE = "state";
static const QString TAG_TIMESTAMP = "timestamp";
static const QString TAG_SPEED = "speed";
static const QString TAG_RECEIVED = "received";
static const QString TAG_INTERNAL_ID = "internalid";

DownloadItemInfo::DownloadItemInfo(const QVariantMap & vm)
    : OData(vm)
{
    setTimeStamp(QDateTime::currentDateTime().toString(dateFormat()));
}

DownloadItemInfo::DownloadItemInfo(const QString &u)
{
    setUrl(u);
    setPath("");
    setInternalId(-1);
    setState(STATE_INVALID);
    setTimeStamp(QDateTime::currentDateTime().toString(dateFormat()));
}

DownloadItemInfo::~DownloadItemInfo()
{
}

bool DownloadItemInfo::operator == (const DownloadItemInfo &right)
{
    return url() == right.url();
}

QString DownloadItemInfo::url() const
{
    return value(TAG_URL).toString();
}

void DownloadItemInfo::setUrl(const QString & url)
{
    insert(TAG_URL, url);
}
QString DownloadItemInfo::path() const
{
    return value(TAG_PATH).toString();
}

void DownloadItemInfo::setPath(const QString & path)
{
    insert(TAG_PATH, path);
}

int DownloadItemInfo::size() const
{
    return value(TAG_SIZE).toInt();
}

void DownloadItemInfo::setSize(int size)
{
    insert(TAG_SIZE, size);
}

DownloadState DownloadItemInfo::state() const
{
    return static_cast<DownloadState>(value(TAG_STATE).toInt());
}

void DownloadItemInfo::setState(DownloadState state)
{
    insert(TAG_STATE, state);
}

QString DownloadItemInfo::timeStamp() const
{
    return value(TAG_TIMESTAMP).toString();
}

void DownloadItemInfo::setTimeStamp(const QString & timeStamp)
{
    insert(TAG_TIMESTAMP, timeStamp);
}

int DownloadItemInfo::internalId() const
{
    return value(TAG_INTERNAL_ID).toInt();
}

void DownloadItemInfo::setInternalId(int id)
{
    insert(TAG_INTERNAL_ID, id);
}

QString DownloadItemInfo::speed() const
{
    return value(TAG_SPEED).toString();
}

void DownloadItemInfo::setSpeed(const QString & speed)
{
    insert(TAG_SPEED, speed);
}

int DownloadItemInfo::received() const
{
    return value(TAG_RECEIVED).toInt();
}

void DownloadItemInfo::setReceived(int size)
{
    insert(TAG_RECEIVED, size);
}

DownloadDB::DownloadDB(const QString & db_name)
    : database_name_(db_name) 
{
    open();
}

DownloadDB::~DownloadDB()
{
    close();
}

bool DownloadDB::open()
{
    if (!database_)
    {
        database_.reset(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", database_name_)));
    }

    if (!database_->isOpen())
    {
        QDir home = QDir::home();
        database_->setDatabaseName(home.filePath(database_name_));
        if (!database_->open())
        {
            qDebug() << database_->lastError().text();
            return false;
        }
        makeSureTableExist(*database_);
    }
    return true;
}

bool DownloadDB::close()
{
    if (database_)
    {
        database_->close();
        database_.reset(0);
        QSqlDatabase::removeDatabase(database_name_);
        return true;
    }
    return false;
}

/// Return all download item list including pending list, finished list and the others.
QStringList DownloadDB::list(DownloadState state)
{
    QStringList list;
    DownloadInfoList infoList = all(state);

    for(int i = 0; i < infoList.size(); ++i)
    {
        list.push_back(infoList[i].path());
    }
    return list;
}

DownloadInfoList DownloadDB::all(DownloadState state)
{
    DownloadInfoList list;

    // Fetch all download items.
    QSqlQuery query(db());
    query.prepare( "select url, value from download ");
    if (!query.exec())
    {
        return list;
    }

    while (query.next())
    {
        QVariantMap m;
        QByteArray ba = query.value(1).toByteArray();
        QDataStream stream(&ba, QIODevice::ReadOnly);
        stream >> m;

        DownloadItemInfo item(m);

        // Ignore items finished.
        if (item.state() == state || state == STATE_INVALID)
        {
            if (!list.contains(item))
            {
                list.push_back(item);
            }
        }
    }

    return list;
}

bool DownloadDB::infoListContains(const DownloadInfoList & info_list, const DownloadItemInfo &item)
{
    bool contains = false;
    foreach (DownloadItemInfo i, info_list)
    {
        if (i.url() == item.url())
        {
            contains = true;
            break;
        }
    }
    return contains;
}

void DownloadDB::infoListRemove(DownloadInfoList & info_list, const DownloadItemInfo &item)
{
    foreach (DownloadItemInfo i, info_list)
    {
        if (i.url() == item.url())
        {
            info_list.remove(info_list.indexOf(i));
        }
    }
}

DownloadInfoList DownloadDB::pendingList(QStringList input,
                                         bool force_all,
                                         bool sort)
{
    DownloadInfoList list;

    // Fetch all download items.
    QSqlQuery query(db());
    query.prepare( "select url, value from download ");
    if (!query.exec())
    {
        return list;
    }

    while (query.next())
    {
        QVariantMap m;
        QByteArray ba = query.value(1).toByteArray();
        QDataStream stream(&ba, QIODevice::ReadOnly);
        stream >> m;

        DownloadItemInfo item(m);

        // Remove the items failed from input too.
        if (item.state() == FAILED)
        {
            // remove the items if exist in input
            input.removeAll(item.url());
        }

        DownloadState state = item.state();
        // Ignore items finished or failed.
        if ((state != FINISHED && state != FINISHED_READ && state != FAILED) || force_all)
        {
            if (!list.contains(item))
            {
                list.push_back(item);
            }
        }
    }

    // check input now.
    foreach(QString i, input)
    {
        DownloadItemInfo item_info;
        item_info.setUrl(i);
        if (!infoListContains(list, item_info))
        {
            list.push_back(item_info);
        }
    }

    if (sort)
    {
        qSort(list.begin(), list.end(), GreaterByTimestamp());
    }
    return list;
}

bool DownloadDB::update(const DownloadItemInfo & item)
{
    QSqlQuery query(db());
    query.prepare( "INSERT OR REPLACE into download (url, value) values(?, ?)");
    query.addBindValue(item.url());

    QByteArray ba;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    stream << item;

    query.addBindValue(ba);
    return query.exec();
}

bool DownloadDB::updateState(const QString & url, DownloadState state)
{
    // find the download item info first.
    QSqlQuery query(db());
    query.prepare( "select url, value from download where url = ?");
    query.addBindValue(url);
    if (!query.exec())
    {
        return false;
    }

    QVariantMap m;
    if (query.next())
    {
        QByteArray ba = query.value(1).toByteArray();
        QDataStream stream(&ba, QIODevice::ReadOnly);
        stream >> m;
    }
    DownloadItemInfo item(m);
    item.setUrl(url);
    item.setState(state);
    return update(item);
}

int  DownloadDB::itemCount(DownloadState state)
{
    return list(state).size();
}

bool DownloadDB::markAsRead(const QString & path,
                            DownloadState state)
{
    bool found = false;
    DownloadInfoList list = all(STATE_INVALID);
    foreach(DownloadItemInfo info, list)
    {
        if (path == info.path())
        {
            found = true;
            info.setState(state);
            update(info);
        }
    }
    return found;
}

void DownloadDB::markAllAsRead(DownloadState state)
{
    DownloadInfoList list = all(STATE_INVALID);
    foreach(DownloadItemInfo info, list)
    {
        info.setState(state);
        update(info);
    }
}

bool DownloadDB::remove(const QString & url)
{
    QSqlQuery query(db());
    query.prepare( "delete from download where url = ?");
    query.addBindValue(url);
    return query.exec();
}

QString DownloadDB::getPathByUrl(const QString &url)
{
    QString path;

    // Fetch all download items.
    QSqlQuery query(db());
    query.prepare( "select url, value from download where url = ?");
    query.addBindValue(url);
    if (!query.exec())
    {
        return path;
    }

    if (query.next())
    {
        QVariantMap m;
        QByteArray ba = query.value(1).toByteArray();
        QDataStream stream(&ba, QIODevice::ReadOnly);
        stream >> m;

        DownloadItemInfo item(m);
        path = item.path();
    }
    return path;
}

QString DownloadDB::getPathById(int id)
{
    QString path;

    // Fetch all download items.
    QSqlQuery query(db());
    query.prepare( "select url, value from download ");
    if (!query.exec())
    {
        return path;
    }

    while (query.next())
    {
        if (!query.value(0).toString().isEmpty())
        {
            QVariantMap m;
            QByteArray ba = query.value(1).toByteArray();
            QDataStream stream(&ba, QIODevice::ReadOnly);
            stream >> m;

            DownloadItemInfo item(m);
            if (item.internalId() == id)
            {
                path = item.path();
                break;
            }
        }
    }

    return path;
}

bool DownloadDB::makeSureTableExist(QSqlDatabase &db)
{
    QSqlQuery query(db);
    query.exec("create table if not exists download ("
               "url text primary key,"
               "value blob) ");
    query.exec("create index if not exists url_index on info (url)");
    return true;
}

QSqlDatabase & DownloadDB::db()
{
    return *database_;
}


}  // namespace cms



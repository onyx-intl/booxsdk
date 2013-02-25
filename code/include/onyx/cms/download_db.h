// Author: John

#ifndef CMS_DOWNLOAD_DB_H_
#define CMS_DOWNLOAD_DB_H_

#include <QtCore/QtCore>
#include <QtSql/QtSql>
#include "onyx/base/base.h"
#include "onyx/data/data.h"

namespace cms
{

enum DownloadState
{
    STATE_INVALID = 0,
    DOWNLOADING = 1,
    FINISHED = 2,       ///< Finished but not read.
    FAILED = 3,         ///< Don't ever download it again.
    PENDING = 4,        ///< Download next time.
    FINISHED_READ = 5   ///< Download finished and has been read.
};

class DownloadItemInfo : public OData
{
public:
    explicit DownloadItemInfo(const QVariantMap & vm = QVariantMap());
    explicit DownloadItemInfo(const QString &u);
    ~DownloadItemInfo();

    bool operator == (const DownloadItemInfo &right);

    QString url() const;
    void setUrl(const QString & url);

    QString path() const;
    void setPath(const QString & path);

    int size() const;
    void setSize(int size);

    DownloadState state() const;
    void setState(DownloadState state);

    QString speed() const;
    void setSpeed(const QString & speed);

    int received() const;
    void setReceived(int size);

    QString timeStamp() const;
    void setTimeStamp(const QString & timeStamp);

    int internalId() const;
    void setInternalId(int id);

};

typedef QVector<DownloadItemInfo> DownloadInfoList;

struct LessByUrl
{
    bool operator()( const DownloadItemInfo & a, const DownloadItemInfo & b ) const
    {
        return (a.url().compare(b.url(), Qt::CaseInsensitive) < 0);
    }
};

struct GreaterByUrl
{
    bool operator()( const DownloadItemInfo & a, const DownloadItemInfo & b ) const
    {
        return (a.url().compare(b.url(), Qt::CaseInsensitive) > 0);
    }
};

struct LessByTimestamp
{
    bool operator()( const DownloadItemInfo & a, const DownloadItemInfo & b ) const
    {
        return (a.timeStamp().compare(b.timeStamp(), Qt::CaseInsensitive) < 0);
    }
};

struct GreaterByTimestamp
{
    bool operator()( const DownloadItemInfo & a, const DownloadItemInfo & b ) const
    {
        return (a.timeStamp().compare(b.timeStamp(), Qt::CaseInsensitive) > 0);
    }
};

class DownloadDB
{
public:
    DownloadDB(const QString & db_name = "download.db");
    ~DownloadDB();

public:
    bool open();
    bool close();

    DownloadInfoList pendingList(QStringList input = QStringList(),
                             bool force_all = false,
                             bool sort = true);

    bool update(const DownloadItemInfo & item);
    bool updateState(const QString & url, DownloadState state = FINISHED);

    int  itemCount(DownloadState state = FINISHED);
    QStringList list(DownloadState state = FINISHED);
    DownloadInfoList all(DownloadState state = STATE_INVALID);
    bool markAsRead(const QString & path, DownloadState state = FINISHED_READ);
    void markAllAsRead(DownloadState state = FINISHED_READ);
    bool remove(const QString & url);
    QString getPathByUrl(const QString & url);

    QString getPathById(int id);

    bool infoListContains(const DownloadInfoList & info_list, const DownloadItemInfo &item);
    void infoListRemove(DownloadInfoList & info_list, const DownloadItemInfo &item);

private:
    bool makeSureTableExist(QSqlDatabase &db);
    QSqlDatabase & db();

private:
    scoped_ptr<QSqlDatabase> database_;

    const QString database_name_;

};


}  // namespace cms

#endif  // CMS_DOWNLOAD_DB_H_

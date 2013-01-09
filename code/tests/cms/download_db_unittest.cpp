// Copyright 2007-2013 Onyx International Inc.
// All Rights Reserved.
// Author: Jim

#include "gtest/gtest.h"
#include "onyx/base/base.h"
#include "onyx/cms/download_db.h"

namespace
{
using namespace cms;

TEST(DownloadDBTest, InfoListContains)
{
    DownloadDB download_db;
    DownloadInfoList list;
    DownloadItemInfo item1;
    item1.setUrl("http://www.url.com/path1");
    item1.setInternalId(1);
    item1.setPath("/path/to/file1");
    list.push_back(item1);

    DownloadItemInfo item2;
    item2.setUrl("http://www.url.com/path2");
    item2.setInternalId(2);
    item2.setPath("/path/to/file2");
    list.push_back(item2);

    EXPECT_TRUE(download_db.infoListContains(list, item1));
    EXPECT_TRUE(download_db.infoListContains(list, item2));
}

TEST(DownloadDBTest, InfoListRemove)
{
    DownloadDB download_db;
    DownloadInfoList list;
    DownloadItemInfo item1;
    item1.setUrl("http://www.url.com/path1");
    item1.setInternalId(1);
    item1.setPath("/path/to/file1");
    list.push_back(item1);

    DownloadItemInfo item2;
    item2.setUrl("http://www.url.com/path2");
    item2.setInternalId(2);
    item2.setPath("/path/to/file2");
    list.push_back(item2);

    EXPECT_EQ(2, list.size());
    download_db.infoListRemove(list, item1);
    EXPECT_EQ(1, list.size());
    EXPECT_FALSE(download_db.infoListContains(list, item1));
    EXPECT_TRUE(download_db.infoListContains(list, item2));
    download_db.infoListRemove(list, item2);
    EXPECT_EQ(0, list.size());
    EXPECT_FALSE(download_db.infoListContains(list, item1));
    EXPECT_FALSE(download_db.infoListContains(list, item2));
}

TEST(DownloadDBTest, Update)
{
    DownloadDB download_db;
    DownloadItemInfo item1;
    QString url = "http://www.url.com/path1";
    item1.setUrl(url);
    QString expected_path = "/path/to/file1";
    item1.setPath(expected_path);
    item1.setState(FAILED);
    download_db.update(item1);

    QString path = download_db.getPathByUrl(url);
    EXPECT_STREQ(expected_path.toStdString().c_str(), path.toStdString().c_str());
}

TEST(DownloadDBTest, MethodAll)
{
    DownloadDB download_db;
    DownloadItemInfo item1;
    item1.setUrl("http://www.url.com/path1");
    item1.setInternalId(1);
    item1.setPath("/path/to/file1");
    item1.setState(FAILED);

    DownloadItemInfo item2;
    item2.setUrl("http://www.url.com/path2");
    item2.setInternalId(2);
    item2.setPath("/path/to/file2");
    item2.setState(DOWNLOADING);

    download_db.update(item1);
    download_db.update(item2);

    DownloadInfoList filter_list = download_db.all(DOWNLOADING);
    EXPECT_FALSE(filter_list.isEmpty());
    EXPECT_TRUE(1 == filter_list.size());
    EXPECT_TRUE(DOWNLOADING == filter_list.first().state());

    filter_list = download_db.all(FAILED);
    EXPECT_FALSE(filter_list.isEmpty());
    EXPECT_EQ(1, filter_list.size());
    EXPECT_EQ(FAILED, filter_list.first().state());
}

TEST(DownloadDBTest, Pending)
{
    DownloadDB download_db;
    DownloadItemInfo item1;
    item1.setUrl("http://www.url.com/path1");
    item1.setInternalId(1);
    item1.setPath("/path/to/file1");
    item1.setState(FAILED);

    DownloadItemInfo item2;
    item2.setUrl("http://www.url.com/path2");
    item2.setInternalId(2);
    item2.setPath("/path/to/file2");
    item2.setState(DOWNLOADING);

    DownloadItemInfo item3;
    item3.setUrl("http://www.url.com/path3");
    item3.setInternalId(3);
    item3.setPath("/path/to/file3");
    item3.setState(FINISHED);

    DownloadItemInfo item4;
    item4.setUrl("http://www.url.com/path4");
    item4.setInternalId(4);
    item4.setPath("/path/to/file4");
    item4.setState(PENDING);

    DownloadItemInfo item5;
    item5.setUrl("http://www.url.com/path5");
    item5.setInternalId(5);
    item5.setPath("/path/to/file5");
    item5.setState(FINISHED_READ);

    DownloadItemInfo item6;
    item6.setUrl("http://www.url.com/path6");
    item6.setInternalId(6);
    item6.setPath("/path/to/file6");
    item6.setState(STATE_INVALID);

    download_db.update(item1);
    download_db.update(item2);
    download_db.update(item3);
    download_db.update(item4);
    download_db.update(item5);
    download_db.update(item6);

    DownloadInfoList pending = download_db.pendingList();
    EXPECT_EQ(3, pending.size());

    EXPECT_EQ(DOWNLOADING, pending.first().state());
    EXPECT_EQ(PENDING, pending.at(1).state());
    EXPECT_EQ(STATE_INVALID, pending.back().state());
}

}

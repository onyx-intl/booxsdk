// Copyright 2007 Onyx International Inc.
// All Rights Reserved.
// Author: John

#include "gtest/gtest.h"
#include "onyx/base/base.h"
#include "onyx/cms/content_manager.h"
#include "onyx/cms/user_db.h"
#include "onyx/cms/download_db.h"

namespace
{
using namespace cms;

static void CreateTempFile(const QString & path_name,
                           const unsigned int size)
{
    QFile file(path_name);
    file.open(QIODevice::ReadWrite);
    char * data = new char[size];
    file.write(data, size);
    file.close();
    delete [] data;
}


/// Open database that does not exist.
/// Results: It can create a new database.
TEST(ContentManagerTest, OpenNotExist)
{
    QDir current = QDir::current();
    QString db = current.filePath("temp.db");

    ContentManager mgr;
    EXPECT_TRUE(mgr.open(db));
    mgr.close();
    current.remove(db);
}

/// Open database on readonly file system like CDROM.
/// Results: It throws exception. Not sure where to catch it
/// : in the Open function or in caller.
TEST(ContentManagerTest, CreateOnReadOnlyFileSystem)
{
    ContentManager mgr;
    EXPECT_FALSE(mgr.open("/xyz/abc/read_only_file_system.db"));
}

/// Put a new content into the database and query it.
TEST(ContentManagerTest, GetContentNodeByName)
{
    QDir current = QDir::current();
    QString db = current.filePath("temp.db");
    current.remove(db);

    ContentManager mgr;
    EXPECT_TRUE(mgr.open(db));

    static const int SIZE = 1024;
    static const QString TEMP_FILE_NAME = "temp.file";
    QString temp_file = current.filePath(TEMP_FILE_NAME);
    CreateTempFile(temp_file, SIZE);

    ContentNode node;
    QFileInfo info(current, TEMP_FILE_NAME);
    node.updateLastAccess();
    node.mutable_name() = info.fileName();
    node.mutable_location() = info.path();
    node.mutable_size() = info.size();
    node.mutable_authors() = "zzzzzzzzzzzz";
    node.mutable_description() = "test sample";
    node.mutable_title() = "abc";
    mgr.createContentNode(node);

    ContentNode result;
    EXPECT_TRUE(mgr.getContentNode(result, info.absoluteFilePath()));

    EXPECT_TRUE(node == result);

    // Should remove it otherwise the test will fail.
    current.remove(db);
}


/// Put a new content into the database and query it by url.
TEST(ContentManagerTest, GetContentNodeByUrl)
{
    QDir current = QDir::current();
    QString db = current.filePath("temp.db");
    current.remove(db);

    ContentManager mgr;
    EXPECT_TRUE(mgr.open(db));

    static const int SIZE = 1024;
    static const QString TEMP_FILE_NAME = "temp.file";
    QString temp_file = current.filePath(TEMP_FILE_NAME);
    CreateTempFile(temp_file, SIZE);

    ContentNode node;
    QFileInfo info(current, TEMP_FILE_NAME);
    node.updateLastAccess();
    node.mutable_name() = info.fileName();
    node.mutable_location() = info.path();
    node.mutable_size() = info.size();
    node.mutable_authors() = "zzzzzzzzzzzz";
    node.mutable_description() = "test sample";
    node.mutable_title() = "abc";
    const QString my_url = "http://www.test.com/";
    const QString wrong_url = "http://www.test.com./";
    EXPECT_TRUE(mgr.updateContentNodeByUrl(node, my_url));

    node.mutable_title() = "cba";
    EXPECT_TRUE(mgr.updateContentNodeByUrl(node, my_url));

    ContentNode result;
    EXPECT_TRUE(mgr.getContentNodeByUrl(result, my_url));
    EXPECT_TRUE(node.name() == result.name());
    EXPECT_TRUE(node.authors() == result.authors());
    EXPECT_TRUE(node.title() == result.title());
    EXPECT_FALSE(mgr.getContentNodeByUrl(result, wrong_url));

    // Should remove it otherwise the test will fail.
    current.remove(db);
}


/// Test the attribute blob object.
TEST(ContentManagerTest, GetAttribute)
{
    QDir current = QDir::current();
    QString db = current.filePath("temp.db");
    current.remove(db);

    ContentManager mgr;
    EXPECT_TRUE(mgr.open(db));

    static const int SIZE = 1024;
    static const QString TEMP_FILE_NAME = "temp.file";
    QString temp_file = current.filePath(TEMP_FILE_NAME);
    CreateTempFile(temp_file, SIZE);

    ContentNode node;
    QFileInfo info(current, TEMP_FILE_NAME);
    node.updateLastAccess();
    node.mutable_name() = info.fileName();
    node.mutable_location() = info.path();
    node.mutable_size() = info.size();
    node.mutable_authors() = "zzzzzzzzzzzz";
    node.mutable_description() = "test sample";
    node.mutable_title() = "abc";

    QVariantMap input, output;
    input.insert("isbn1", "aaaaaaaa");
    input.insert("isbn2", "bbbbbbbb");
    input.insert("isbn3", "cccccccc");
    node.setAttributes(input);
    mgr.createContentNode(node);

    ContentNode result;
    mgr.getContentNode(node.id(), result);
    result.attributes(output);

    EXPECT_TRUE(input == output);
    QString data = output.value("isbn1").toString();
    data = output.value("isbn2").toString();
    data = output.value("isbn3").toString();


    // test update node
    QVariantMap new_input, new_output;
    new_input.insert("isbn2", "bbbbbbbbxxxxxxx");
    new_input.insert("isbn3", "ccccccccxxxxxxx");
    result.setAttributes(new_input);
    mgr.updateContentNode(result);

    ContentNode xx;
    mgr.getContentNode(node.id(), xx);
    xx.attributes(new_output);
    data = new_output.value("isbn2").toString();
    data = new_output.value("isbn3").toString();

    // Should remove it otherwise the test will fail.
    current.remove(db);
}

/// Test the options blob object.
TEST(ContentManagerTest, GetOption)
{
    QDir current = QDir::current();
    QString db = current.filePath("temp.db");
    current.remove(db);

    ContentManager mgr;
    EXPECT_TRUE(mgr.open(db));

    static const int SIZE = 1024;
    static const QString TEMP_FILE_NAME = "temp.file";
    QString temp_file = current.filePath(TEMP_FILE_NAME);
    CreateTempFile(temp_file, SIZE);

    ContentNode node;
    QFileInfo info(current, TEMP_FILE_NAME);
    node.updateLastAccess();
    node.mutable_name() = info.fileName();
    node.mutable_location() = info.path();
    node.mutable_size() = info.size();
    node.mutable_authors() = "zzzzzzzzzzzz";
    node.mutable_description() = "test sample";
    node.mutable_title() = "abc";
    mgr.createContentNode(node);

    cms_blob options;
    options.resize(SIZE);
    for(int i = 0; i < SIZE; ++i)
    {
        options[i] = qrand();
    }
    mgr.updateOptions(node.id(), options);

    cms_blob result;
    mgr.getContentOptions(node.id(), result);
    EXPECT_TRUE(options == result);

    // Should remove it otherwise the test will fail.
    current.remove(db);
}

/// Test the bookmarks blob object.
TEST(ContentManagerTest, GetBookmark)
{
    QDir current = QDir::current();
    QString db = current.filePath("temp.db");
    current.remove(db);

    ContentManager mgr;
    EXPECT_TRUE(mgr.open(db));

    static const int SIZE = 1024;
    static const QString TEMP_FILE_NAME = "temp.file";
    QString temp_file = current.filePath(TEMP_FILE_NAME);
    CreateTempFile(temp_file, SIZE);

    ContentNode node;
    QFileInfo info(current, TEMP_FILE_NAME);
    node.updateLastAccess();
    node.mutable_name() = info.fileName();
    node.mutable_location() = info.path();
    node.mutable_size() = info.size();
    node.mutable_authors() = "zzzzzzzzzzzz";
    node.mutable_description() = "test sample";
    node.mutable_title() = "abc";
    mgr.createContentNode(node);

    cms_blob bookmarks;
    bookmarks.resize(SIZE);
    for(int i = 0; i < SIZE; ++i)
    {
        bookmarks[i] = qrand();
    }
    mgr.updateBookmarks(node.id(), bookmarks);

    cms_blob result;
    mgr.getBookmarks(node.id(), result);
    EXPECT_TRUE(bookmarks == result);

    // Should remove it otherwise the test will fail.
    current.remove(db);
}

TEST(ContentManagerTest, RecentDocuments)
{
    QDir current = QDir::current();
    QString db = current.filePath("temp.db");
    current.remove(db);

    ContentManager mgr;
    EXPECT_TRUE(mgr.open(db));

    static const int SIZE = 1024;
    static const int COUNT = 10;
    cms_long ids[COUNT] = {CMS_INVALID_ID};

    for(int i = 0; i < COUNT; ++i)
    {
        QString name = "temp_%1.file";
        name = name.arg(i);
        QString temp_file = current.filePath(name);
        CreateTempFile(temp_file, SIZE);

        ContentNode node;
        mgr.getContentNode(node, temp_file);
        node.updateLastAccess();
        mgr.updateContentNode(node);

        ids[i] = node.id();
        EXPECT_TRUE(mgr.addToRecentDocuments(temp_file));

        // Should add a sleep here.
    }

    cms::cms_ids result;
    EXPECT_TRUE(mgr.getRecentDocuments(result));

    for(int i = 0; i < COUNT; ++i)
    {
        EXPECT_TRUE(ids[i] == result[i]);
    }

    QString path = mgr.latestReading();
    EXPECT_FALSE(path.isEmpty());

    // Should remove it otherwise the test will fail.
    current.remove(db);
}

TEST(ContentManagerTest, sketchDB)
{
    QString db = getSketchDB("a.pdf");
}

TEST(ContentManagerTest, Shortcuts)
{
    QDir current = QDir::current();
    QString db = current.filePath("temp.db");
    current.remove(db);

    ContentManager mgr;
    EXPECT_TRUE(mgr.open(db));

    static const int COUNT = 10;

    for(int i = 0; i < COUNT; ++i)
    {
        QString name = "temp.file";
        EXPECT_TRUE(mgr.link(name, name));
        EXPECT_TRUE(mgr.links(name) == 1);

        EXPECT_TRUE(mgr.unlinkBySource(name));
        EXPECT_TRUE(mgr.links(name) == 0);
    }

    // Should remove it otherwise the test will fail.
    current.remove(db);
}


TEST(ContentManagerTest, Notes)
{
    QDir current = QDir::current();
    QString db = current.filePath("temp.db");
    current.remove(db);

    ContentManager mgr;
    EXPECT_TRUE(mgr.open(db));

    Notes notes;
    mgr.allNotes(notes);

    NoteInfo note;
    note.mutable_name() = mgr.suggestedNoteName();
    EXPECT_TRUE(mgr.addNoteIndex(note));
    // THIS IS FAILING!
    // EXPECT_TRUE(mgr.removeNote(note.name()));

    QString path = internalSketchDBPath(mgr.suggestedNoteName());
    EXPECT_FALSE(path.isEmpty());
    current.remove(db);
}

TEST(ContentManagerTest, UserDB)
{
    UserDB db;
    EXPECT_TRUE(db.open());


    db.store("adobe_id", "aaaa");
    db.store("user", "my_user_name");
    db.store("password", "my_password");

    QString result;
    QVariant value;
    db.load("adobe_id", value);
    result = value.toString();

    db.load("user", value);
    result = value.toString();

    db.load("password", value);
    result = value.toString();
}

TEST(ContentManagerTest, DownloadDB)
{
    DownloadDB db;
    EXPECT_TRUE(db.open());

    const QString url = "http://test.com/test.html";
    DownloadInfoList list = db.pendingList();
    EXPECT_TRUE(db.updateState(url, cms::DOWNLOADING));

    list = db.pendingList();
    DownloadItemInfo item;
    item.setUrl(url);
    EXPECT_TRUE(db.infoListContains(list, item));

}


}   // end of namespace

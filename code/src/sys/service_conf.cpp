
#include "onyx/sys/service_conf.h"

namespace sys
{

static const QString OPEN_METHOD = "open";

/// Define internal service.
static Service nm_service("com.onyx.service.nm",
                          "/com/onyx/object/nm",
                          "com.onyx.interface.nm",
                          OPEN_METHOD,
                          "network_manager");

static Service note_service("com.onyx.service.images",
                            "/com/onyx/object/images",
                            "com.onyx.interface.images",
                            OPEN_METHOD,
                            "image_reader");

static Service wb_service("com.onyx.service.webbrowser",
                          "/com/onyx/object/webbrowser",
                          "com.onyx.interface.webbrowser",
                          OPEN_METHOD,
                          "web_browser");

static Service drm_service("com.onyx.service.adobe_drm_handler",
                           "/com/onyx/object/adobe_drm_handler",
                           "com.onyx.interface.adobe_drm_handler",
                           OPEN_METHOD,
                           "naboo_reader");

static Service messenger_service("com.onyx.service.messenger",
                                 "/com/onyx/object/messenger",
                                 "com.onyx.interface.messenger",
                                 OPEN_METHOD,
                                "messenger");

static Service write_pad_service("com.onyx.service.text_editor",
                                 "/com/onyx/object/text_editor",
                                 "com.onyx.interface.text_editor",
                                 OPEN_METHOD,
                                "text_editor");

static Service dict_service("com.onyx.service.dict_tool",
                            "/com/onyx/object/dict_tool",
                            "com.onyx.interface.dict_tool",
                            OPEN_METHOD,
                            "dict_tool");

static Service rss_service("com.onyx.service.feed_reader",
                            "/com/onyx/object/feed_reader",
                            "com.onyx.interface.feed_reader",
                            OPEN_METHOD,
                            "feed_reader");

static Service newsfeeds_Service("com.onyx.service.rss_reader",
                            "/com/onyx/object/rss_reader",
                            "com.onyx.interface.rss_reader",
                            OPEN_METHOD,
                            "rss_reader");

static Service sudoku_service("com.onyx.service.sudoku",
                            "/com/onyx/object/sudoku",
                            "com.onyx.interface.sudoku",
                            OPEN_METHOD,
                            "sudoku.oar");

static  Service naboo_viewer_service("com.onyx.service.naboo_viewer",
                            "/com/onyx/object/naboo_viewer",
                            "com.onyx.interface.naboo_viewer",
                            OPEN_METHOD,
                            "naboo_reader");

static  Service onyx_pdf_reader_service("com.onyx.service.onyx_pdf_reader",
                            "/com/onyx/object/onyx_pdf_reader",
                            "com.onyx.interface.onyx_pdf_reader",
                            OPEN_METHOD,
                            "onyx_pdf_reader");

static   Service onyx_reader("com.onyx.service.onyx_reader",
                             "/com/onyx/object/onyx_reader",
                             "com.onyx.interface.onyx_reader",
                            OPEN_METHOD,
                            "onyx_reader");

static   Service cool_reader("com.onyx.service.cool_reader",
                             "/com/onyx/object/cool_reader",
                             "com.onyx.interface.cool_reader",
                            OPEN_METHOD,
                            "cr3");

static   Service html_reader("com.onyx.service.htmlreader",
                            "/com/onyx/object/htmlreader",
                            "com.onyx.interface.htmlreader",
                           OPEN_METHOD,
                           "html_reader");

static   Service office_viewer("com.onyx.service.office_viewer",
                            "/com/onyx/object/office_viewer",
                            "com.onyx.interface.office_viewer",
                            OPEN_METHOD,
                            "office_viewer");

static   Service comic_reader("com.onyx.service.comic_reader",
                            "/com/onyx/object/comic_reader",
                            "com.onyx.interface.comic_reader",
                            OPEN_METHOD,
                            "comic_reader");

Service::Service()
{
}

Service::Service(const QString &svr, const QString &obj,
                 const QString &ifn, const QString &m, const QString &app)
        : service_name_(svr)
        , object_path_(obj)
        , interface_name_(ifn)
        , method_(m)
        , app_name_(app)
        , runnable_(false)
{
}

Service::Service(const Service &right)
    : extension_name_(right.extension_name_)
    , service_name_(right.service_name_)
    , object_path_(right.object_path_)
    , interface_name_(right.interface_name_)
    , method_(right.method_)
    , app_name_(right.app_name_)
    , runnable_(right.runnable_)
{
}

Service::Service(bool runnable)
: runnable_(runnable)
{
}

Service::~Service()
{
}

Service & Service::operator = (const Service & right)
{
    if (this != &right)
    {
        extension_name_ = right.extension_name_;
        service_name_   = right.service_name_;
        object_path_    = right.object_path_;
        interface_name_ = right.interface_name_;
        method_         = right.method_;
        app_name_       = right.app_name_;
        runnable_       = right.runnable_;
    }
    return *this;
}

bool Service::operator== (const Service & right)
{
    return (service_name_ == right.service_name_);
}

/// Need a default service map.
static Services DEFAULT_SERVICES;

/// Maintain all services state. Service is used by explorer to
/// view content for end user.
/// The service files usually is under /usr/share/dbus-1/services/
/// [D-BUS Service]
/// Name=com.onyx.text
/// Exec=/usr/bin/textviewer
/// The services table is generated by hand.
ServiceConfig::ServiceConfig()
{
    loadDefaultServices();
}

ServiceConfig::~ServiceConfig()
{
}

bool ServiceConfig::makeSureTableExist(QSqlDatabase& database)
{
    QSqlQuery query(database);
    bool ok = query.exec("create table if not exists services ("
                         "extension text primary key, "
                         "service text, "
                         "object text, "
                         "interface text, "
                         "method text, "
                         "app text "
                         ")");
    if (ok)
    {
        return query.exec("create index if not exists key_index on services (extension) ");
    }
    return true;
}

void ServiceConfig::loadDefaultServices()
{
    int enable_fb_epub = qgetenv("ENABLE_FB_EPUB").toInt();
    if (DEFAULT_SERVICES.size() <= 0)
    {
        // html based service.
        Service html_service("com.onyx.service.htmlreader",
                             "/com/onyx/object/htmlreader",
                             "com.onyx.interface.htmlreader",
                            OPEN_METHOD,
                            "html_reader");
        html_service.mutable_extensions().push_back("chm");
        DEFAULT_SERVICES.push_back(html_service);

        // naboo viewer service.
        naboo_viewer_service.mutable_extensions().push_back("pdf");
        if (!enable_fb_epub)
        {
            naboo_viewer_service.mutable_extensions().push_back("epub");
        }
        DEFAULT_SERVICES.push_back(naboo_viewer_service);
#if 0
        // onyx pdf reader service.
        onyx_pdf_reader_service.mutable_extensions().push_back("pdf");
        if (!enable_fb_epub)
        {
            onyx_pdf_reader_service.mutable_extensions().push_back("epub");
        }
        DEFAULT_SERVICES.push_back(onyx_pdf_reader_service);
#endif
        // image service.
        if (note_service.mutable_extensions().size() <= 0)
        {
            note_service.mutable_extensions().push_back("gif");
            note_service.mutable_extensions().push_back("jpg");
            note_service.mutable_extensions().push_back("jpeg");
            note_service.mutable_extensions().push_back("png");
            note_service.mutable_extensions().push_back("tif");
            note_service.mutable_extensions().push_back("tiff");
            note_service.mutable_extensions().push_back("bmp");
        }
        DEFAULT_SERVICES.push_back(note_service);

        // Script service.
        Service script_service("com.onyx.service.script",
                               "/com/onyx/object/script",
                               "com.onyx.interface.script",
                               OPEN_METHOD,
                               "script_manager");
        script_service.mutable_extensions().push_back("js");
        DEFAULT_SERVICES.push_back(script_service);

        // Wifi configuration file.
        nm_service.mutable_extensions().push_back("wpa");
        DEFAULT_SERVICES.push_back(nm_service);

        // web browser based service.
        wb_service.mutable_extensions().push_back("htm");
        wb_service.mutable_extensions().push_back("html");
        DEFAULT_SERVICES.push_back(wb_service);

        // onyx_reader based service
        bool has_office_viewer = hasOfficeViewer();
        if (!has_office_viewer)
        {
            onyx_reader.mutable_extensions().push_back("doc");
        }
        onyx_reader.mutable_extensions().push_back("txt");
        onyx_reader.mutable_extensions().push_back("pdb");
        onyx_reader.mutable_extensions().push_back("fb2");
        onyx_reader.mutable_extensions().push_back("oeb");
        onyx_reader.mutable_extensions().push_back("ztxt");
        onyx_reader.mutable_extensions().push_back("tcr");
        onyx_reader.mutable_extensions().push_back("rtf");
        onyx_reader.mutable_extensions().push_back("prc");
        onyx_reader.mutable_extensions().push_back("mobi");
        onyx_reader.mutable_extensions().push_back("zip");
        onyx_reader.mutable_extensions().push_back("bz2");
        onyx_reader.mutable_extensions().push_back("gz");
        onyx_reader.mutable_extensions().push_back("abf");

        if (enable_fb_epub)
        {
            onyx_reader.mutable_extensions().push_back("epub");
        }

        // Seems it can not open tar file.
        DEFAULT_SERVICES.push_back(onyx_reader);

        // cool_reader based service
        if (!has_office_viewer)
        {
            cool_reader.mutable_extensions().push_back("doc");
        }
        cool_reader.mutable_extensions().push_back("txt");
        cool_reader.mutable_extensions().push_back("fb2");
        cool_reader.mutable_extensions().push_back("html");
        cool_reader.mutable_extensions().push_back("tcr");
        cool_reader.mutable_extensions().push_back("rtf");
        cool_reader.mutable_extensions().push_back("epub");


        // Seems it can not open tar file.
        DEFAULT_SERVICES.push_back(cool_reader);

        // Office viewer.
        if (has_office_viewer)
        {
            office_viewer.mutable_extensions().push_back("doc");
            office_viewer.mutable_extensions().push_back("docx");
            office_viewer.mutable_extensions().push_back("xls");
            office_viewer.mutable_extensions().push_back("xlsx");
            office_viewer.mutable_extensions().push_back("ppt");
            office_viewer.mutable_extensions().push_back("pptx");
            DEFAULT_SERVICES.push_back(office_viewer);
        }

        // oar_wrapper based service
        Service oar_wrapper("com.onyx.service.oar_wrapper",
                            "/com/onyx/object/oar_wrapper",
                            "com.onyx.interface.oar_wrapper",
                            OPEN_METHOD,
                            "oar_wrapper");
        oar_wrapper.mutable_extensions().push_back("oar");
        DEFAULT_SERVICES.push_back(oar_wrapper);

        // djvu reader
        Service djvu_wrapper("com.onyx.service.djvu_reader",
                             "/com/onyx/object/djvu_reader",
                             "com.onyx.interface.djvu_reader",
                             OPEN_METHOD,
                             "djvu_reader");
        djvu_wrapper.mutable_extensions().push_back("djv");
        djvu_wrapper.mutable_extensions().push_back("djvu");
        DEFAULT_SERVICES.push_back(djvu_wrapper);

        comic_reader.mutable_extensions().push_back("rar");
        comic_reader.mutable_extensions().push_back("cbr");
        comic_reader.mutable_extensions().push_back("7z");
        comic_reader.mutable_extensions().push_back("cb7");
        comic_reader.mutable_extensions().push_back("cbz");
        comic_reader.mutable_extensions().push_back("zip");
        DEFAULT_SERVICES.push_back(comic_reader);
    }
}

/// Always read all services from database.
void ServiceConfig::loadAllServices(QSqlDatabase &database, Services &services)
{
    QString ext, svr, obj, ifname, method, app;
    services.clear();

    loadDefaultServices();
    services = DEFAULT_SERVICES;

    QSqlQuery query(database);
    query.prepare("SELECT extension, service, object, interface, method, app FROM services");
    query.exec();
    while (query.next())
    {
        int index = 0;
        ext = query.value(index++).toString();
        svr = query.value(index++).toString();
        obj = query.value(index++).toString();
        ifname = query.value(index++).toString();
        method = query.value(index++).toString();
        app = query.value(index).toString();

        // Add to the service list.
        Service service(svr, obj, ifname, method, app);
        service.mutable_extensions().push_back(ext);

        ServicesIter iter;
        // first kill default service for ext
        for(iter = services.begin(); iter != services.end(); ++iter)
        {
            if (iter->extensions().contains(ext))
            {
                iter->mutable_extensions().removeAll(ext);
            }
        }

        for(iter = services.begin(); iter != services.end(); ++iter)
        {
            if (*iter == service)
            {
                iter->mutable_extensions().push_back(ext);
                break;
            }
        }

        if (iter == services.end())
        {
            services.push_back(service);
        }
    }
}

bool ServiceConfig::calibrationService(QSqlDatabase &database, Service &service)
{
    service.mutable_app_name() = "mouse_calibration";
    return true;
}

bool ServiceConfig::metService(QSqlDatabase &, Service &service)
{
    service.mutable_app_name() = "met";
    return true;
}

bool ServiceConfig::musicService(QSqlDatabase &database, Service & service)
{
    // music service.
    static Service music_service("com.onyx.service.music",
                                 "/com/onyx/object/music",
                                 "com.onyx.interface.music",
                                 OPEN_METHOD,
#ifndef BUILD_WITH_TFT
                                 "music_player");
#else
                                 "audio_player");
#endif

    if (music_service.extensions().size() <= 0)
    {
        music_service.mutable_extensions().push_back("mp3");
        music_service.mutable_extensions().push_back("wav");
#ifndef BUILD_WITH_TFT
        music_service.mutable_extensions().push_back("rm");
        music_service.mutable_extensions().push_back("wma");
#endif
    }
    service = music_service;
    return true;
}

bool ServiceConfig::mediaService(QSqlDatabase &, Service & service)
{
    static Service media_service("com.onyx.service.media",
                                 "/com/onyx/object/media",
                                 "com.onyx.interface.media",
                                 OPEN_METHOD,
                                 "onyx_media_player");

    if (media_service.extensions().size() <= 0)
    {
        media_service.mutable_extensions().push_back("mp3");
        media_service.mutable_extensions().push_back("rm");
        media_service.mutable_extensions().push_back("wma");
        media_service.mutable_extensions().push_back("wav");

        media_service.mutable_extensions().push_back("avi");
        media_service.mutable_extensions().push_back("akv");
        media_service.mutable_extensions().push_back("mp4");
        media_service.mutable_extensions().push_back("flv");
        media_service.mutable_extensions().push_back("asf");
        media_service.mutable_extensions().push_back("mkv");
        media_service.mutable_extensions().push_back("mpg");
        media_service.mutable_extensions().push_back("rmvb");
        media_service.mutable_extensions().push_back("3gp");
    }
    service = media_service;
    return true;
}

bool ServiceConfig::noteService(QSqlDatabase &database, Service & service)
{
    service = note_service;
    return true;
}

bool ServiceConfig::networkService(QSqlDatabase &, Service & service)
{
    service = nm_service;
    return true;
}

bool ServiceConfig::webBrowserService(QSqlDatabase &, Service &service)
{
    service = wb_service;
    return true;
}

bool ServiceConfig::DRMService(QSqlDatabase &, Service &service)
{
    service = drm_service;
    return true;
}

bool ServiceConfig::messengerService(QSqlDatabase &, Service &service)
{
    service = messenger_service;
    return true;
}

bool ServiceConfig::writePadService(QSqlDatabase &, Service & service)
{
    service = write_pad_service;
    return true;
}

bool ServiceConfig::dictionaryService(QSqlDatabase &, Service & service)
{
    service = dict_service;
    return true;
}

bool ServiceConfig::rssService(QSqlDatabase&, Service &service)
{
    service = rss_service;
    return true;
}

bool ServiceConfig::news_feeds_Service(QSqlDatabase&, Service &service)
{
    service = newsfeeds_Service;
    return true;
}

bool ServiceConfig::sudokuService(QSqlDatabase&, Service &service)
{
    service = sudoku_service;
    return true;
}

bool ServiceConfig::nabooReaderService(QSqlDatabase &, Service & service)
{
    service = naboo_viewer_service;
    return true;
}

bool ServiceConfig::onyxPdfReaderService(QSqlDatabase &, Service &service)
{
    service = onyx_pdf_reader_service;
    return true;
}

bool ServiceConfig::onyxReaderService(QSqlDatabase &, Service & service)
{
    service = onyx_reader;
    return true;
}

bool ServiceConfig::coolReaderService(QSqlDatabase &, Service & service)
{
    service = cool_reader;
    return true;
}

bool ServiceConfig::htmlReaderService(QSqlDatabase &, Service & service)
{
    service = html_reader;
    return true;
}

bool ServiceConfig::comicReaderService(QSqlDatabase &, Service & service)
{
    service = comic_reader;
    return true;
}

bool ServiceConfig::officeViewerService(QSqlDatabase &, Service & service)
{
    service = office_viewer;
    return true;
}

bool ServiceConfig::hasOfficeViewer()
{
    return QFile::exists("/opt/onyx/arm/bin/office_viewer");
}

bool ServiceConfig::checkService(QSqlDatabase &database, const Service &service)
{
    if (service.extensions().size() <= 0)
    {
        return false;
    }

    QSqlQuery query(database);
    query.prepare( "SELECT extension FROM services where "
                   "extension = ? and "
                   "service = ? and "
                   "object = ? and "
                   "interface = ? and "
                   "app = ? ");
    query.addBindValue(service.extensions().first());
    query.addBindValue(service.service_name());
    query.addBindValue(service.object_path());
    query.addBindValue(service.interface_name());
    query.addBindValue(service.app_name());
    return (query.exec() && query.next());
}

bool ServiceConfig::addService(QSqlDatabase &database, const Service &service)
{
    if (service.extensions().size() <= 0)
    {
        return false;
    }

    QSqlQuery query(database);
    query.prepare( "insert into services "
                   "(extension, service, object, interface, method, app) "
                   " values "
                   "(?, ?, ?, ?, ?, ?)" );
    query.addBindValue(service.extensions().first()),
    query.addBindValue(service.service_name()),
    query.addBindValue(service.object_path()),
    query.addBindValue(service.interface_name()),
    query.addBindValue(service.method());
    query.addBindValue(service.app_name());
    return query.exec();
}

bool ServiceConfig::registerService(QSqlDatabase &database,
                                    const Service &service,
                                    const QString &path)
{
    if (checkService(database, service))
    {
        return false;
    }

    addService(database, service);

    // Need to create dbus file. TODO
    return true;
}

bool ServiceConfig::unRegisterService(QSqlDatabase &database, const Service &service)
{
    QSqlQuery query(database);
    query.prepare( "DELETE FROM services where "
                   "extension = ? and service = ? and "
                   "object = ? and interface = ? and app = ? " );
    query.addBindValue(service.extensions().first());
    query.addBindValue(service.service_name());
    query.addBindValue(service.object_path());
    query.addBindValue(service.interface_name());
    query.addBindValue(service.app_name());
    return query.exec();
}

};

#ifndef BASE_CONFIGURATION_H_
#define BASE_CONFIGURATION_H_

#include "onyx/ui/ui.h"
#include "onyx/cms/content_manager.h"

using namespace cms;

namespace vbf
{

/// Define common tags.
const char * const CONFIG_FONT          = "font";
const char * const CONFIG_FONT_SIZE     = "font_size";
const char * const CONFIG_FONT_FAMILY   = "font_family";
const char * const CONFIG_ENCODING      = "encoding";
const char * const CONFIG_LAST_LOCATION = "last_location";
const char * const CONFIG_FLASH_TYPE    = "flash_type";
const char * const CONFIG_VIEW_PORT     = "view_port";
const char * const CONFIG_PASSWORD      = "password";

const char * const CONFIG_ZOOM_SETTING    = "zoom_setting";
const char * const CONFIG_PAGE_LAYOUT     = "page_layout";
const char * const CONFIG_ROTATE_DEGREE   = "rotate_degree";
const char * const CONFIG_VIEW_X          = "view_x";
const char * const CONFIG_VIEW_Y          = "view_y";
const char * const CONFIG_PAGE_NUMBER     = "page_number";
const char * const CONFIG_FULLSCREEN      = "full_screen";

const char * const CONFIG_PAGING_MODE     = "paging_mode";
const char * const CONFIG_READING_MODE    = "reading_mode";
const char * const CONFIG_DOC_LOCATION    = "document_location";
const char * const CONFIG_FONT_INDEX      = "font_index";
const char * const CONFIG_FONT_RATIO      = "font_ratio";
const char * const CONFIG_SKETCH_COLOR    = "sketch_color";
const char * const CONFIG_SKETCH_SHAPE    = "sketch_shape";
//const char * const CONFIG_NAV_MATRIX      = "nav_matrix";
//const char * const CONFIG_ENV_MATRIX      = "env_matrix";
const char * const CONFIG_NAV_MATRIX_A      = "nav_matrix.a";
const char * const CONFIG_NAV_MATRIX_B      = "nav_matrix.b";
const char * const CONFIG_NAV_MATRIX_C      = "nav_matrix.c";
const char * const CONFIG_NAV_MATRIX_D      = "nav_matrix.d";
const char * const CONFIG_NAV_MATRIX_E      = "nav_matrix.e";
const char * const CONFIG_NAV_MATRIX_F      = "nav_matrix.f";

const char * const CONFIG_ENV_MATRIX_A      = "env_matrix.a";
const char * const CONFIG_ENV_MATRIX_B      = "env_matrix.b";
const char * const CONFIG_ENV_MATRIX_C      = "env_matrix.c";
const char * const CONFIG_ENV_MATRIX_D      = "env_matrix.d";
const char * const CONFIG_ENV_MATRIX_E      = "env_matrix.e";
const char * const CONFIG_ENV_MATRIX_F      = "env_matrix.f";

const char * const CONFIG_IS_COMIC_MODE_ACTIVATED = "comic_mode_activated";

const char * const CONFIG_IMAGE_EMBOLDEN_LEVEL = "image_embolden_level";

const char * const CONFIG_HYPHENATION = "enable_hyphenation";

struct Configuration
{
    QVariantMap options;        ///< Document related options.
    ContentNode info;           ///< Document Metadata.
};


/// Open database connection.
bool openDatabase(const QString &doc_path, cms::ContentManager & db);

/// Load Document options.
bool loadDocumentOptions(cms::ContentManager & db,
                         const QString &doc_path,
                         Configuration & conf);

/// Store Document options.
bool saveDocumentOptions(cms::ContentManager & db,
                         const QString &doc_path,
                         Configuration & conf,
                         bool add_to_history = true);


}

#endif

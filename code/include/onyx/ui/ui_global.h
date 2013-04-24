// Author: John

/// This is the main header file for the UI package. The ui namespace
/// encapsulate the classes we use in our user-facing applications.

#ifndef ONYX_UI_GLOBAL_H__
#define ONYX_UI_GLOBAL_H__

#ifndef _WIN32
#pragma GCC system_header
#endif

#include <Qt/QtCore>
#include <Qt/QtGui>

namespace ui
{

static const int Device_Menu_Key = Qt::Key_Menu;

typedef float ZoomFactor;


enum ViewType
{
    INVALID_VIEW = -1,
    LIST_VIEW = 1,
    DETAILS_VIEW = 2,
    THUMBNAIL_VIEW = 3,
};


enum EditType
{
    INVALID_EDIT = -1,
    EDIT_CUT  = 0,
    EDIT_COPY,
    EDIT_PASTE,
    EDIT_DELETE
};


enum PageLayoutType
{
    INVALID_LAYOUT = -1,
    PAGE_LAYOUT = 10,
    CONTINUOUS_LAYOUT,
    FACE_TO_FACE_LAYOUT,
    SCROLL_LAYOUT,
    REFLOWABLE_LAYOUT,
    THUMBNAIL_LAYOUT
};


enum ReadingToolsType
{
    INVALID_TOOL = -1,
    SEARCH_TOOL = 0,
    RETRIEVE_WORD,
    TOC_VIEW_TOOL,
    ANNOTATION_VIEW_TOOL,
    COPY_CONTENT,
    PASTE_CONTENT,
    NOTES_BACKGROUND_SELECTION,
    EXPORT_SKETCH_DATA,
    INSERT_NOTE,
    REMOVE_NOTE,
    SLIDE_SHOW,
    DISPLAY_HYPERLINKS,
    DICTIONARY_TOOL,
    GOTO_PAGE,
    TEXT_TO_SPEECH,
    SCROLL_PAGE,
    PREVIOUS_VIEW,
    NEXT_VIEW,
    ADD_BOOKMARK,
    DELETE_BOOKMARK,
    SHOW_ALL_BOOKMARKS,
    SET_TOBE_BOOTSPLASH,
    SELECT_ALL,
    SAVE_DOCUMENT,
    UNDO_TOOL,
    REDO_TOOL,
    SAVE_ACCOUNT,
    DISPLAY_ACCOUNT,
    DELETE_ACCOUNT,
    CLEAR_COOKIES,
    AUTO_LOAD_IMAGE,
    CLOCK_TOOL,
    COMIC_MODE,
    IMAGE_EMBOLDEN_TOOL,
    READING_OPTIONS,
    MARGIN_SETTING,
    WEB_BROWSER_MODE,
    KEY_MAPPING,
    DISPLAY_HYPHENATION,
    UNDEFINED_TOOL,
};

enum NetworkType
{
    INVALID_TYPE = -1,
    NETWORK_WIFI = 0,
    NETWORK_WCDMA,
    NETWORK_PROXY
};


enum SpecialZoomSetting
{
    ZOOM_DECREASE    = -9,
    ZOOM_INCREASE    = -8,
    ZOOM_BY_USER     = -7,
    ZOOM_BY_TWO_POINTS = -6,
    ZOOM_TO_PAGE     = -5,
    ZOOM_TO_WIDTH    = -4,
    ZOOM_TO_HEIGHT   = -3,
    ZOOM_SELECTION   = -2,
    ZOOM_HIDE_MARGIN = -1,
    INVALID_ZOOM     = 0,
    ZOOM_MIN         = 10,
    ZOOM_ACTUAL      = 100,
    ZOOM_MAX         = 800
};


enum ViewSettingType
{
    INVALID_MODE = -1,
    PORTRAIT_MODE = 0,
    LANDSCAPE_MODE = 270
};

enum SketchMode
{
    MODE_SKETCHING = 10,
    MODE_ERASING,
    MODE_MERGING, // deprecated
    MODE_UNKNOWN
};

enum SketchColor
{
    INVALID_SKETCH_COLOR = 99,
    SKETCH_COLOR_WHITE = 100,
    SKETCH_COLOR_LIGHT_GRAY,
    SKETCH_COLOR_DARK_GRAY,
    SKETCH_COLOR_BLACK,
    UNKNOWN_SKETCH_COLOR
};

enum SketchShape
{
    INVALID_SKETCH_SHAPE = 199,
    SKETCH_SHAPE_0 = 200,
    SKETCH_SHAPE_1,
    SKETCH_SHAPE_2,
    SKETCH_SHAPE_3,
    SKETCH_SHAPE_4,
    UNKNOWN_SKETCH_SHAPE
};

enum SketchMiscItem
{
    SKETCH_MISC_INVALID_ITEM = 299,
    SKETCH_MISC_PAGE_MODE = 300,
    SKETCH_MISC_PAN_MODE,
    SKETCH_MISC_TOGGLE_ANNOTATION_VISIBLE, // toggle both annot and sketch's visible
    SKETCH_MISC_PDF_MERGE,
};

enum RotateDegree
{
    INVALID_ROTATE_DEGREE = -1,
    ROTATE_0_DEGREE = 0,
    ROTATE_90_DEGREE,
    ROTATE_180_DEGREE,
    ROTATE_270_DEGREE
};

enum SystemAction
{
    INVALID_SYSTEM_ACTION = -1,
    RETURN_TO_LIBRARY = 0,
    SCREEN_UPDATE_TYPE,
    ROTATE_SCREEN,
    MUSIC,
    ABOUT_INFO,
    FULL_SCREEN,
    EXIT_FULL_SCREEN,
    BACKLIGHT_BRIGHTNESS,
    SYSTEM_VOLUME,
    GLOW_LIGHT_CONTROL
};

enum AnnotationMode
{
    INVALID_ANNOTATION_MODE = -1,
    ADD_ANNOTATION = 0,
    DELETE_ANNOTATION,
    DIAPLAY_ALL_ANNOTATIONS,
    EXPORT_ANNOTATIONS,
    EDIT_ANNOTATIONS,
    UNKNOWN_ANNOTATION_MODE
};

enum ImageRenderSetting
{
    INVALID_IMAGE_RENDER_SETTING = -1,
    IMAGE_NEED_DITHER = 1,
    IMAGE_NEED_CONVERT,
    IMAGE_NEED_SMOOTH,
    IMAGE_SETTING_UNKNOWN
};

/// To optimize screen update for eink device.
enum UpdateState
{
    UPDATE_NONE,        ///< Do not receive update request yet.
    UPDATE_INITIAL,     ///< Receive the first update request.
    UPDATE_NORMAL,      ///< After first update request.
};

enum FunctionStatus
{
    FUNC_DISABLE = 0,
    FUNC_NORMAL,
    FUNC_SELECTED
};

enum FunctionID
{
    ID_ZOOM_IN = 0,
    ID_ZOOM_BEST_FIT,
    ID_ZOOM_TO_WIDTH,
    ID_ZOOM_TO_HEIGHT,
    ID_PAN,
    ID_FREE_PEN,
    ID_ADD_ANNOTATION,
    ID_DELETE_ANNOTATION,
    ID_SLIDE_SHOW,
    ID_DISPLAY_HYPERLINKS,
    ID_RETRIEVE_WORD,           // Not Used
    ID_WAIT_DISPLAY_SEARCH_RES, // Not Used
    ID_SKETCHING,
    ID_ERASING,
    ID_COMIC_MODE,
    ID_HAND,
    ID_UNKNOWN
};

enum SpeakStyle
{
    SPEAK_STYLE_INVALID = 0,
    SPEAK_STYLE_CLEAR,
    SPEAK_STYLE_NORMAL,
    SPEAK_STYLE_PLAIN,
    SPEAK_STYLE_VIVID,
};

}  // namespace ui

#endif  // ONYX_UI_GLOBAL_H__

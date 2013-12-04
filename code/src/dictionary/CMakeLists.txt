
ENABLE_QT()

#INCLUDE(FindZLIB)
#INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
#LINK_DIRECTORIES(${ZLIB_LIBRARY_DIRS})


# qstardict backend.
FILE(GLOB QSTARDICT_PLUGIN_HDRS qstardict_plugin/*.h)
FILE(GLOB QSTARDICT_PLUGIN_SRCS qstardict_plugin/*.cpp)

FILE(GLOB HDRS ${ONYXSDK_DIR}/include/onyx/dictionary/*.h)
QT4_WRAP_CPP(MOC_SRCS ${HDRS})

SET(SRCS
  ${HDRS}
  ${QSTARDICT_PLUGIN_HDRS}
  ${QSTARDICT_PLUGIN_SRCS}
  dictionary_manager.cpp
  dict_widget.cpp
  dict_frame.cpp
  onyx_dict_frame.cpp)

# moc
set(MOC_HDRS ${HDRS}  ${QSTARDICT_PLUGIN_HDRS})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HDRS})

# resource
SET(resource_srcs dictionary_images.qrc)
QT4_ADD_RESOURCES(dictionary_ui_res ${resource_srcs})


ADD_LIBRARY(dictionary ${SRCS} ${MOC_SRCS} ${dictionary_ui_res})
TARGET_LINK_LIBRARIES(dictionary onyx_ui ${ADD_LIB} z)

########### Unit Tests #####################
INCLUDE_DIRECTORIES(.)
ADD_EXECUTABLE(dictionary_test unittests/libdict_test.cpp ${SRCS} ${MOC_SRCS})
TARGET_LINK_LIBRARIES(dictionary_test onyx_sys onyx_screen
        onyx_ui dictionary tts sound
        ${QT_LIBRARIES}
        ${ADD_LIB} z)
SET_TARGET_PROPERTIES(dictionary_test PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})


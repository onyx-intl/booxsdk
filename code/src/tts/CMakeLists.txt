
ENABLE_QT()

IF(UNIX)
    ADD_DEFINITIONS(-DPLATFORM_POSIX)
ELSE (UNIX)
    ADD_DEFINITIONS(-DPLATFORM_WINDOWS)
ENDIF (UNIX)
FILE(GLOB HDRS ${ONYXSDK_DIR}/include/onyx/tts/*.h)
# aisound
include_directories(aisound)
link_directories(${CMAKE_SOURCE_DIR}/lib/tts/aisound)

FILE(GLOB AISOUND_HDRS aisound/*.h)
FILE(GLOB AISOUND_SRCS aisound/*.cpp)


# espeak
include_directories(espeak)
FILE(GLOB ESPEAK_HDRS espeak/src/*.h)
SET(ESPEAK_SRCS
    espeak/src/speak_lib.cpp
    espeak/src/compiledict.cpp
    espeak/src/dictionary.cpp
    espeak/src/intonation.cpp
    espeak/src/readclause.cpp
    espeak/src/setlengths.cpp
    espeak/src/numbers.cpp
    espeak/src/synth_mbrola.cpp
    espeak/src/synthdata.cpp
    espeak/src/synthesize.cpp
    espeak/src/translate.cpp
    espeak/src/tr_languages.cpp
    espeak/src/voices.cpp
    espeak/src/wavegen.cpp
    espeak/src/phonemelist.cpp
    espeak/src/espeak_command.cpp
    espeak/src/event.cpp
    espeak/src/fifo.cpp
    espeak/src/debug.cpp
    espeak/src/klatt.cpp
)

#SET(SRCS ${ESPEAK_SRCS})
#SET(SRCS ${AISOUND_SRCS})
#SET(SRCS ${EJ_SRCS})


# acapela
include_directories(backend)
link_directories(${CMAKE_SOURCE_DIR}/lib/tts/acapela/lib)

FILE(GLOB ACAPELA_HDRS acapela/*.h)
FILE(GLOB ACAPELA_SRCS acapela/*.cpp)


# source files.

SET(SRCS ${HDRS} tts.cpp tts_widget.cpp )


#SET(SRCS ${AISOUND_SRCS} ${AISOUND_HDRS} ${HDRS} tts.cpp tts_widget.cpp)
SET(MOC_HDRS ${HDRS})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HDRS})



# resource
SET(resource_srcs tts_images.qrc)
QT4_ADD_RESOURCES(tts_ui_res ${resource_srcs})

ADD_LIBRARY(tts ${SRCS} ${MOC_SRCS} ${tts_ui_res})

TARGET_LINK_LIBRARIES(tts sound  onyx_ui
   ${QT_LIBRARIES}
   ${ADD_LIB} )

INSTALL(FILES ${ESPEAK_HDRS} ${HDRS} DESTINATION include)

# unittest
FILE(GLOB UNITTEST_SRCS unittest/unittest.cpp)
ADD_EXECUTABLE(tts_unittest ${UNITTEST_SRCS})
if(BUILD_FOR_ARM)
TARGET_LINK_LIBRARIES(tts_unittest tts
   ${QT_LIBRARIES}
   ${ADD_LIB}
)
else(BUILD_FOR_ARM)
TARGET_LINK_LIBRARIES(tts_unittest tts
   ${QT_LIBRARIES}
   ${ADD_LIB}
)
endif(BUILD_FOR_ARM)
SET_TARGET_PROPERTIES(tts_unittest PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})

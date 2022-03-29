if (APPLE)
    set(PLATFORM_SOURCES midway_osx.cpp)
    set(PLATFORM_LIBS "-framework CoreMIDI" "-framework CoreFoundation")
elseif(WIN32)
    set(PLATFORM_SOURCES midway_win32.cpp)
    set(PLATFORM_LIBS winmm)
else()
    message("Unsupported platform - WIN32 or MacOS only")
endif()


add_library(Midway STATIC include/midway/midway.h ${PLATFORM_SOURCES} midway.cpp)
target_include_directories(Midway INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Midway PUBLIC ${PLATFORM_LIBS})
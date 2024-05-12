cmake_minimum_required(VERSION 3.16)
if(APPLE)
#set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE) # comment out if you dont want a univeral binary
endif()
project(stickersolve-gui)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_FLAGS_DEBUG "-g -O3 -Wall -pg") # gprof stickersolve-gui gmon.out > gmon.txt
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if(APPLE)
find_package(Qt5 REQUIRED Core Widgets Gui PATHS /usr/local/opt/qt@5) # update path to qt cmake files here for MacOS
link_directories(/usr/local/opt/boost/lib/) # update path to boost cmake files here for MacOS
else()
set(Qt5_DIR ./vendor/usr/lib/x86_64-linux-gnu/cmake/Qt5)

find_package(Qt5 REQUIRED Core Widgets Gui PATHS ./vendor/lib/cmake)

set(CMAKE_BUILD_WITH_INSTALL_RPATH on)
set(CMAKE_INSTALL_RPATH "./vendor/lib" "./lib")
link_directories(${PROJECT_SOURCE_DIR}/vendor/lib)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include/, ${PROJECT_SOURCE_DIR}/vendor/include/) 


file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*" "${PROJECT_SOURCE_DIR}/include/*")
file(GLOB_RECURSE VENDOR_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/vendor/src/*" "${PROJECT_SOURCE_DIR}/vendor/include/*")


if(WIN32)
  add_executable(${PROJECT_NAME} WIN32 ${APP_SOURCES} ${VENDOR_SOURCES})
  target_compile_options(${PROJECT_NAME} PRIVATE)
  # set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
  target_link_libraries(${PROJECT_NAME} 
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    ${ADDITIONAL_LIBRARIES}
  )
elseif(APPLE)
  add_executable(${PROJECT_NAME} ${APP_SOURCES} ${VENDOR_SOURCES})
  target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
  target_link_libraries(${PROJECT_NAME} 
    pthread
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    -lboost_system
    -lboost_iostreams
    -lboost_filesystem
  )
  set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER com.github.TAR-ALEX.${PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION "1.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.1"
  )
else()
  add_executable(${PROJECT_NAME} ${APP_SOURCES} ${VENDOR_SOURCES})
  target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
  target_link_libraries(${PROJECT_NAME} 
    pthread
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    -l:libboost_system.a 
    -l:libboost_iostreams.a 
    -l:libboost_filesystem.a
  )
endif()
cmake_minimum_required(VERSION 3.5)

project(VisCR VERSION 1.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_OUTPUT_EXTENSION .obj)

set(CMAKE_PREFIX_PATH %{Qt:QT_INSTALL_PREFIX})
set(VS_Version 14.41.34120)
# set(CMAKE_C_COMPILER "D:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.43.34808/bin/HostX64/x64/cl.exe")
# set(CMAKE_CXX_COMPILER "D:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.43.34808/bin/HostX64/x64/cl.exe")

# set(CUDA_HOST_COMPILER "D:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.43.34808/bin/HostX64/x64/cl.exe")
set(CUDA_TOOLKIT_ROOT_DIR "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.7")
set(CUDA_USE_STATIC_CUDA_RUNTIME ON)


set(QT_CMAKE_DIR D:/Qt6/6.7.0/msvc2019_64/lib/cmake/)
set(QT_DIR ${QT_CMAKE_DIR}Qt6)
set(QT_QMAKE_EXECUTABLE D:/Qt6/6.7.0/msvc2019_64/bin/qmake.exe)
set(WINDEPLOYQT_EXECUTABLE D:/Qt6/6.7.0/msvc2019_64/bin/windeployqt.exe)
set(Qt6Charts_DIR ${QT_CMAKE_DIR}Qt6Charts)
set(Qt6CoreTools_DIR ${QT_CMAKE_DIR}Qt6CoreTools)
set(Qt6Core_DIR ${QT_CMAKE_DIR}Qt6Core)
set(Qt6EntryPointPrivate_DIR ${QT_CMAKE_DIR}Qt6EntryPointPrivate)
set(Qt6GuiTools_DIR ${QT_CMAKE_DIR}Qt6GuiTools)
set(Qt6Gui_DIR ${QT_CMAKE_DIR}Qt6Gui)
set(Qt6WidgetsTools_DIR ${QT_CMAKE_DIR}Qt6WidgetsTools)
set(Qt6_DIR ${QT_CMAKE_DIR}Qt6)
set(Qt6Widgets_DIR ${QT_CMAKE_DIR}Qt6Widgets)
set(Qt6OpenGL_DIR ${QT_CMAKE_DIR}Qt6OpenGL)
set(Qt6OpenGLWidgets_DIR ${QT_CMAKE_DIR}Qt6OpenGLWidgets)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(Qt6Charts)
find_package(Qt6 COMPONENTS Widgets OpenGL OpenGLWidgets REQUIRED)

set(PROJECT_INCLUDES 
    ../includes/mainwindow.h
    ../includes/gl_widget.h
    ../includes/camera.h
    ../includes/cons.h
    ../includes/connect.h
)

set(PROJECT_CPP
    ../main.cpp
    ../mainwindow.cpp
    ../camera.cpp
    ../connect.cpp
    ../gl_widget.cpp
)
set(PROJECT_ICON
    ../pics/icon.qrc
    ../pics/icon.rc
)

set(PROJECT_SOURCES
    ${PROJECT_INCLUDES}
    ${PROJECT_CPP}
    ${PROJECT_ICON}
    ../mainwindow.ui 
)

# INCLUDE_DIRECTORIES(D:/Documents/VisCR/includes)

qt_add_executable(VisCR MANUAL_FINALIZATION ${PROJECT_SOURCES})

target_link_libraries(VisCR PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(VisCR PRIVATE Qt${QT_VERSION_MAJOR}::Charts)
target_link_libraries(VisCR PRIVATE Qt6::OpenGLWidgets ${OPENGL_LIBRARIES})
target_link_libraries(VisCR PRIVATE Qt::OpenGLWidgets)
target_link_libraries(VisCR PRIVATE "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22000.0\\um\\x64\\OpenGL32.Lib")
target_link_libraries(VisCR PRIVATE "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22000.0\\um\\x64\\GlU32.Lib")


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.VisCR)
endif()
set_target_properties(VisCR PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS VisCR
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(VisCR)
endif()

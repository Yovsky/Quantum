cmake_minimum_required(VERSION 3.16)
project(Quantum VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Network
    Qml
    Quick
    QuickControls2
    Sql
    Svg
    Widgets
)

qt_standard_project_setup()

qt_add_executable(Quantum WIN32 MACOSX_BUNDLE
    src/backend/backend.cpp src/backend/backend.h
    src/backend/downloadmodel.cpp src/backend/downloadmodel.h
    src/backend/filenamehandler.cpp src/backend/filenamehandler.h
    src/core/databasemanager.cpp src/core/databasemanager.h
    src/core/downloader.cpp src/core/downloader.h
    src/core/downloadworker.cpp src/core/downloadworker.h
    src/core/main.cpp
    src/models/downloadcategories.h
    src/models/downloadstatus.h
)
qt_add_qml_module(Quantum
    URI Quantum
    VERSION ${PROJECT_VERSION}
    QML_FILES
        qml/Main.qml
        qml/pages/MainPage.qml
        qml/components/WindowButton.qml
        qml/components/UiButton.qml
        qml/components/Counter.qml
        qml/components/CategoryTab.qml
        qml/UrlWindow.qml
        qml/components/DialogInputBox.qml
        qml/js/Helper.js
        qml/components/DownloadCard.qml
        qml/components/ProgressBar.qml
        qml/components/DropDown.qml
        qml/pages/DownloadsPage.qml
    RESOURCES
        qml/assets/icons/maximize.png
        qml/assets/icons/minimize.png
        qml/assets/icons/restore.png
        qml/assets/fonts/Lexend.ttf
        qml/assets/icon.svg
        qml/assets/icons/close.svg
        qml/assets/icons/document.svg
        qml/assets/icons/down-arrow.svg
        qml/assets/icons/download.svg
        qml/assets/icons/folder.svg
        qml/assets/icons/left-arrow.svg
        qml/assets/icons/music.svg
        qml/assets/icons/pause.svg
        qml/assets/icons/play.svg
        qml/assets/icons/program.svg
        qml/assets/icons/setting.svg
        qml/assets/icons/video.svg
        qml/assets/icons/close.png
        qml/assets/icons/compressed.svg
    NO_RESOURCE_TARGET_PATH
)

# Avoid collision between executable name and QML module directory (QTBUG-98568).
if(NOT WIN32 AND NOT APPLE)
    set_property(TARGET Quantum PROPERTY OUTPUT_NAME Quantum_app)
endif()

target_link_libraries(Quantum PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Sql
    Qt6::Svg
    Qt6::Widgets
    Shell32
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(Quantum PRIVATE
        Qt::Widgets
    )
endif()

install(TARGETS Quantum
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_qml_app_script(
    TARGET Quantum
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
    MACOS_BUNDLE_POST_BUILD
)

install(SCRIPT ${deploy_script})

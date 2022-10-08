cmake_minimum_required(VERSION 3.1)

project(feidesk LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

find_package(Qt5 COMPONENTS Core Widgets Network RemoteObjects REQUIRED)

set(SOURCE_FILES
    main.cpp
    richedit/qkxtextobject.cpp
    richedit/qkxtextcursor.cpp
    richedit/qkxmoviepool.cpp
    richedit/qkxtextbrowser.cpp
    qkxutils.cpp
    qkxfirewall.cpp
    qkxsettingex.cpp
    qkxmenu.cpp
    qkxbuttonassist.cpp
    qkxiconlabelbutton.cpp
    qkxpowerapplication.cpp
    qkxpowernetserver.cpp
    qkxoptionform.cpp
    qkxmydeviceform.cpp
    qkxconnectionform.cpp
    qkxpowermainwindow.cpp
    qkxsimplemainwindow.cpp
    qkxpushbutton.cpp
    qkxdesktoptoolform.cpp
    qkxdesktopplayform.cpp
    qkxdesktopplaystatusform.cpp
    qkxdesktopplaywindow.cpp
    qkxfiletransferwindow.cpp
    qkxfiletransferstatusform.cpp
    qkxloadingwidget.cpp
    qkxloadingbarwidget.cpp
    qkxdevicetreemodel.cpp
    qkxlocalabilitymodel.cpp
    qkxremoteabilitymodel.cpp
    qkxservicemanager.cpp
    qkxiconframepaint.cpp
    qkxtcpftprequest.cpp
    qkxserverdialog.cpp
    qkxminiworkspacewidget.cpp
    qkxlanguagedialog.cpp
    qkxauthorizeconfirmdialog.cpp
    qkxusercreatedialog.cpp
    qkxaboutdialog.cpp
    qkxminisessionitem.cpp
    qkxdesktopftprequest.cpp
    qkxdesktopftpwidget.cpp
    qkxtermwindow.cpp
    qkxtermwidgetex.cpp
    qkxtermstatusform.cpp
    qkxchatwindow.cpp
    qkxchatstatusform.cpp
    qkxchatinteraction.cpp
    qkxoptionsdialog.cpp
)

set(HEADER_FILES
    richedit/qkxtextobject.h
    richedit/qkxtextcursor.h
    richedit/qkxmoviepool.h
    richedit/qkxtextbrowser.h
    version.h
    qkxdeskdef.h
    qkxutils.h
    qkxmodel_share.h
    qkxfirewall.h
    qkxsettingex.h
    qkxmenu.h
    qkxbuttonassist.h
    qkxiconlabelbutton.h
    qkxpowerapplication.h
    qkxpowernetserver.h
    qkxoptionform.h
    qkxmydeviceform.h
    qkxconnectionform.h
    qkxpowermainwindow.h
    qkxsimplemainwindow.h
    qkxpushbutton.h
    qkxdesktopplayform.h
    qkxdesktopplaystatusform.h
    qkxdesktoptoolform.h
    qkxdesktopplaywindow.h
    qkxfiletransferwindow.h
    qkxfiletransferstatusform.h
    qkxtcpftprequest.h
    qkxloadingwidget.h
    qkxloadingbarwidget.h
    qkxdevicetreemodel.h
    qkxlocalabilitymodel.h
    qkxremoteabilitymodel.h
    qkxservicemanager.h
    qkxiconframepaint.h
    qkxserverdialog.h
    qkxminiworkspacewidget.h
    qkxlanguagedialog.h
    qkxauthorizeconfirmdialog.h
    qkxusercreatedialog.h
    qkxaboutdialog.h
    qkxminisessionitem.h
    qkxdesktopftprequest.h
    qkxdesktopftpwidget.h
    qkxtermwindow.h
    qkxtermwidgetex.h
    qkxtermstatusform.h
    qkxchatwindow.h
    qkxchatstatusform.h
    qkxchatinteraction.h
    qkxoptionsdialog.h
)

set(OTHER_FILES
    feidesk.qrc
    qkxoptionform.ui
    qkxmydeviceform.ui
    qkxconnectionform.ui
    qkxpowermainwindow.ui
    qkxsimplemainwindow.ui
    qkxdesktoptoolform.ui
    qkxdesktopplaywindow.ui
    qkxdesktopplaystatusform.ui
    qkxfiletransferstatusform.ui
    qkxserverdialog.ui
    qkxminiworkspacewidget.ui
    qkxlanguagedialog.ui
    qkxauthorizeconfirmdialog.ui
    qkxusercreatedialog.ui
    qkxaboutdialog.ui
    qkxminisessionitem.ui
    qkxdesktopftpwidget.ui
    qkxtermwindow.ui
    qkxtermstatusform.ui
    qkxchatwindow.ui
    qkxchatstatusform.ui
    qkxoptionsdialog.ui
)

include_directories(${SERVICE_ROOT_DIR}
    ${LIBQTSERVICE_ROOT_DIR}/include
    ${ZLIB_ROOT_DIR}/include
    ${LIBYUV_ROOT_DIR}/include
    ${KXVNC_ROOT_DIR}
    ${KXNET_ROOT_DIR}
    ${KXFTP_ROOT_DIR}
    ${KXCAP_ROOT_DIR}
    ${KXTERM_ROOT_DIR}
    ${KXPTY_ROOT_DIR}
    ${AES_ROOT_DIR}
    ${KXUTIL_ROOT_DIR}
)

link_directories(${ZLIB_ROOT_DIR}/lib
    ${LIBQTSERVICE_ROOT_DIR}/lib
    ${LIBYUV_ROOT_DIR}/lib
)

link_libraries(kxvnc kxnet kxcap kxterm kxpty kxftp kxutil)
if(WIN32)
    set(SOURCE_FILES ${SOURCE_FILES}
        qkxutils_win.cpp
    )

    set(OTHER_FILES ${OTHER_FILES}
        feidesk.rc
    )
elseif(APPLE)
    message("APPLE Here")
    set(SOURCE_FILES ${SOURCE_FILES}
        qkxutils_unix.cpp
    )
else()
    message("other")
    set(SOURCE_FILES ${SOURCE_FILES}
        qkxutils_unix.cpp
    )
endif()

if(WIN32)
    message("window system.")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        link_libraries(qtserviced)
    else()
        link_libraries(qtservice)
    endif()
    add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES} ${HEADER_FILES} ${OTHER_FILES})
elseif(APPLE)
    message("macosx system")
    # And this part tells CMake where to find and install the file itself
    set(PROGRAM_VERSION "1.0" CACHE STRINGS "you can modify this value by command : cmake -DPROGRAM_VERSION=\"xx\"")
    set(ICNS_NAME  "feidesk128x128.icns")
    set(myApp_ICON ${CMAKE_CURRENT_SOURCE_DIR}/resource/${ICNS_NAME})
    set_source_files_properties(${myApp_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${myApp_ICON} ${SOURCE_FILES} ${HEADER_FILES} ${OTHER_FILES})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUNDLE True
        MACOSX_BUNDLE_INFO_STRING "FeiDesk Remote Assistant"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.aoyiduo.feidesk"
        MACOSX_BUNDLE_ICON_FILE "${ICNS_NAME}"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROGRAM_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROGRAM_VERSION}
        MACOSX_BUNDLE_BUNDLE_NAME "FeiDesk"
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resource/template.plist.in
        LINK_FLAGS "-sectcreate __CGPreLoginApp __cgpreloginapp /dev/null"
    )
else()
    message("linux system.")
    add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${OTHER_FILES})
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Network Qt5::RemoteObjects)


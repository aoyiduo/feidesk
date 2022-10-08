cmake_minimum_required(VERSION 3.1)

project(kxpty LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    add_definitions(-DKXPTY_LIBRARY)
elseif(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    add_definitions(-DKXPTY_LIBRARY -DHAVE_PTSNAME=1 -DTIOCGPTN=1 -DPTM_DEVICE=1 -DHAVE_LOGIN=1 -DHAVE_SYS_TIME_H=1)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    add_definitions(-DKXPTY_LIBRARY -DHAVE_POSIX_OPENPT=1 -DHAVE_PTSNAME=1 -DTIOCGPTN=1)
endif()

find_package(Qt5 COMPONENTS Core Network Widgets)

set(SOURCE_FILES
    qkxpty.cpp
    qkxptyserver.cpp
    qkxptyserverprivate.cpp
    qkxptyclient.cpp
    qkxptyrequest.cpp
    qkxptyrequestprivate.cpp
)

set(HEADER_FILES    
    qkxpty_private.h
    qkxpty_share.h
    qkxpty.h
    qkxptyserver.h
    qkxptyserverprivate.h
    qkxptyclient.h
    qkxptyrequest.h
    qkxptyrequestprivate.h
)

set(OTHER_FILES
)

if(WIN32)
    set(SOURCE_FILES ${SOURCE_FILES}
        qkxptyprocess_win.cpp
    )
    set(HEADER_FILES ${HEADER_FILES}
        qkxptyprocess_win.h
    )
else()
    set(SOURCE_FILES ${SOURCE_FILES}
        kpty.cpp
        kptydevice.cpp
        qkxptyrequest.cpp
        qkxptyclient.cpp
        qkxptyprocess_unix.cpp
    )
    set(HEADER_FILES ${HEADER_FILES}
        kpty.h
        kptydevice.h
        qkxptyrequest.h
        qkxptyclient.h
        qkxptyprocess_unix.h
    )
endif()

include_directories(
    ${KXUTIL_ROOT_DIR}
)
link_libraries(kxutil)

if(WIN32)
    link_libraries(ws2_32)
elseif(APPLE)
    message("APPLE Here")
else()
    message("other")
endif()

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES} ${OTHER_FILES})
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network Qt5::Widgets)

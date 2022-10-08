cmake_minimum_required(VERSION 3.1)

project(FeiDeskAll)

macro(qt5_generate_myrepc outfiles infile outputtype)
    # get include dirs and flags
    get_filename_component(abs_infile ${infile} ABSOLUTE)
    get_filename_component(infile_name "${infile}" NAME)
    string(REPLACE ".rep" "" _infile_base ${infile_name})
    if(${outputtype} STREQUAL "SOURCE")
        set(_outfile_base "rep_${_infile_base}_source")
        set(_repc_args -o source)
    elseif(${outputtype} STREQUAL "MERGED")
            set(_outfile_base "rep_${_infile_base}_merged")
            set(_repc_args -o merged)
    else()
        set(_outfile_base "rep_${_infile_base}_replica")
        set(_repc_args -o replica)
    endif()
    set(_outfile_header "${CMAKE_CURRENT_BINARY_DIR}/${_outfile_base}.h")
    add_custom_command(OUTPUT ${_outfile_header}
        DEPENDS ${abs_infile}
        COMMAND ${Qt5RemoteObjects_REPC_EXECUTABLE} ${abs_infile} ${_repc_args} ${_outfile_header}
        VERBATIM)
    set_source_files_properties(${_outfile_header} PROPERTIES GENERATED TRUE)

    qt5_get_moc_flags(_moc_flags)
    # Make sure we get the compiler flags from the Qt5::RemoteObjects target (for includes)
    # (code adapted from QT5_GET_MOC_FLAGS)
    foreach(_current ${Qt5RemoteObjects_INCLUDE_DIRS})
        if("${_current}" MATCHES "\\.framework/?$")
            string(REGEX REPLACE "/[^/]+\\.framework" "" framework_path "${_current}")
            set(_moc_flags ${_moc_flags} "-F${framework_path}")
        else()
            set(_moc_flags ${_moc_flags} "-I${_current}")
        endif()
    endforeach()

    set(_moc_outfile "${CMAKE_CURRENT_BINARY_DIR}/moc_${_outfile_base}.cpp")
    qt5_create_moc_command(${_outfile_header} ${_moc_outfile} "${_moc_flags}" "" "" "")
    list(APPEND ${outfiles} "${_outfile_header}" ${_moc_outfile})
endmacro()

#
# repc timemodel.rep -o merged rep_timemodel_merged.h
# qt5_generate_myrepc to fix qt5_generate_repc bug.
# orignal source: xxx/remoteobjects/Qt5RemoteObjectsMacros.cmake
# example: qt5_generate_myrepc(HEADER_FILES ${REPC_FILES} MERGED)

SET(CMAKE_EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_EXECUTABLE_OUTPUT_PATH_DEBUG ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_EXECUTABLE_OUTPUT_PATH_RELEASE ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_PATH_DEBUG ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_PATH_RELEASE ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_DEBUG_POSTFIX "d")

set(SERVICE_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/qtservice)
set(AES_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/qtaes)
set(LIBEVENT_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/libevent-2.1.12)
set(QTDISPATCHER_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/qtdispatcher)
set(KXVNC_ROOT_DIR ${PROJECT_SOURCE_DIR}/kxvnc)
set(KXNET_ROOT_DIR ${PROJECT_SOURCE_DIR}/kxnet)
set(KXTERM_ROOT_DIR ${PROJECT_SOURCE_DIR}/kxterm)
set(KXPTY_ROOT_DIR ${PROJECT_SOURCE_DIR}/kxpty)
set(KXCAP_ROOT_DIR ${PROJECT_SOURCE_DIR}/kxcap)
set(KXFTP_ROOT_DIR ${PROJECT_SOURCE_DIR}/kxftp)
set(KXUTIL_ROOT_DIR ${PROJECT_SOURCE_DIR}/kxutil)
set(MODEM_ROOT_DIR ${PROJECT_SOURCE_DIR}/modem)


if(WIN32)
    set(OPENSSL_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/openssl/winx)
    set(ZLIB_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/zlib/winx)
    set(LIBOPENH264_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/openh264/winx)
    set(LIBPORTAUDIO_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/portaudio/winx)
    set(LIBOPUS_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/opus/winx)
    set(LIBJPEG_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/jpeg/winx)
    set(LIBYUV_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/libyuv/winx)
    set(LIBQTSERVICE_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/qtservice/winx)
    file(GLOB MY_COPY_FILES ${OPENSSL_ROOT_DIR}/bin/*.dll)
    file(GLOB MY_COPY_FILES ${MY_COPY_FILES} ${LIBQTSERVICE_ROOT_DIR}/bin/*.dll)
else()
    set(OPENSSL_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/openssl/unix)
    set(ZLIB_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/zlib/unix)
    set(LIBOPENH264_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/openh264/unix)
    set(LIBPORTAUDIO_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/portaudio/unix)
    set(LIBOPUS_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/opus/unix)
    set(LIBJPEG_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/jpeg/unix)
    set(LIBYUV_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/libyuv/unix)
    set(LIBQTSERVICE_ROOT_DIR ${PROJECT_SOURCE_DIR}/third/qtservice/unix)
    file(GLOB MY_COPY_FILES ${OPENSSL_ROOT_DIR}/lib/lib*.dylib)
    file(GLOB MY_COPY_FILES ${MY_COPY_FILES} ${OPENSSL_ROOT_DIR}/lib/lib*.so*)
endif()
file(COPY ${MY_COPY_FILES} DESTINATION ${PROJECT_SOURCE_DIR}/bin)


message("copy file list:${MY_COPY_FILES}")
message("root path:${PROJECT_SOURCE_DIR}")

add_subdirectory(kxutil)
add_subdirectory(kxftp)
add_subdirectory(kxcap)
add_subdirectory(kxnet)
add_subdirectory(kxvnc)
add_subdirectory(kxterm)
add_subdirectory(kxpty)
add_subdirectory(feidesk)



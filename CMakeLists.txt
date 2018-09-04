
cmake_minimum_required(VERSION 3.10)

if(NOT UTF8PROC_ROOT)
  message(SEND_ERROR "UTF8PROC_ROOT Must be defined")
endif()
file(TO_CMAKE_PATH "${UTF8PROC_ROOT}/include" UTF8PROC_HEADER)
# TODO: Make this work for .so objects on linux/os x
file(TO_CMAKE_PATH "${UTF8PROC_ROOT}/lib/utf8proc_static.lib" UTF8PROC_LIB)

project(tsbl_language)

#link_directories(${UTF8PROC_LIB})
add_library("utf8proc" STATIC IMPORTED)
set_property(TARGET "utf8proc" PROPERTY IMPORTED_LOCATION "${UTF8PROC_LIB}")

set(EXEC_NAME "tsbl")
set(LIB_NAME "tsbl_static")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory("./source")
add_subdirectory("./include")

source_group("Source Files" FILES ./source/CMakeLists.txt)
source_group("Source Files\\tsbl" FILES ${SOURCE_TSBL})
source_group("Source Files\\repl" FILES ${SOURCE_REPL})

source_group("Header Files" FILES ./include/CMakeLists.txt)
source_group("Header Files\\tsbl" FILES ${INCLUDE_TSBL})

add_library(${LIB_NAME} STATIC ${SOURCE_LIB} ${INCLUDE_LIB})
target_include_directories(${LIB_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${UTF8PROC_HEADER}
)
target_link_libraries(${LIB_NAME}
  PUBLIC
    utf8proc
)
target_compile_features(${LIB_NAME} PRIVATE cxx_std_17)

add_executable(${EXEC_NAME} ${SOURCE_REPL})
target_include_directories(${EXEC_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${UTF8PROC_HEADER}
)
target_link_libraries(${EXEC_NAME}
  PUBLIC
    ${LIB_NAME}
    utf8proc
)
target_compile_features(${EXEC_NAME} PRIVATE cxx_std_17)

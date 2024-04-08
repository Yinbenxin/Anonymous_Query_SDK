if (is_arm64_prefix)
    set(main "pir-java-arm64")
elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "arm64")
    set(main "pir-java-arm64")
elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    set(main "pir-java-x64")
else ()
    set(main "pir-java")
endif ()

set(PACKAGE_NAME "com.pir")

cmake_policy(SET CMP0078 NEW)
cmake_policy(SET CMP0086 NEW)

set(CMAKE_CXX_STANDARD 14)


set(CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS "")
find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})
find_package(SWIG REQUIRED)
include(UseSWIG)
set(UseSWIG_TARGET_NAME_PREFERENCE STANDARD)
file(GLOB_RECURSE SRCS *.i)
set(CMAKE_SWIG_OUTDIR ${CMAKE_SOURCE_DIR}/cjava)
#set(CMAKE_SWIG_FLAGS -package ${PACKAGE_NAME} -go -cgo -gccgo -intgosize 64)
set(CMAKE_SWIG_FLAGS -package ${PACKAGE_NAME} -java)
set_property(SOURCE ${SRCS} PROPERTY CPLUSPLUS ON)

# 生成Go绑定
swig_add_library(${main} LANGUAGE java SOURCES ${SRCS})

target_link_libraries(${main} pir faker)

if (APPLE)
    target_link_libraries(${main} "-undefined dynamic_lookup")
endif ()
# 指定输出目录
set_target_properties(${main} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/cjava
)
#set(CMAKE_SWIG_OUTDIR ${CMAKE_BINARY_DIR}/go)
if (WIN32)
    set_target_properties(${main} PROPERTIES LINK_FLAGS "-shared")
else ()
    set_target_properties(${main} PROPERTIES LINK_FLAGS "-dynamiclib")
endif ()


if (WIN32)
    set(main_suffix ".dll")
    set_target_properties(${main} PROPERTIES OUTPUT_NAME ${main} SUFFIX ${main_suffix})
    set(main_fullname "${main}${main_suffix}")
elseif (APPLE)
    set(main_suffix ".dylib")
    set_target_properties(${main} PROPERTIES OUTPUT_NAME ${main} PREFIX "lib" SUFFIX ${main_suffix})
    set(main_fullname "lib${main}${main_suffix}")
else (UNIX)
    set(main_suffix ".so")
    set_target_properties(${main} PROPERTIES OUTPUT_NAME ${main} PREFIX "lib" SUFFIX ${main_suffix})
    set(main_fullname "lib${main}${main_suffix}")
endif ()


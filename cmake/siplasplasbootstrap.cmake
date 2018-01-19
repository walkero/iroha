set(SIPLASPLAS_ROOT $ENV{SIPLASPLAS_ROOT})
mark_as_advanced(SIPLASPLAS_ROOT)

set(LLVM_VERSION $ENV{LLVM_VERSION})
mark_as_advanced(LLVM_VERSION)

set(SIPLASPLAS_LIBCLANG_INCLUDE_DIR $ENV{SIPLASPLAS_LIBCLANG_INCLUDE_DIR})
mark_as_advanced(SIPLASPLAS_LIBCLANG_INCLUDE_DIR)

set(SIPLASPLAS_LIBCLANG_LIBRARY $ENV{SIPLASPLAS_LIBCLANG_LIBRARY})
mark_as_advanced(SIPLASPLAS_LIBCLANG_LIBRARY)

set(SIPLASPLAS_LIBRARIES_STATIC $ENV{SIPLASPLAS_LIBRARIES_STATIC})
mark_as_advanced(SIPLASPLAS_LIBRARIES_STATIC)

find_program(PYTHON_PIP_EXECUTABLE  pip2)
mark_as_advanced(PYTHON_PIP_EXECUTABLE)

set(SIPLASPLAS_LIBCLANG_VERSION   ${LLVM_VERSION}.0) # libclang version
set(SIPLASPLAS_DOWNLOAD_LIBCLANG  OFF)


if(EXISTS "${SIPLASPLAS_ROOT}")
    list(APPEND CMAKE_MODULE_PATH "${SIPLASPLAS_ROOT}/cmake")
    include(siplasplas)      # ${SIPLASPLAS_ROOT}/cmake/siplasplas.cmake
    find_package(Siplasplas) # ${SIPLASPLAS_ROOT}/cmake/FindSiplasplas.cmake
    # these targets will be defined:
    #    siplasplas-logger
    #    siplasplas-utility     DEPENDS siplasplas-logger
    #    siplasplas-allocator   DEPENDS siplasplas-utility
    #    siplasplas-typeerasure DEPENDS siplasplas-utility
    #    siplasplas-reflection-common  HEADER_ONLY
    #    siplasplas-reflection-static  HEADER_ONLY DEPENDS siplasplas-utility
    #    siplasplas-reflection-dynamic DEPENDS siplasplas-reflection-static siplasplas-typeerasure
    #    siplasplas-signals            DEPENDS siplasplas-utility siplasplas-typeerasure
    #    siplasplas-fswatch            DEPENDS siplasplas-reflection-static
    #    siplasplas-cmake              DEPENDS siplasplas-signals siplasplas-reflection-static
else()
    message(FATAL_ERROR "The siplasplas root directory (${SIPLASPLAS_ROOT}) does not exist")
endif()

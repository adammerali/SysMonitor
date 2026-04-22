# FindADLX.cmake
# Looks for AMD ADLX SDK headers and library in third_party/adlx/.
# To use: download from https://github.com/GPUOpen-LibrariesAndSDKs/ADLX/releases
# and place SDK/Include/* in third_party/adlx/include/
# and SDK/Lib/x64/amd_adlx.lib in third_party/adlx/lib/
#
# Sets: ADLX_FOUND, ADLX_INCLUDE_DIR, ADLX_LIBRARY

set(_adlx_root "${CMAKE_SOURCE_DIR}/third_party/adlx")

find_path(ADLX_INCLUDE_DIR ADLXDefines.h
    PATHS "${_adlx_root}/include"
    NO_DEFAULT_PATH
)

find_library(ADLX_LIBRARY
    NAMES amd_adlx
    PATHS "${_adlx_root}/lib"
    NO_DEFAULT_PATH
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ADLX DEFAULT_MSG ADLX_INCLUDE_DIR ADLX_LIBRARY)

# FindNVML.cmake
# Searches CUDA Toolkit install locations for nvml.h and nvml.lib.
# Sets: NVML_FOUND, NVML_INCLUDE_DIR, NVML_LIBRARY

set(_nvml_search_roots
    "$ENV{CUDA_PATH}"
    "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.8"
    "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.6"
    "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.4"
    "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.3"
    "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.2"
    "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.0"
    "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.8"
    "C:/Program Files/NVIDIA Corporation/NVSMI"
)

find_path(NVML_INCLUDE_DIR nvml.h
    PATHS ${_nvml_search_roots}
    PATH_SUFFIXES include
    NO_DEFAULT_PATH
)

find_library(NVML_LIBRARY
    NAMES nvml
    PATHS ${_nvml_search_roots}
    PATH_SUFFIXES lib/x64 lib lib64
    NO_DEFAULT_PATH
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NVML DEFAULT_MSG NVML_INCLUDE_DIR NVML_LIBRARY)

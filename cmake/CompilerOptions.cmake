# CompilerOptions.cmake
# MSVC-specific flags for the whole project.

if(MSVC)
    add_compile_options(
        /W4         # High warning level
        /WX-        # Warnings are not errors (third-party headers are noisy)
        /permissive-
        /MP         # Parallel compilation
        /utf-8
        # Suppress specific warnings from vendor headers
        /wd4100     # unreferenced formal parameter
        /wd4201     # nonstandard extension: nameless struct/union
    )

    # Release: optimise for speed
    add_compile_options("$<$<CONFIG:Release>:/O2>")
    add_compile_options("$<$<CONFIG:Release>:/GL>")
    add_link_options("$<$<CONFIG:Release>:/LTCG>")

    # Always generate PDB
    add_compile_options(/Zi)
    add_link_options(/DEBUG)
endif()

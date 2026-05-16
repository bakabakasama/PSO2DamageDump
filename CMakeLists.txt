# CMakeLists.txt
cmake_minimum_required(VERSION 3.13)

# Definitions
project(PSO2DamageDump VERSION 1.0 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for VS Code (it is stupid otherwise)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# The goodies
file(GLOB CXX_SRCS "src/*.cpp")

# ---------------------------------------------------------
# Target: pso2damagedump.dll
# ---------------------------------------------------------
# Make a shared library (.dll)
add_library(pso2damagedump SHARED ${CXX_SRCS})

# Get rid of "lib" prefix
set_target_properties(pso2damagedump PROPERTIES PREFIX "")

# Output to bin/
set_target_properties(pso2damagedump PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add Windows library links
target_link_libraries(pso2damagedump PRIVATE shlwapi)

# ---------------------------------------------------------
# Compiler-Specific Overrides
# ---------------------------------------------------------
# MinGW linker flags
if(MINGW)
# --- COMPILER FLAGS ---
    target_compile_options(pso2damagedump PRIVATE 
        -Wall 
        -O2 
        -ffunction-sections 
        -fdata-sections 
        -flto
    )

    # --- LINKER FLAGS ---
    target_link_options(pso2damagedump PRIVATE 
        -static 
        -static-libgcc 
        -static-libstdc++ 
        -Wl,--enable-stdcall-fixup 
        -Wl,--kill-at
        -Wl,--gc-sections
        -flto
        -s
    )
endif()

# MSVC specific configurations
if(MSVC)
    target_compile_options(pso2damagedump PRIVATE /W3)
endif()
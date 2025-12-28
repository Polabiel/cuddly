cmake_minimum_required(VERSION 3.15)
project(cuddly VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Source files
set(SOURCES
    src/main.cpp
    src/config.c
)

# Executable
add_executable(cuddly ${SOURCES})

# Include directories (for future header files)
target_include_directories(cuddly PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(VTE REQUIRED vte-2.91)
pkg_check_modules(JSON_GLIB REQUIRED json-glib-1.0)

# Include directories for dependencies
target_include_directories(cuddly PRIVATE ${GTK3_INCLUDE_DIRS} ${VTE_INCLUDE_DIRS} ${JSON_GLIB_INCLUDE_DIRS})

# Link libraries
target_link_libraries(cuddly ${GTK3_LIBRARIES} ${VTE_LIBRARIES} ${JSON_GLIB_LIBRARIES})

# Compiler flags
target_compile_options(cuddly PRIVATE -Wall -Wextra -pedantic)

# Install rules
install(TARGETS cuddly DESTINATION bin)
install(FILES resources/defaults.json DESTINATION share/cuddly)

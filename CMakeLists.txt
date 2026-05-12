cmake_minimum_required(VERSION 3.10)
project(PotooMaps C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Definitions pour Windows
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Dossiers d'inclusion
include_directories(
    include
    include/Core
    include/Utils
    external/cJSON
)

# Sources (recherche recursive dans src/ et external/)
file(GLOB_RECURSE SOURCES 
    "src/*.c"
    "external/cJSON/*.c"
)

# Executable
add_executable(PotooMaps ${SOURCES})

# Flags d'avertissement pour une compilation stricte
target_compile_options(PotooMaps PRIVATE
    $<$<C_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wundef -Wstrict-prototypes -Wmissing-prototypes>
    $<$<C_COMPILER_ID:GNU>:-Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wdouble-promotion>
    $<$<C_COMPILER_ID:Clang>:-Wdocumentation -Wdouble-promotion>
    $<$<C_COMPILER_ID:MSVC>:/W4 /permissive->
)

# Bibliotheque mathematique (m) sur Linux/Unix
if(UNIX AND NOT APPLE)
    target_link_libraries(PotooMaps PRIVATE m)
endif()

# Copie automatique des Assets vers le dossier de build
# Cela permet de lancer l'executable directement depuis build/
add_custom_command(TARGET PotooMaps POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Assets
    $<TARGET_FILE_DIR:PotooMaps>/Assets
)

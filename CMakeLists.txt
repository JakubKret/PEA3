cmake_minimum_required(VERSION 3.20)
project(PEA4)

set(CMAKE_CXX_STANDARD 20)

# Definicja pliku wykonywalnego
add_executable(PEA4
        main.cpp
        Timer.h
        AtspGraph.h
        AtspGraph.cpp
        GeneticAlgorithm.h
        GeneticAlgorithm.cpp
)

# Optymalizacja kodu (odpowiednik flagi -O3)
if(MSVC)
    target_compile_options(PEA4 PRIVATE /O2)
else()
    target_compile_options(PEA4 PRIVATE -O3)
endif()

# Statyczne linkowanie dla Windowsa (dla środowiska MinGW/CLion)
if(WIN32)
    target_link_options(PEA4 PRIVATE -static -static-libgcc -static-libstdc++)
endif()
cmake_minimum_required(VERSION 3.20)
project(PEA3)

set(CMAKE_CXX_STANDARD 20)

# Definicja pliku wykonywalnego
add_executable(PEA3
        main.cpp
        Timer.h
        AtspGraph.h
        AtspGraph.cpp
        TabuSearch.h
        TabuSearch.cpp
)

# Optymalizacja kodu (odpowiednik flagi -O3)
if(MSVC)
    target_compile_options(PEA3 PRIVATE /O2)
else()
    target_compile_options(PEA3 PRIVATE -O3)
endif()

# Statyczne linkowanie dla Windowsa (dla środowiska MinGW/CLion)
if(WIN32)
    target_link_options(PEA3 PRIVATE -static -static-libgcc -static-libstdc++)
endif()
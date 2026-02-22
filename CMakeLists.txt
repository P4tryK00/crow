cmake_minimum_required(VERSION 3.15)
project(MojeCV)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# 1. Pobieramy bibliotekę Asio
FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-28-0
)
FetchContent_MakeAvailable(asio)

# 2. WYMUSZAMY (FORCE) ścieżkę do Asio w pamięci podręcznej
set(ASIO_INCLUDE_DIR "${asio_SOURCE_DIR}/asio/include" CACHE PATH "Ścieżka do Asio" FORCE)

# 3. Wyłączamy zbędne testy i przykłady Crow, by przyspieszyć proces
set(CROW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CROW_BUILD_TESTS OFF CACHE BOOL "" FORCE)

# 4. Pobieramy framework Crow
FetchContent_Declare(
        Crow
        GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
        GIT_TAG v1.2.0
)
FetchContent_MakeAvailable(Crow)

add_executable(MojeCV main.cpp)
target_link_libraries(MojeCV PRIVATE Crow::Crow)
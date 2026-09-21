cmake_minimum_required(VERSION 3.20)
project(RouteTrack VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ROUTETRACK_CORE_SOURCES
    src/modelos/Modelos.cpp
    src/servicios/Utilidades.cpp
    src/persistencia/CsvRepository.cpp
    src/persistencia/BitacoraService.cpp
    src/servicios/PedidoService.cpp
    src/servicios/ConductorService.cpp
    src/servicios/RutaService.cpp
    src/reportes/ReporteService.cpp
    src/reportes/CsvHtmlExporter.cpp
)

add_library(RouteTrackCore STATIC ${ROUTETRACK_CORE_SOURCES})
target_include_directories(RouteTrackCore PUBLIC include)
target_compile_definitions(RouteTrackCore PUBLIC ROUTETRACK_ROOT="${CMAKE_SOURCE_DIR}")

if(MSVC)
    target_compile_options(RouteTrackCore PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(RouteTrackCore PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(RouteTrack src/main.cpp src/interfaz/MenuConsola.cpp)
target_link_libraries(RouteTrack PRIVATE RouteTrackCore)

add_executable(RouteTrackTests tests/RouteTrackTests.cpp)
target_link_libraries(RouteTrackTests PRIVATE RouteTrackCore)

set_target_properties(RouteTrack RouteTrackTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
)

enable_testing()
add_test(NAME RouteTrackTests COMMAND RouteTrackTests)

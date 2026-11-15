cmake_minimum_required(VERSION 3.16)
project(Team11TrafficSimulation VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(traffic_core
    src/Road.cpp src/Intersection.cpp src/Vehicle.cpp src/TrafficNetwork.cpp
    src/network_config_parser.cpp
    src/Simulation.cpp tms/signals/signal.cpp tms/signals/signal_config_parser.cpp)
target_include_directories(traffic_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})
if(MSVC)
    target_compile_options(traffic_core PRIVATE /W4)
else()
    target_compile_options(traffic_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(traffic_sim src/main.cpp)
target_link_libraries(traffic_sim PRIVATE traffic_core)

add_executable(strategy_comparison src/strategy_comparison.cpp)
target_link_libraries(strategy_comparison PRIVATE traffic_core)

enable_testing()
add_executable(simulation_tests tests/simulation_tests.cpp)
target_link_libraries(simulation_tests PRIVATE traffic_core)
add_test(NAME simulation_tests COMMAND simulation_tests)

add_executable(performance_benchmark src/performance_benchmark.cpp)
target_link_libraries(performance_benchmark PRIVATE traffic_core)

add_executable(reliability_benchmark src/reliability_benchmark.cpp)
target_link_libraries(reliability_benchmark PRIVATE traffic_core)

# Independent component tests
add_executable(road_tests tests/road_tests.cpp)
target_link_libraries(road_tests PRIVATE traffic_core)
add_test(NAME road_tests COMMAND road_tests)

add_executable(vehicle_tests tests/vehicle_tests.cpp)
target_link_libraries(vehicle_tests PRIVATE traffic_core)
add_test(NAME vehicle_tests COMMAND vehicle_tests)

add_executable(intersection_tests tests/intersection_tests.cpp)
target_link_libraries(intersection_tests PRIVATE traffic_core)
add_test(NAME intersection_tests COMMAND intersection_tests)

add_executable(network_tests tests/network_tests.cpp)
target_link_libraries(network_tests PRIVATE traffic_core)
add_test(NAME network_tests COMMAND network_tests)

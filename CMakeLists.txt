# cmake_minimum_required(VERSION <specify CMake version here>)
project(MileStone3)

set(CMAKE_CXX_STANDARD 14)
set(GCC_COVERAGE_COMPILE_FLAGS "-pthread -std=c++11")

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
add_executable(MileStone3 MainTrain.cpp timeseries.h timeseries.cpp SimpleAnomalyDetector.h SimpleAnomalyDetector.cpp AnomalyDetector.h anomaly_detection_util.h anomaly_detection_util.cpp minCircle.h minCircle.cpp HybridAnomalyDetector.h HybridAnomalyDetector.cpp CLI.h CLI.cpp commands.h Server.h Server.cpp)
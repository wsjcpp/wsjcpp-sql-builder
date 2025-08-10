cmake_minimum_required(VERSION 3.0)

project(wsjcpp-sql-builder C CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/src.wsjcpp/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${wsjcpp-sql-builder_SOURCE_DIR})

# include header dirs
list (APPEND WSJCPP_INCLUDE_DIRS "src")

list (APPEND WSJCPP_SOURCES "src/main.cpp")
list (APPEND WSJCPP_SOURCES "src/wsjcpp_sql_builder.cpp")

#### BEGIN_WSJCPP_APPEND
#### END_WSJCPP_APPEND

include_directories(${WSJCPP_INCLUDE_DIRS})

add_executable (wsjcpp-sql-builder ${WSJCPP_SOURCES})

target_link_libraries(wsjcpp-sql-builder ${WSJCPP_LIBRARIES})

install(
    TARGETS
        wsjcpp-sql-builder
    RUNTIME DESTINATION
        /usr/bin
)


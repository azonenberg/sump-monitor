cmake_minimum_required(VERSION 3.3)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
project(scopehal-cmake)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-g -O3 -Wall -Wextra --std=c++11")

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)
pkg_check_modules(SIGCXX sigc++-2.0)

include_directories(${GTKMM_INCLUDE_DIRS} ${SIGCXX_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS} ${SIGCXX_LIBRARY_DIRS})

add_executable(sumpmon
	MainWindow.cpp
	main.cpp
)

target_link_libraries(sumpmon graphwidget
	${GTKMM_LIBRARIES}
	${SIGCXX_LIBRARIES}
	pthread
	)

add_subdirectory(graphwidget)

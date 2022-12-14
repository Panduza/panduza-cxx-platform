cmake_minimum_required(VERSION 3.16)

# set the project name and version
project(panduza-cxx-platform LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -I/usr/include/jsoncpp")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

set(SOURCES 
	src/loguru/loguru.cxx
	src/base64/base64.cxx
	src/main.cxx
	src/meta_platform.cxx
	src/meta_driver.cxx
	src/meta_drivers/meta_driver_io_fake.cxx
	src/meta_drivers/meta_driver_psu_fake.cxx
	src/meta_drivers/meta_driver_file_fake.cxx
	)

set(SOURCES-METAPLAFORM-LIBRARY
	src/meta_platform.cxx
	src/meta_driver.cxx
	)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

# include dependencies file to declare them
include(cmake/dependencies.cmake)

#initialize the PAHO C variables
set(PAHO_BUILD_SHARED TRUE)

# fetch PAHO C
FetchContent_MakeAvailable(paho.mqtt.c)

#initialize the PAHO C++ variables
set(PAHO_MQTT_C_INCLUDE_DIRS ${paho.mqtt.c_SOURCE_DIR}/src)
set(PAHO_MQTT_C_LIBRARIES ${paho.mqtt.c_BINARY_DIR}/src/libpaho-mqtt3as.so)

# fetch PAHO C++
FetchContent_MakeAvailable(paho.mqtt.cpp)

# make paho c++ dependent from paho c
add_dependencies(paho-cpp-objs paho-mqtt3as)

# set extra libraries needed
set(EXTRA_LIBS 
	dl 
	ftd2xx 
	pthread
	boost_filesystem
	boost_system
	# base64     #to uncomment if the library does load
)

# include mqtt libraries
set(MQTT_LIBRARIES
	paho-mqtt3as
	paho-mqtt3c 
	paho-mqttpp3
)

# check for jsoncpp library
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

# add the executable
add_executable(panduza-cxx-platform ${SOURCES})
add_library(Metaplatform SHARED ${SOURCES})
# add_library(Metaplatform SHARED ${SOURCES-METAPLAFORM-LIBRARY})
target_link_libraries(Metaplatform PUBLIC ${EXTRA_LIBS} ${JSONCPP_LIBRARIES} ${MQTT_LIBRARIES})

# link libraries with the executable
target_link_libraries(panduza-cxx-platform PUBLIC ${EXTRA_LIBS} ${JSONCPP_LIBRARIES} ${MQTT_LIBRARIES})

# add the binary tree to the search path for include files
target_include_directories(panduza-cxx-platform PUBLIC "${PROJECT_BINARY_DIR}")

install(TARGETS paho-mqtt3as paho-mqtt3c paho-mqttpp3
		LIBRARY DESTINATION /usr/local/lib
)

install (TARGETS Metaplatform
		LIBRARY DESTINATION /usr/local/lib
)
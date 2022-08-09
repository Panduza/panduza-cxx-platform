cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(Scan LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -I/usr/include/jsoncpp")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

# specify sources
set(SOURCES 
	src/loguru/loguru.cxx
	src/main.cxx
	src/meta_platform.cxx
	src/meta_driver.cxx
	src/meta_drivers/meta_driver_io_fake.cxx
	src/meta_drivers/meta_driver_psu_fake.cxx
	)

#set(CMAKE_BUILD_RPATH "/usr/local/lib")

# set extra libraries needed
set(EXTRA_LIBS 
	dl 
	ftd2xx 
	pthread
	boost_program_options
	boost_filesystem
	boost_system
	dl
)

# include mqtt libraries
set(MQTT_LIBRARIES
	paho-mqttpp3 
	paho-mqtt3as 
	paho-mqtt3c 
)

# check for jsoncpp library
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

# add the executable
add_executable(Scan ${SOURCES})

# link libraries with the executable
target_link_libraries(Scan PUBLIC ${EXTRA_LIBS} ${JSONCPP_LIBRARIES} ${MQTT_LIBRARIES})

# add the binary tree to the search path for include files
target_include_directories(Scan PUBLIC "${PROJECT_BINARY_DIR}")
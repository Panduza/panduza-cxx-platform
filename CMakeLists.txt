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
	# src/bsdl_parser/bsdl_loader.cxx 
	# src/bsdl_parser/bsdl_strings.cxx 
	# src/drivers/drv_loader.cxx 
	# src/drivers/ftdi_jtag/ftdi_jtag_drv.cxx 
	# src/jtag_core/script.cxx 
	# src/jtag_core/jtag_core.cxx
	src/loguru/loguru.cxx
	src/base64/base64.cxx
	src/main.cxx
	# src/subscriber.cxx
	# src/io.cxx
	# src/connect.cxx
	# src/interfaces.cxx
	src/meta_drivers/ft2232_boundary_scan/jtag_manager.cxx
	# src/config_parser.cxx
	src/meta_platform.cxx
	src/meta_driver.cxx
	src/meta_drivers/meta_driver_io_fake.cxx
	src/meta_drivers/meta_driver_psu_fake.cxx
	src/meta_drivers/meta_driver_group_info.cxx
	src/meta_drivers/meta_driver_FT2232_boundary_scan.cxx
	src/meta_drivers/meta_driver_FT2232_bsdl_loader.cxx
	src/meta_drivers/meta_driver_FT2232_io.cxx
	src/meta_drivers/ft2232_boundary_scan/jtag_core/jtag_core.cxx
	src/meta_drivers/ft2232_boundary_scan/jtag_core/script.cxx
	src/meta_drivers/ft2232_boundary_scan/drivers/drv_loader.cxx
	src/meta_drivers/ft2232_boundary_scan/drivers/ftdi_jtag/ftdi_jtag_drv.cxx
	src/meta_drivers/ft2232_boundary_scan/bsdl_parser/bsdl_loader.cxx
	src/meta_drivers/ft2232_boundary_scan/bsdl_parser/bsdl_strings.cxx
	src/meta_drivers/ft2232_boundary_scan/io.cxx
	)

#set(CMAKE_BUILD_RPATH "/usr/local/lib")

# set extra libraries needed
set(EXTRA_LIBS 
	dl 
	ftd2xx 
	pthread
	boost_program_options
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
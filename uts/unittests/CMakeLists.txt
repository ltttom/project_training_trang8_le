cmake_minimum_required(VERSION 2.8)
if(DEFINED ENV{BUILDTOP})
SET(BUILDTOP $ENV{BUILDTOP})
else()
string(FIND ${CMAKE_CURRENT_SOURCE_DIR} "/nad/" Index)
if(${Index} EQUAL "-1")
message(FATAL_ERROR "Not Defined BUILDTOP")
endif()
string( SUBSTRING ${CMAKE_CURRENT_SOURCE_DIR} 0 ${Index} BUILDTOP )
endif()
message("BUILDTOP = ${BUILDTOP}")
# set sysroot
SET( CMAKE_SYSROOT ${BUILDTOP}/nad/sa515m/SA515M_apps/apps_proc/build/tmp-glibc/work/armv7at2hf-neon-oe-linux-gnueabi/oemcallapp/0.0-r0/recipe-sysroot )
SET( CMAKE_C_COMPILER ${CMAKE_SYSROOT}/../recipe-sysroot-native/usr/bin/arm-oe-linux-gnueabi/arm-oe-linux-gnueabi-gcc )
SET( CMAKE_CXX_COMPILER ${CMAKE_SYSROOT}/../recipe-sysroot-native/usr/bin/arm-oe-linux-gnueabi/arm-oe-linux-gnueabi-g++ )
SET( QEMU_BIN_PATH ${CMAKE_SYSROOT}/../../../systemd/1_244.5-r0/recipe-sysroot-native/usr/bin/qemu-arm )
# set defines
ADD_DEFINITIONS (
	-DHAVE_CONFIG_H
	-DHAVE_SYS_UIO_H
	-DPIC
	-DTARGET_ION_ABI_VERSION=2
	-D_FORTIFY_SOURCE=2
	-D_MORE_MARSHM
	-D__UNITTEST__
)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ld-linux-armhf.so.3 ld-linux.so.3
    WORKING_DIRECTORY ${CMAKE_SYSROOT}/lib
)
# set include paths
INCLUDE_DIRECTORIES (
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/mock
	${CMAKE_CURRENT_SOURCE_DIR}/../../
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/data/
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/processor/
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/receiver/
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/services_adapter/
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils/
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/tspecall/
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/tspecall/core/services/
	${CMAKE_SYSROOT}
	${CMAKE_SYSROOT}/usr/include/servicelayer
	${CMAKE_SYSROOT}/usr/include/
# bitbake meta-shift.
	${BITBAKE_WORKDIR}/build/include
	${EXT_INCLUDE_PATH}
)
# set c cxx flags
ADD_COMPILE_OPTIONS (
	--sysroot=${CMAKE_SYSROOT}
	-O0
	-Wa,--noexecstack
	-Wall
	-Werror=array-bounds
	-Werror=overflow
	-Werror=sequence-point
	-Wformat-security
	-Wl,--as-needed
	-Wl,--hash-style=gnu
	-Wl,-retain-symbols-file
	-Wl,-soname
	-Wl,-z
	-Wl,-z,relro,-z,now,-z,noexecstack
	-Wl,.libs/liboemcall.exp
	-Wl,liboemcall.so.0
	-Wl,noexecstack
	-Wl,relro
	-Wno-return-type
	-Wno-write-strings
	-fno-access-control
	-fno-delete-null-pointer-checks
	-fpermissive
	-fprofile-arcs
	-ftest-coverage
	-fvisibility-inlines-hidden
	-g
	-march=armv7-a
	-mfloat-abi=hard
	-mfpu=neon
	-mthumb
	-nostdlib
	-pie
	-rdynamic
	-shared
    -fexceptions
	-std=c++14
)
# set link directory
link_directories(
	${CMAKE_SYSROOT}/usr/lib
	${CMAKE_SYSROOT}/lib
	${CMAKE_SYSROOT}/usr/lib/arm-oe-linux-gnueabi/8.2.0
	${EXT_LIBRARY_PATH}
)
# set target properties
SET(
	CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
# set target list
SET(
	ALL_TARGET
	someip_manager
	receiver_manager
	utils_common
	data_collector_can
	data_collector_gps
	data_builder_us
	data_builder_rcall
	oem_call_handler
	oem_call_app
	oem_call_timer
	acn_phone_processor
	processor_manager
	r_call_jp_processor
# TSP feature
	NAECallProcessor
	TelephoneManagerAdapter
	ApplicationManagerAdapter
	HTTPSManagerAdapter
)
#find_program(GCOV_PATH gcov)
# set coverage module
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake)
include(CodeCoverage)
set(COVERAGE_EXCLUDES '/usr/*' '*unittests/testscripts*' '*/usr/include/*' '*.h')
# bitbake meta-shift.
IF(NOT WITH_LGE_UNIT_TESTS)
SETUP_TARGET_FOR_COVERAGE(
	NAME all_coverage
	EXECUTABLE ${ALL_TARGET}
	DEPENDENCIES ${ALL_TARGET}
	)
SETUP_TARGET_FOR_UTS_RUNNER(
	NAME uts_run
	EXECUTABLE ${ALL_TARGET}
	DEPENDENCIES ${ALL_TARGET}
	)
ENDIF()
# bitbake meta-shift.
set(COMMON_LIBRARIES
	application
	binder
	context
	corebase
	crypto
	curl
	dl
	dlt
	gcov
	gmock
	gtest
	halsystem
	ialarm
	iapplication
	iaudio
	icommunication
	iconfiguration
	idiag
	ihmi
	ilgefileio
	ilocation
	ioperationmode
	ipower
	iremoteif
	isomeipconsumer
	isomeipprovider
	itelephony
	itime
	ivehicle
	jsoncpp
	m
	pcre
	property
	pthread
	serviceutils
	ssl
	utils
	watchdog_client
	${EXT_PTHREAD_LIB}
)
# set filelist for each target
SET ( someip_manager_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/UT_GMOCK_someip_manager.cpp
)
add_executable ( someip_manager ${someip_manager_SRC_FILES} )
# set library for each target
target_link_libraries( someip_manager ${COMMON_LIBRARIES})
SETUP_TARGET_FOR_COVERAGE(
	NAME someip_manager_coverage
	EXECUTABLE someip_manager
	DEPENDENCIES someip_manager
)
SET ( receiver_manager_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/UT_GMOCK_receiver_manager.cpp
)
add_executable ( receiver_manager ${receiver_manager_SRC_FILES} )
# set library for each target
target_link_libraries( receiver_manager ${COMMON_LIBRARIES} )
SETUP_TARGET_FOR_COVERAGE(
	NAME receiver_manager_coverage
	EXECUTABLE receiver_manager
	DEPENDENCIES receiver_manager
)
SET ( utils_common_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/UT_GMOCK_utils_common.cpp
)
add_executable ( utils_common ${utils_common_SRC_FILES} )
# set library for each target
target_link_libraries( utils_common ${COMMON_LIBRARIES} )
SETUP_TARGET_FOR_COVERAGE(
	NAME utils_common_coverage
	EXECUTABLE utils_common
	DEPENDENCIES utils_common
)
SET ( data_collector_can_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/UT_GMOCK_data_collector_can.cpp
)
add_executable ( data_collector_can ${data_collector_can_SRC_FILES} )
# set library for each target
target_link_libraries( data_collector_can ${COMMON_LIBRARIES} )
SETUP_TARGET_FOR_COVERAGE(
	NAME data_collector_can_coverage
	EXECUTABLE data_collector_can
	DEPENDENCIES data_collector_can
)
SET ( data_collector_gps_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/UT_GMOCK_data_collector_gps.cpp
)
add_executable ( data_collector_gps ${data_collector_gps_SRC_FILES} )
# set library for each target
target_link_libraries( data_collector_gps ${COMMON_LIBRARIES} )
SETUP_TARGET_FOR_COVERAGE(
	NAME data_collector_gps_coverage
	EXECUTABLE data_collector_gps
	DEPENDENCIES data_collector_gps
)
SET ( data_builder_us_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/UT_GMOCK_data_builder_us.cpp
)
add_executable ( data_builder_us ${data_builder_us_SRC_FILES} )
# set library for each target
target_link_libraries( data_builder_us ${COMMON_LIBRARIES} )
SETUP_TARGET_FOR_COVERAGE(
	NAME data_builder_us_coverage
	EXECUTABLE data_builder_us
	DEPENDENCIES data_builder_us
)
SET ( data_builder_rcall_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/UT_GMOCK_data_builder_rcall.cpp
)
add_executable ( data_builder_rcall ${data_builder_rcall_SRC_FILES} )
# set library for each target
target_link_libraries( data_builder_rcall ${COMMON_LIBRARIES} )
SETUP_TARGET_FOR_COVERAGE(
	NAME data_builder_rcall_coverage
	EXECUTABLE data_builder_rcall
	DEPENDENCIES data_builder_rcall
)
SET ( oem_call_handler_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/UT_GMOCK_oem_call_handler.cpp
)
add_executable ( oem_call_handler ${oem_call_handler_SRC_FILES} )
# set library for each target
target_link_libraries( oem_call_handler ${COMMON_LIBRARIES} )
SETUP_TARGET_FOR_COVERAGE(
	NAME oem_call_handler_coverage
	EXECUTABLE oem_call_handler
	DEPENDENCIES oem_call_handler
)
SET ( oem_call_app_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/UT_GMOCK_oem_call_app.cpp
)
add_executable ( oem_call_app ${oem_call_app_SRC_FILES} )
# set library for each target
target_link_libraries( oem_call_app ${COMMON_LIBRARIES} )
SETUP_TARGET_FOR_COVERAGE(
	NAME oem_call_app_coverage
	EXECUTABLE oem_call_app
	DEPENDENCIES oem_call_app
)
SET ( oem_call_timer_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/UT_GMOCK_oem_call_timer.cpp
)
add_executable ( oem_call_timer ${oem_call_timer_SRC_FILES} )
# set library for each target
target_link_libraries( oem_call_timer ${COMMON_LIBRARIES} )
SETUP_TARGET_FOR_COVERAGE(
	NAME oem_call_timer_coverage
	EXECUTABLE oem_call_timer
	DEPENDENCIES oem_call_timer
)
SET ( acn_phone_processor_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/UT_GMOCK_acn_phone_processor.cpp
)
add_executable ( acn_phone_processor ${acn_phone_processor_SRC_FILES} )
# set library for each target
target_link_libraries( acn_phone_processor ${COMMON_LIBRARIES} )
SETUP_TARGET_FOR_COVERAGE(
	NAME acn_phone_processor_coverage
	EXECUTABLE acn_phone_processor
	DEPENDENCIES acn_phone_processor
)
SET ( processor_manager_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/UT_GMOCK_processor_manager.cpp
)
add_executable ( processor_manager ${processor_manager_SRC_FILES} )
# set library for each target
target_link_libraries( processor_manager ${COMMON_LIBRARIES} )
SETUP_TARGET_FOR_COVERAGE(
	NAME processor_manager_coverage
	EXECUTABLE processor_manager
	DEPENDENCIES processor_manager
)
SET ( r_call_jp_processor_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/UT_GMOCK_r_call_jp_processor.cpp
)
add_executable ( r_call_jp_processor ${r_call_jp_processor_SRC_FILES} )
# set library for each target
target_link_libraries( r_call_jp_processor ${COMMON_LIBRARIES} )
SETUP_TARGET_FOR_COVERAGE(
	NAME r_call_jp_processor_coverage
	EXECUTABLE r_call_jp_processor
	DEPENDENCIES r_call_jp_processor
)

# TSP feature

##############################################################################

SET ( NAECallProcessor_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/tsp_src/UT_GMOCK_NAECallProcessor.cpp
)

add_executable ( NAECallProcessor ${NAECallProcessor_SRC_FILES} )

# set library for each target
target_link_libraries( NAECallProcessor
	dl
	dlt
	gcov
	gmock
	gtest
	itime
	jsoncpp
	m
	pcre
	utils
	serviceutils
	itelephony
	iconfiguration
	binder
	ssl
	${EXT_PTHREAD_LIB}
)

SETUP_TARGET_FOR_COVERAGE(
	NAME NAECallProcessor_coverage
	EXECUTABLE NAECallProcessor
	DEPENDENCIES NAECallProcessor
)

##############################################################################

SET ( TelephoneManagerAdapter_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/tsp_src/UT_GMOCK_TelephoneManagerAdapter.cpp
)

add_executable ( TelephoneManagerAdapter ${TelephoneManagerAdapter_SRC_FILES} )

# set library for each target
target_link_libraries( TelephoneManagerAdapter
	dl
	dlt
	gcov
	gmock
	gtest
	itime
	jsoncpp
	m
	pcre
	utils
	serviceutils
	itelephony
	iconfiguration
	binder
	ssl
	${EXT_PTHREAD_LIB}
)

SETUP_TARGET_FOR_COVERAGE(
	NAME TelephoneManagerAdapter_coverage
	EXECUTABLE TelephoneManagerAdapter
	DEPENDENCIES TelephoneManagerAdapter
)

##############################################################################

SET ( ApplicationManagerAdapter_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/tsp_src/UT_GMOCK_ApplicationManagerAdapter.cpp
)

add_executable ( ApplicationManagerAdapter ${ApplicationManagerAdapter_SRC_FILES} )

# set library for each target
target_link_libraries( ApplicationManagerAdapter
	dl
	dlt
	gcov
	gmock
	gtest
	itime
	jsoncpp
	m
	pcre
	utils
	iapplication
	serviceutils
	itelephony
	iconfiguration
	binder
	ssl
	${EXT_PTHREAD_LIB}
)

##############################################################################

SETUP_TARGET_FOR_COVERAGE(
	NAME ApplicationManagerAdapter_coverage
	EXECUTABLE ApplicationManagerAdapter
	DEPENDENCIES ApplicationManagerAdapter
)

find_library(IIDSI_BINDER_LIB iidsibinder ${CMAKE_SYSROOT})

if (IIDSI_BINDER_LIB)
    #add_definitions(-DREGION_PCB26)
    message(STATUS "Found iidsibinder: ${IIDSI_BINDER_LIB}")
else()
    message(FATAL_ERROR "Could not find iidsibinder")
endif()

SET ( HTTPSManagerAdapter_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/gtest_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testscripts/tsp_src/UT_GMOCK_HTTPSManagerAdapter.cpp
)

add_executable (HTTPSManagerAdapter ${HTTPSManagerAdapter_SRC_FILES} )

# set library for each target
target_link_libraries( HTTPSManagerAdapter
	dl
	dlt
	gcov
	gmock
	gtest
	itime
	jsoncpp
	m
	pcre
	utils
	serviceutils
	itelephony
	iconfiguration
	binder
	ssl
	curl
  # ${IIDSI_BINDER_LIB}
	${EXT_PTHREAD_LIB}
)

SETUP_TARGET_FOR_COVERAGE(
	NAME HTTPSManagerAdapter_coverage
	EXECUTABLE HTTPSManagerAdapter
	DEPENDENCIES HTTPSManagerAdapter
)

# bitbake meta-shift.
# googletest/ctest
Find_Package(GTest)
foreach(BIN ${ALL_TARGET})
    if(WITH_LGE_UNIT_TESTS AND GTEST_FOUND)
        GTEST_ADD_TESTS( ${BIN} "" AUTO )
        MESSAGE ( STATUS " GTEST_ADD_TEST BIN : ${BIN}" )
    endif()
endforeach()
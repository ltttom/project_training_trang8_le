# Run all Target or single Target

find_program( QEMU_ARM_PATH qemu-arm)

if (EXISTS ${QEMU_ARM_PATH})
else()
set(QEMU_ARM_PATH ${QEMU_BIN_PATH})
endif()

Message ("SYSROOTS  : ${CMAKE_SYSROOT}")
Message ("PATH QEMU : ${QEMU_ARM_PATH}")
 
set( ENV{LD_LIBRARY_PATH} ${CMAKE_SYSROOT}/usr/lib/ )
 
# convert variable from string to list
separate_arguments(TARGET_BIN)
foreach(BIN ${TARGET_BIN})
        message("Run : ${QEMU_ARM_PATH} ${TARGET_PATH}/${BIN} --gtest_output=xml:reports/${BIN}-result.xml\n")
        execute_process(COMMAND ${QEMU_ARM_PATH} -L ${CMAKE_SYSROOT} ${TARGET_PATH}/${BIN} --gtest_output=xml:reports/${BIN}-result.xml)
endforeach()

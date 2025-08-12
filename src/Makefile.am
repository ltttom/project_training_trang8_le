#Makefile.am - liboemcall.a
common_sources:= \
	oem_call_app.cpp \
	oem_call_handler.cpp \
	oem_call_timer.cpp \
	data/data_builder_rcall.cpp \
	data/data_builder_us.cpp \
	data/data_collector_can.cpp \
	data/data_collector_gps.cpp \
	processor/acn_phone_processor.cpp \
	processor/processor_manager.cpp \
	processor/r_call_jp_processor.cpp \
	services_adapter/receiver_manager.cpp \
	services_adapter/someip_manager.cpp \
	services_adapter/telephone_manager_adapter.cpp \
	services_adapter/config_manager_adapter.cpp \
	services_adapter/hmi_manager_adapter.cpp \
	services_adapter/audio_manager_adapter.cpp \
	services_adapter/power_manager_adapter.cpp \
	services_adapter/app_manager_adapter.cpp \
	services_adapter/rim_adapter.cpp \
	utils/utils_common.cpp \
	logger.cpp \
	tspecall/variant/honda/processors/ECallState.cpp \
	tspecall/variant/honda/processors/NAECallProcessor.cpp \
	tspecall/variant/honda/processors/NAECallRecovery.cpp \
	tspecall/variant/honda/processors/NADESSECall.cpp \
	tspecall/variant/honda/processors/CPDSmartphoneProcessor.cpp \
	tspecall/variant/honda/msd/DataBuilderUS.cpp \
	tspecall/variant/honda/msd/DataBuilderCPDSmartphone.cpp \
	tspecall/variant/honda/processors/ECallVariableRepo.cpp \
	tspecall/variant/honda/processors/NAECallStateMachine.cpp \
	tspecall/variant/honda/processors/NAECallStates.cpp \
	tspecall/core/ECallApp2.cpp \
	tspecall/core/processors/ProcessorsManager.cpp \
	tspecall/core/services/ApplicationManagerAdapter.cpp \
	tspecall/core/services/CommunicationManagerAdapter.cpp \
	tspecall/core/services/VehicleManagerAdapter.cpp \
	tspecall/core/services/TelephoneManagerAdapter.cpp \
	tspecall/core/services/PowerManagerAdapter.cpp \
	tspecall/core/services/LocationManagerAdapter.cpp \
	tspecall/core/services/HMIManagerAdapter.cpp \
	tspecall/core/services/AudioManagerAdapter.cpp \
	tspecall/core/services/ConfigurationManagerAdapter.cpp \
	tspecall/core/services/HTTPSManagerAdapter.cpp \
	tspecall/core/services/AlarmManagerAdapter.cpp \
	tspecall/core/services/RemoteIfManagerAdapter.cpp \
	tspecall/core/services/SystemPropertyAdapter.cpp \
	tspecall/core/services/TimeManagerAdapter.cpp \
  tspecall/core/services/VehicleVariationAdapter.cpp \
	tspecall/core/utils/BackgroundTask.cpp \
	tspecall/core/utils/ECallAppHandler.cpp \
	tspecall/core/utils/CommonFunctions.cpp

# Target name DSO
lib_LTLIBRARIES := liboemcall.la

liboemcall_la_CPPFLAGS := \
  -I$(SDKTARGETSYSROOT)/usr/include/servicelayer \
  -I$(top_srcdir)/src \
  -D_MORE_MARSHM \
  -O2 \
  -g \
  -Wall \
  -Werror \
  -Wno-unused-parameter \
  -DHAVE_SYS_UIO_H \
  -rdynamic \
  -std=c++14 \
  $(LGE_BIN_CFLAGS)\
        $(AM_CPPFLAGS)

# add lib hmi and vif
liboemcall_la_LDFLAGS = \
  -lpthread \
  -lhalsystem \
  -lilgefileio \
  -lapplication \
  -liapplication \
  -licommunication \
  -liconfiguration \
  -lisomeipprovider \
  -lisomeipconsumer \
  -liaudio \
  -litime \
  -lipower \
  -lilocation \
  -lcorebase \
  -lwatchdog_client \
  -litelephony \
  -shared \
  -module \
  -lutils \
  -ldlt \
  -lbinder \
  -liremoteif \
  -lserviceutils \
  -livehicle \
  -lihmi \
  -lm \
  -ldl \
  -lpcre \
  -ljsoncpp\
  -lcurl \
  -lcontext \
  -lproperty \
  -lioperationmode \
  -lialarm \
  -export-dynamic \
  -export-symbols-regex 'createApplication' \
  $(IDS_LIBS) \
  $(VEHICLE_INFOR_LIB) \
  $(LGE_BIN_LDFLAGS)

requiredlibs := -lpthread

liboemcall_la_SOURCES := \
  $(common_sources)

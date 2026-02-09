#####################################
#         MLE Tech		    #
#   ESP32 Project build Makefile.   #
#####################################

############################################
# User supplied and default parameters
############################################
# default platform
DOIT := 1

# Base package name
PROJECT := Midi32.ino

# Shared files directory
SHARED := ../Common

# library paths
LIBS := --libraries ./libraries --library $(SHARED) 

# header files
INCLUDES := config.h UpdateOTA.h I2Cmessages.h FrontEnd.h ConfigJSON.h \
            LFOosc.h Osc.h SerialMonitor.h Settings.h Voice.h Filter4.h FilterLP.h Novation.h \
	        $(SHARED)/DispMessages.h $(SHARED)/SynthCommon.h Stats.h
 
# source files
SOURCES := $(PROJECT) Config.cpp SerialMonitor.cpp UpdateOTA.cpp Settings.cpp I2Cmessages.cpp \
           FrontEnd.cpp MIDI_Keyboard.cpp MIDI_Control.cpp MIDI_Sequencer.cpp Novation.cpp \
           Control_Voice.cpp Control_LFO.cpp Tuning.cpp Mapping.cpp LFOosc.cpp \
           Filter4.cpp FilterLP.cpp Voice.cpp Osc.cpp 
   
# serial port for uploading
DEFAULT_PORT := COM3
#DEFAULT_PORT := COM8
  
# IP address for uploading
UPLOAD_OTA := 192.168.2.21


############################################
# Configuration for Seeed Studio ESP32c3
############################################
ifdef C3
  FQBN   := esp32:esp32:XIAO_ESP32C3
  OUTPUT := output_esp32c3
endif
############################################
# Configuration for Wrooom ESP32-S3-WROOM-1
############################################
ifdef S3
  FQBN   := esp32:esp32:esp32s3usbotg
#  FQBN   := --fqbn esp32:esp32:esp32s3
  OUTPUT := output_esp32s3
endif
############################################
# Configuration for Wrooom ESP32-WROOM
############################################
ifdef DOIT
  FQBN   := esp32:esp32:esp32doit-devkit-v1
  OUTPUT := output_doit
endif

############################################
# Fixed definitions
############################################
ARDUNIO := arduino-cli
BUILD   := --build-path $(OUTPUT)/build --output-dir $(OUTPUT)
COMPILE := compile $(BUILD) --log-level warn --log-file $(OUTPUT)/build.log  $(LIBS) -j 0
TARGET  := $(OUTPUT)/$(PROJECT).bin
MD5      = md5sums -u $(TARGET)

############################################
# Use default or user supplied COM port
############################################
ifndef SERIAL_PORT
  SERIAL_PORT := $(DEFAULT_PORT)
endif

############################################
# Use Serial port or OTA to upload
############################################
ifdef OTA
  UPLOAD := espota.exe -d -r -i $(OTA) -p 3232 --auth=admin -f $(TARGET)
else
  UPLOAD := $(ARDUNIO) upload --input-dir $(OUTPUT) -p $(PT) --fqbn $(FQBN) $(TARGET)
endif


############################################
############################################
# User input targets
############################################
PHONY: OTA Serial monitor clean

OTA :
	@$(MAKE) target OTA=$(UPLOAD_OTA)

Serial :
	@$(MAKE) target PT=$(SERIAL_PORT)

clean :
	@rmdir /Q /S $(OUTPUT)



############################################
############################################
# Build instructions
############################################
target : $(TARGET)
	@python ../Tools/Beep.py UPLOAD
	$(UPLOAD)
	@python ../Tools/Beep.py OK

$(TARGET) : $(SOURCES) $(INCLUDES)  libraries/.git
	if not exist $(OUTPUT) (mkdir $(OUTPUT))
	$(ARDUNIO) $(COMPILE) --fqbn $(FQBN)

libraries/.git:
	@git submodule update --init --recursive


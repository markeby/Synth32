#####################################
#         MLE Tech		    #
#   ESP32 Project build Makefile.   #
#####################################

############################################
# User supplied and default parameters
############################################
# default platform
DOIT := 1
#S3  := 1
# Base package name
PROJECT := Midi32.ino

# Shared files directory
SHARED := ../Common

# header files
INCLUDES := config.h Debug.h Diagnostics.h Envelope.h  FrontEnd.h I2Cdevices.h I2Cmessages.h Files.h \
            LFOosc.h Osc.h SerialMonitor.h Settings.h SoftLFO.h SynthChannel.h UpdateOTA.h \
	    $(SHARED)/DispMessages.h $(SHARED)/SynthCommon.h
	   

# source files
SOURCES := $(PROJECT) SoftLFO.cpp I2Cdevices.cpp Debug.cpp Settings.cpp I2Cmessages.cpp Files.cpp \
           SerialMonitor.cpp FrontEnd.cpp FrontEndLFO.cpp SynthChannel.cpp Osc.cpp \
	   Controls.cpp LFOosc.cpp Envelope.cpp Diagnostics.cpp UpdateOTA.cpp

# serial port for uploading
DEFAULT_PORT := COM3

# IP address for uploading
UPLOAD_OTA := 192.168.1.30

############################################
# Configuration for Seeed Studio ESP32c3
############################################
ifdef C3
  ESP32  := --fqbn esp32:esp32:XIAO_ESP32C3
  OUTPUT := output_esp32c3
endif
############################################
# Configuration for Wrooom ESP32-S3-WROOM-1
############################################
ifdef S3
  ESP32 := --fqbn esp32:esp32:esp32s3usbotg
#  ESP32 := --fqbn esp32:esp32:esp32s3
  OUTPUT := output_esp32s3
endif
############################################
# Configuration for Wrooom ESP32-WROOM
############################################
ifdef DOIT
  ESP32  := --fqbn esp32:esp32:esp32doit-devkit-v1
  OUTPUT := output_doit
endif

############################################
# Fixed definitions
############################################
ARDUNIO := arduino-cli
BUILD   := --build-path $(OUTPUT)/build --output-dir $(OUTPUT)
COMPILE := compile $(BUILD) --log-level warn --log-file $(OUTPUT)/build.log
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
  UPLOAD := $(ARDUNIO) upload --input-dir $(OUTPUT) -p $(PT) $(ESP32) $(TARGET)
endif


############################################
############################################
# User input targets
############################################
PHONY: testOTA lab c3 monitor clean

Midi32_OTA :
	@$(MAKE) target OTA=$(UPLOAD_OTA)

Midi32_Serial :
	@$(MAKE) target PT=$(SERIAL_PORT)

monitor :
	$(ARDUNIO) monitor -p $(PORT) -c baudrate=115200

clean :
	@rmdir /Q /S $(OUTPUT)



############################################
############################################
# Build instructions
############################################
target : $(TARGET)
	@python3 ../Tools/Beep.py UPLOAD
	$(UPLOAD)
	@python3 ../Tools/Beep.py OK

$(TARGET) : $(SOURCES) $(INCLUDES)
	if not exist $(OUTPUT) (mkdir $(OUTPUT))
	$(ARDUNIO) $(COMPILE) $(ESP32)


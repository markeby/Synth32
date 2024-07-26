#####################################
#         MLE Tech		    #
#   ESP32 Project build Makefile.   #
#####################################

############################################
# User supplied and default parameters
############################################
# default platform
S3DEV := 1

# Base package name
PROJECT := Disp32.ino

# header files
SHARED := ../Common

# Shared files directory
INCLUDES := config.h SerialMonitor.h Settings.h Graphics.h Widgets.h \
            ESP_Panel_Board_Custom.h ESP_Panel_Board_Supported.h ESP_Panel_Conf.h lvgl_port_v8.h \
	    Debug.h $(SHARED)/DispMessages.h $(SHARED)/SynthCommon.h
# source files
SOURCES := $(PROJECT) Debug.cpp Widgets.cpp lvgl_port_v8.cpp SerialMonitor.cpp \
           Settings.cpp ClientI2C.cpp Graphics.cpp

# serial port for uploading
DEFAULT_PORT := COM7

# IP address for uploading
UPLOAD_OTA := 192.168.1.31

# ESP32 board options
# PSRAM=disabled,PartitionScheme=default,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,LoopCore=1,EventsCore=1,DebugLevel=none
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
# ESP32 := --fqbn esp32:esp32:esp32s3usbotg
  ESP32 := --fqbn esp32:esp32:esp32s3:PSRAM=opi,PartitionScheme=huge_app 
  OUTPUT := output_esp32s3
endif
############################################
# Configuration for Wrooom ESP32-S3-WROOM-1
############################################
ifdef S3DEV
  ESP32 := --fqbn esp32:esp32:esp32s3:PSRAM=opi,FlashSize=8M,PartitionScheme=huge_app 
  OUTPUT := output_esp32s3dev
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
COMPILE := compile $(BUILD) --log-level warn --log-file $(OUTPUT)/build.log --library $(SHARED) 
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
  UPLOAD := python3 ../tools/UploadOTA.py $(TARGET) $(OTA)
else
  UPLOAD := $(ARDUNIO) upload --input-dir $(OUTPUT) -p $(PT) $(ESP32) $(TARGET)
endif


############################################
############################################
# User input targets
############################################
PHONY: testOTA lab c3 monitor clean

Disp32_OTA :
	@$(MAKE) target OTA=$(UPLOAD_OTA)

Disp32_Serial :
	@$(MAKE) target PT=$(SERIAL_PORT)

monitor :
	@$(ARDUNIO) monitor -p $(PORT) -c baudrate=115200

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
	@if not exist $(OUTPUT) (mkdir $(OUTPUT))
	$(ARDUNIO) $(COMPILE) $(ESP32) 



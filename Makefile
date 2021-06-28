PROGRAM = ESP8266_MP3_DECODER

PROGRAM_SRC_DIR = ./src
PROGRAM_INC_DIR = ./include
EXTRA_COMPONENTS = ./libmad

LIBS ?= gcc hal m

include $(SDK_PATH)/common.mk

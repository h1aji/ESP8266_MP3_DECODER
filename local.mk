
# Flash size in megabits
# Valid values are same as for esptool.py - 2,4,8,16,32
FLASH_SIZE ?= 32

# Flash mode, valid values are same as for esptool.py - qio,qout,dio.dout
FLASH_MODE ?= qio

# Flash speed in MHz, valid values are same as for esptool.py - 80, 40, 26, 20
FLASH_SPEED ?= 40

# Output directories to store intermediate compiled files
# relative to the program directory
BUILD_DIR ?= $(PROGRAM_DIR)build/
FIRMWARE_DIR ?= $(PROGRAM_DIR)firmware/

# esptool.py from https://github.com/themadinventor/esptool
ESPTOOL ?= /usr/bin/esptool
# serial port settings for esptool.py
ESPPORT ?= /dev/ttyUSB0
ESPBAUD ?= 115200

# firmware tool arguments
ESPTOOL_ARGS=-fs $(FLASH_SIZE)m -fm $(FLASH_MODE) -ff $(FLASH_SPEED)m

# Source components to compile and link. Each of these are subdirectories
# of the root, with a 'component.mk' file.
COMPONENTS     ?= $(EXTRA_COMPONENTS) FreeRTOS lwip core open_esplibs

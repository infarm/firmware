v ?= 0
DEBUG_BUILD ?= n
APP ?= modbus
USE_SWD ?= y
MF_DFU ?= all program-dfu
MF_OCD ?= all program-ocd
MF_FLASH ?= $(MF_DFU)
MF_DEBUG ?= PARTICLE_DEVELOP=1 PLATFORM=photon MODULAR=n
MF_RELEASE ?= PLATFORM=photon DEBUG_BUILD=n USE_SWD=n USE_SWD=n
MAKE_FLAGS ?= $(MF_DEBUG) v=$(v) APP=$(APP) USE_SWD=$(USE_SWD)

modbus-flash:
	make $(MF_FLASH) $(MAKE_FLAGS)

modbus-test:
	make clean all program-dfu TEST=wiring/no_fixture PLATFORM=photon

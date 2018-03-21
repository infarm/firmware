
TARGET_SPARK_CRYPTO_SRC_PATH = $(CRYPTO_MODULE_PATH)/src

CPPSRC += $(call target_files,$(TARGET_SPARK_CRYPTO_SRC_PATH),*.cpp)
CSRC += $(call target_files,$(TARGET_SPARK_CRYPTO_SRC_PATH),*.c)

CPPFLAGS += -std=gnu++1z

LOG_MODULE_CATEGORY = crypto


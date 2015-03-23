ifeq ($(call is-board-platform-in-list, msm8996),true)
    TARGET_USES_SDE = true
else
    TARGET_USES_SDE = false
endif

display-hals := libgralloc libcopybit liblight libmemtrack libqservice libqdutils
display-hals += hdmi_cec

ifeq ($(TARGET_USES_SDE), true)
    sde-libs := displayengine/libs
    display-hals += $(sde-libs)/utils $(sde-libs)/core $(sde-libs)/hwc
else
    display-hals += libgenlock libhwcomposer liboverlay libhdmi
endif

ifeq ($(call is-vendor-board-platform,QCOM),true)
    include $(call all-named-subdir-makefiles,$(display-hals))
else
ifneq ($(filter msm% apq%,$(TARGET_BOARD_PLATFORM)),)
    include $(call all-named-subdir-makefiles,$(display-hals))
endif
endif

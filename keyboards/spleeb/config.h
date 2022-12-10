// Copyright 2022 Chris Hoage (@chrishoage)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "config_common.h"

/* Handedness. */
#define MASTER_RIGHT

#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_MODS_ENABLE

#ifdef POINTING_DEVICE_ENABLE
// Transport dpi for display on oled
#    define SPLIT_TRANSACTION_IDS_KB RPC_ID_KB_CONFIG_SYNC

#    define POINTING_DEVICE_AUTO_MOUSE_ENABLE
#    define CIRQUE_PINNACLE_POSITION_MODE CIRQUE_PINNACLE_ABSOLUTE_MODE
#    define POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE
#endif // POINTING_DEVICE_ENABLE

#define I2C_DRIVER I2CD1
#define I2C1_SDA_PIN GP16
#define I2C1_SCL_PIN GP17

#ifdef OLED_ENABLE
#    define OLED_DISPLAY_128X64
#    define OLED_FONT_H "./lib/glcdfont.c"
#endif // OLED_ENABLE

/* Reset. */
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED GP17
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 1000U

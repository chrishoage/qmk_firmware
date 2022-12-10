// Copyright 2022 Chris Hoage (@chrishoage)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#ifdef POINTING_DEVICE_ENABLE
// Transport dpi for display on oled
#    define SPLIT_TRANSACTION_IDS_KB RPC_ID_KB_CONFIG_SYNC

#    define AUTO_MOUSE_TIME 250

#    define CIRQUE_PINNACLE_DIAMETER_MM 35
#    define POINTING_DEVICE_ROTATION_180

#    define SPLEEB_DRAGSCROLL_REVERSE_X
#endif // POINTING_DEVICE_ENABLE

#define BOOTMAGIC_LITE_ROW 5
#define BOOTMAGIC_LITE_COLUMN 6

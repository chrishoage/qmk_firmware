// Copyright 2022 Chris Hoage (@chrishoage)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "spleeb.h"
#include "transactions.h"

#ifdef CONSOLE_ENABLE
#    include "print.h"
#endif // CONSOLE_ENABLE

#ifdef ENCODER_ENABLE
enum EncoderMode {
    ENCODER_DEFAULT_DPI,
    ENCODER_SNIPING_DPI,
    _NUM_R_MODES,
    ENCODER_VOLUME,
    _NUM_L_MODES,
};

#    define __ENC_L_START (_NUM_R_MODES + 1)
#endif // ENCODER_ENABLE

#if defined(ENCODER_ENABLE) || defined(POINTER_ENABLE)
typedef union {
    uint32_t raw;
    struct {
        uint8_t pointer_default_dpi : 4; // 16 steps available.
        uint8_t pointer_sniping_dpi : 2; // 4 steps available.
        uint8_t enc_modes[2];
        bool    is_dragscroll_enabled : 1;
        bool    is_sniping_enabled : 1;
    } __attribute__((packed));
} spleeb_config_t;

static spleeb_config_t g_spleeb_config = {0};

/**
 * \brief Set the value of `config` from EEPROM.
 *
 * Note that `is_dragscroll_enabled` and `is_sniping_enabled` are purposefully
 * ignored since we do not want to persist this state to memory.  In practice,
 * this state is always written to maximize write-performances.  Therefore, we
 * explicitly set them to `false` in this function.
 */
static void read_spleeb_config_from_eeprom(spleeb_config_t* config) {
    config->raw                   = eeconfig_read_kb();
    config->is_dragscroll_enabled = false;
    config->is_sniping_enabled    = false;
}

/**
 * \brief Save the value of `config` to eeprom.
 *
 * Note that all values are written verbatim, including whether drag-scroll
 * and/or sniper mode are enabled.  `read_spleeb_config_from_eeprom(…)`
 * resets these 2 values to `false` since it does not make sense to persist
 * these across reboots of the board.
 */
static void write_spleeb_config_to_eeprom(spleeb_config_t* config) {
    eeconfig_update_kb(config->raw);
}

void eeconfig_init_kb(void) {
    g_spleeb_config.raw                 = 0;
    g_spleeb_config.pointer_default_dpi = 4;
    for (size_t i = 0; i < NUM_ENCODERS; i++) {
        g_spleeb_config.enc_modes[i] = 0;
    }

    write_spleeb_config_to_eeprom(&g_spleeb_config);
    eeconfig_init_user();
}

void matrix_init_kb(void) {
    read_spleeb_config_from_eeprom(&g_spleeb_config);
    matrix_init_user();
}

void spleeb_config_sync_handler(uint8_t initiator2target_buffer_size, const void* initiator2target_buffer, uint8_t target2initiator_buffer_size, void* target2initiator_buffer) {
    if (initiator2target_buffer_size == sizeof(g_spleeb_config)) {
        memcpy(&g_spleeb_config, initiator2target_buffer, sizeof(g_spleeb_config));
    }
}

void keyboard_post_init_kb(void) {
    debug_enable = true;
    transaction_register_rpc(RPC_ID_KB_CONFIG_SYNC, spleeb_config_sync_handler);
    keyboard_post_init_user();
}

void housekeeping_task_kb(void) {
    if (is_keyboard_master()) {
        // Keep track of the last state, so that we can tell if we need to propagate to slave.
        static spleeb_config_t last_spleeb_config = {0};
        static uint32_t        last_sync          = 0;
        bool                   needs_sync         = false;

        // Check if the state values are different.
        if (memcmp(&g_spleeb_config, &last_spleeb_config, sizeof(g_spleeb_config))) {
            needs_sync = true;
            memcpy(&last_spleeb_config, &g_spleeb_config, sizeof(g_spleeb_config));
        }
        // Send to slave every 500ms regardless of state change.
        if (timer_elapsed32(last_sync) > 500) {
            needs_sync = true;
        }

        // Perform the sync if requested.
        if (needs_sync) {
            if (transaction_rpc_send(RPC_ID_KB_CONFIG_SYNC, sizeof(g_spleeb_config), &g_spleeb_config)) {
                last_sync = timer_read32();
            }
        }
    }
    // No need to invoke the user-specific callback, as it's been called
    // already.
}
#endif // defined(ENCODER_ENABLE) || defined(POINTER_ENABLE)

#ifdef ENCODER_ENABLE
static const char* get_encoder_mode_str(uint8_t mode) {
    switch (mode) {
        case ENCODER_DEFAULT_DPI:
            return "dft dpi";
        case ENCODER_SNIPING_DPI:
            return "snp dpi";
        case ENCODER_VOLUME:
            return "volume";
        default:
            return get_u8_str(mode, ' ');
    }
}

void step_spleeb_right_enc(spleeb_config_t* config) {
    config->enc_modes[1] = (config->enc_modes[1] + 1) % _NUM_R_MODES;
    write_spleeb_config_to_eeprom(config);
}

void step_spleeb_left_enc(spleeb_config_t* config) {
    config->enc_modes[0] = __ENC_L_START + (((config->enc_modes[0] - __ENC_L_START) + 1) % (_NUM_L_MODES - __ENC_L_START));
    write_spleeb_config_to_eeprom(config);
}

bool encoder_update_kb(uint8_t index, bool clockwise) {
    if (!encoder_update_user(index, clockwise)) {
        return false;
    }

    uint8_t enc_mode = g_spleeb_config.enc_modes[index];

    switch (index) {
        case 0:
            switch (enc_mode) {
                case ENCODER_VOLUME:
                    tap_code(clockwise ? KC_VOLU : KC_VOLD);
                    break;
                default:
                    break;
            }
            break;

        case 1:
            switch (enc_mode) {
#    ifdef POINTING_DEVICE_ENABLE
                case ENCODER_DEFAULT_DPI:
                    spleeb_cycle_pointer_default_dpi(clockwise);
                    break;
                case ENCODER_SNIPING_DPI:
                    spleeb_cycle_pointer_sniping_dpi(clockwise);
                    break;
#    endif // !POINTING_DEVICE_ENABLE
                default:
                    break;
            }
            break;
    }

    return true;
}
#endif // ENCODER_ENABLE

#ifdef POINTING_DEVICE_ENABLE
#    ifndef SPLEEB_MINIMUM_DEFAULT_DPI
#        define SPLEEB_MINIMUM_DEFAULT_DPI 300
#    endif // SPLEEB_MINIMUM_DEFAULT_DPI

#    ifndef SPLEEB_DEFAULT_DPI_CONFIG_STEP
#        define SPLEEB_DEFAULT_DPI_CONFIG_STEP 100
#    endif // SPLEEB_DEFAULT_DPI_CONFIG_STEP

#    ifndef SPLEEB_MINIMUM_SNIPING_DPI
#        define SPLEEB_MINIMUM_SNIPING_DPI 100
#    endif // SPLEEB_MINIMUM_SNIPING_DPI

#    ifndef SPLEEB_SNIPING_DPI_CONFIG_STEP
#        define SPLEEB_SNIPING_DPI_CONFIG_STEP 100
#    endif // SPLEEB_SNIPING_DPI_CONFIG_STEP

#    ifndef SPLEEB_DRAGSCROLL_DIVISOR
#        define SPLEEB_DRAGSCROLL_DIVISOR 64
#    endif // !SPLEEB_DRAGSCROLL_DIVISOR

/** \brief Return the current value of the pointer's default DPI. */
static uint16_t get_pointer_default_dpi(spleeb_config_t* config) {
    return (uint16_t)config->pointer_default_dpi * SPLEEB_DEFAULT_DPI_CONFIG_STEP + SPLEEB_MINIMUM_DEFAULT_DPI;
}

/** \brief Return the current value of the pointer's sniper-mode DPI. */
static uint16_t get_pointer_sniping_dpi(spleeb_config_t* config) {
    return (uint16_t)config->pointer_sniping_dpi * SPLEEB_SNIPING_DPI_CONFIG_STEP + SPLEEB_MINIMUM_SNIPING_DPI;
}

/** \brief Return the current value of the pointer's default DPI. */
static uint16_t get_pointer_current_dpi(spleeb_config_t* config) {
    // if (config->is_dragscroll_enabled) {
    //     return SPLEEB_DRAGSCROLL_DPI;
    // } else
    if (config->is_sniping_enabled) {
        return get_pointer_sniping_dpi(config);
    } else {
        return get_pointer_default_dpi(config);
    }
}

/** \brief Set the appropriate DPI for the input config. */
static void maybe_update_pointing_device_cpi(spleeb_config_t* config) {
    if (config->is_dragscroll_enabled) {
        // pointing_device_set_cpi(SPLEEB_DRAGSCROLL_DPI);
    } else if (config->is_sniping_enabled) {
        pointing_device_set_cpi(get_pointer_sniping_dpi(config));
    } else {
        pointing_device_set_cpi(get_pointer_default_dpi(config));
    }
}

/**
 * \brief Update the pointer's default DPI to the next or previous step.
 *
 * Increases the DPI value if `forward` is `true`, decreases it otherwise.
 * The increment/decrement steps are equal to SPLEEB_DEFAULT_DPI_CONFIG_STEP.
 */
static void step_pointer_default_dpi(spleeb_config_t* config, bool forward) {
    config->pointer_default_dpi += forward ? 1 : -1;
    maybe_update_pointing_device_cpi(config);
}

/**
 * \brief Update the pointer's sniper-mode DPI to the next or previous step.
 *
 * Increases the DPI value if `forward` is `true`, decreases it otherwise.
 * The increment/decrement steps are equal to SPLEEB_SNIPING_DPI_CONFIG_STEP.
 */
static void step_pointer_sniping_dpi(spleeb_config_t* config, bool forward) {
    config->pointer_sniping_dpi += forward ? 1 : -1;
    maybe_update_pointing_device_cpi(config);
}

uint16_t spleeb_get_pointer_default_dpi(void) {
    return get_pointer_default_dpi(&g_spleeb_config);
}

uint16_t spleeb_get_pointer_sniping_dpi(void) {
    return get_pointer_sniping_dpi(&g_spleeb_config);
}

void spleeb_cycle_pointer_default_dpi_noeeprom(bool forward) {
    step_pointer_default_dpi(&g_spleeb_config, forward);
}

void spleeb_cycle_pointer_default_dpi(bool forward) {
    step_pointer_default_dpi(&g_spleeb_config, forward);
    write_spleeb_config_to_eeprom(&g_spleeb_config);
}

void spleeb_cycle_pointer_sniping_dpi_noeeprom(bool forward) {
    step_pointer_sniping_dpi(&g_spleeb_config, forward);
}

void spleeb_cycle_pointer_sniping_dpi(bool forward) {
    step_pointer_sniping_dpi(&g_spleeb_config, forward);
    write_spleeb_config_to_eeprom(&g_spleeb_config);
}

bool spleeb_get_pointer_sniping_enabled(void) {
    return g_spleeb_config.is_sniping_enabled;
}

void spleeb_set_pointer_sniping_enabled(bool enable) {
    g_spleeb_config.is_sniping_enabled = enable;
    maybe_update_pointing_device_cpi(&g_spleeb_config);
}

bool spleeb_get_pointer_dragscroll_enabled(void) {
    return g_spleeb_config.is_dragscroll_enabled;
}

void spleeb_set_pointer_dragscroll_enabled(bool enable) {
    g_spleeb_config.is_dragscroll_enabled = enable;
    cirque_pinnacle_enable_cursor_glide(enable);
    maybe_update_pointing_device_cpi(&g_spleeb_config);
}
#endif // POINTING_DEVICE_ENABLE

#ifdef POINTING_DEVICE_ENABLE
void pointing_device_init_kb(void) {
    maybe_update_pointing_device_cpi(&g_spleeb_config);

    // only glide on drag scroll
    cirque_pinnacle_enable_cursor_glide(false);

    set_auto_mouse_enable(true);
    pointing_device_init_user();
}
#endif // POINTING_DEVICE_ENABLE

#ifdef POINTING_DEVICE_ENABLE
/**
 * \brief Augment the pointing device behavior.
 *
 * Implement drag-scroll.
 */
static void pointing_device_task_spleeb(report_mouse_t* mouse_report) {
    static int16_t scroll_x = 0;
    static int16_t scroll_y = 0;
    if (g_spleeb_config.is_dragscroll_enabled) {
        scroll_x -= mouse_report->x;
        scroll_y += mouse_report->y;
        mouse_report->h = scroll_x / SPLEEB_DRAGSCROLL_DIVISOR;
        mouse_report->v = scroll_y / SPLEEB_DRAGSCROLL_DIVISOR;
        mouse_report->x = 0;
        mouse_report->y = 0;
        scroll_x -= (int16_t)mouse_report->h * SPLEEB_DRAGSCROLL_DIVISOR;
        scroll_y -= (int16_t)mouse_report->v * SPLEEB_DRAGSCROLL_DIVISOR;
    }
}
#endif // POINTING_DEVICE_ENABLE

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
#ifdef POINTING_DEVICE_ENABLE
    if (is_keyboard_master()) {
        pointing_device_task_spleeb(&mouse_report);

        mouse_report = pointing_device_task_user(mouse_report);
    }
#endif // POINTING_DEVICE_ENABLE
    return mouse_report;
}

#ifdef POINTING_DEVICE_ENABLE
/**
 * \brief Outputs the Spleeb configuration to console.
 *
 * Prints the in-memory configuration structure to console, for debugging.
 * Includes:
 *   - raw value
 *   - drag-scroll: on/off
 *   - sniping: on/off
 *   - default DPI: internal table index/actual DPI
 *   - sniping DPI: internal table index/actual DPI
 */
static void debug_spleeb_config_to_console(spleeb_config_t* config) {
#    ifdef CONSOLE_ENABLE
    dprintf("(spleeb) process_record_kb: config = {\n"
            "\traw = 0x%lu,\n"
            "\t{\n"
            "\t\tis_dragscroll_enabled=%u\n"
            "\t\tis_sniping_enabled=%u\n"
            "\t\tdefault_dpi=0x%X (%u)\n"
            "\t\tsniping_dpi=0x%X (%u)\n"
            "\t}\n"
            "}\n",
            config->raw, config->is_dragscroll_enabled, config->is_sniping_enabled, config->pointer_default_dpi, get_pointer_default_dpi(config), config->pointer_sniping_dpi, get_pointer_sniping_dpi(config));
#    endif // CONSOLE_ENABLE
}
#endif // POINTING_DEVICE_ENABLE

bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
    if (!process_record_user(keycode, record)) {
#ifdef POINTING_DEVICE_ENABLE

        debug_spleeb_config_to_console(&g_spleeb_config);
#endif // POINTING_DEVICE_ENABLE
        return false;
    }
#ifdef POINTING_DEVICE_ENABLE
    switch (keycode) {
        case POINTER_DEFAULT_DPI_FORWARD:
            if (record->event.pressed) {
                spleeb_cycle_pointer_default_dpi(true);
            }
            break;
        case POINTER_DEFAULT_DPI_REVERSE:
            if (record->event.pressed) {
                spleeb_cycle_pointer_default_dpi(false);
            }
            break;
        case POINTER_SNIPING_DPI_FORWARD:
            if (record->event.pressed) {
                spleeb_cycle_pointer_sniping_dpi(true);
            }
            break;
        case POINTER_SNIPING_DPI_REVERSE:
            if (record->event.pressed) {
                spleeb_cycle_pointer_sniping_dpi(false);
            }
            break;
        case SNIPING_MODE:
            spleeb_set_pointer_sniping_enabled(record->event.pressed);
            break;
        case SNIPING_MODE_TOGGLE:
            if (record->event.pressed) {
                spleeb_set_pointer_sniping_enabled(!spleeb_get_pointer_sniping_enabled());
            }
            break;
        case DRAGSCROLL_MODE:
            spleeb_set_pointer_dragscroll_enabled(record->event.pressed);
            break;
        case DRAGSCROLL_MODE_TOGGLE:
            if (record->event.pressed) {
                spleeb_set_pointer_dragscroll_enabled(!spleeb_get_pointer_dragscroll_enabled());
            }
            break;
    }
#endif // POINTING_DEVICE_ENABLE

#ifdef ENCODER_ENABLE
    switch (keycode) {
        case ENC_MODE_LEFT_TOGGLE:
            if (record->event.pressed) {
                step_spleeb_left_enc(&g_spleeb_config);
            }
            break;
        case ENC_MODE_RIGHT_TOGGLE:
            if (record->event.pressed) {
                step_spleeb_right_enc(&g_spleeb_config);
            }
            break;
    }
#endif // ENCODER_ENABLE

#ifdef POINTING_DEVICE_ENABLE
    if ((keycode >= POINTER_DEFAULT_DPI_FORWARD && keycode < SPLEEB_SAFE_RANGE) || IS_MOUSEKEY(keycode)) {
        debug_spleeb_config_to_console(&g_spleeb_config);
    }
#endif // POINTING_DEVICE_ENABLE

    return true;
}

#ifdef POINTING_DEVICE_ENABLE

bool is_mouse_record_kb(uint16_t keycode, keyrecord_t* record) {
    switch (keycode) {
        case DRAGSCROLL_MODE:
        case SNIPING_MODE:
            return true;
        default:
            return false;
    }

    return is_mouse_record_user(keycode, record);
}

#endif // POINTING_DEVICE_ENABLE

#ifdef OLED_ENABLE

static void render_status(void) {
    oled_write_P(PSTR("LAYER: "), false);

    switch (get_highest_layer(layer_state)) {
        case 0:
            oled_write_ln_P(PSTR("\xC0\xC1"), false);
            break;
        case 1:
            oled_write_ln_P(PSTR("\xC2\xC3"), false);
            break;
        case 2:
            oled_write_ln_P(PSTR("\xC4\xC5"), false);
            break;
        case 3:
            oled_write_ln_P(PSTR("\xC6\xC7"), false);
            break;
        case 4:
            oled_write_ln_P(PSTR("\xC8\xC9"), false);
            break;
        case 5:
            oled_write_ln_P(PSTR("\xCA\xCB"), false);
            break;
        default:
            // Or use the write_ln shortcut over adding '\n' to the end of your string
            oled_write_char(get_highest_layer(layer_state) + 0x30, true);
    }

    oled_write_ln_P("", false);

    uint8_t modifiers = get_mods();
    led_t   led_state = host_keyboard_led_state();
    oled_write_ln_P(PSTR("MODS:"), false);

    oled_write_ln_P("", false);

    oled_write_P(PSTR("\325\326"), (modifiers & MOD_MASK_SHIFT));
    oled_write_P(PSTR("\327\330"), (modifiers & MOD_MASK_CTRL));
    oled_write_P(PSTR("\331\332"), (modifiers & MOD_MASK_ALT));
    oled_write_ln_P(PSTR("\333\334"), (modifiers & MOD_MASK_GUI));

    oled_write_ln_P("", false);

    oled_write_P(PSTR("LOCK: "), false);
    oled_write_P(PSTR("\235\236"), led_state.caps_lock);
    oled_write_ln_P(PSTR("\275\276"), led_state.num_lock);

#    ifdef POINTING_DEVICE_ENABLE
    oled_write_ln_P(PSTR("POINTER:"), false);

    oled_write_ln_P("", false);

    oled_write_P(PSTR("dpi:"), false);
    oled_write_ln_P(get_u16_str(get_pointer_current_dpi(&g_spleeb_config), ' '), false);
#    endif // POINTING_DEVICE_ENABLE

    oled_write_ln_P("", false);
    oled_write_ln_P(PSTR("ENCODER:"), false);

#    ifdef ENCODER_ENABLE
    oled_write_ln_P("", false);
    oled_write_P(PSTR("R: "), false);
    oled_write_P(get_encoder_mode_str(g_spleeb_config.enc_modes[1]), false);
    oled_write_P(PSTR("L: "), false);
    oled_write_ln_P(get_encoder_mode_str(g_spleeb_config.enc_modes[0]), false);
#    endif // ENCODER_ENABLE
}

oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    return OLED_ROTATION_90; // rotates for proper orientation
}

bool oled_task_kb(void) {
    if (is_keyboard_master()) {
        return false;
    }

    if (!oled_task_user()) {
        return false;
    }

    render_status();
    return false;
}
#endif // OLED_ENABLE

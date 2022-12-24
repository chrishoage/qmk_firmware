// Copyright 2022 Chris Hoage (@chrishoage)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

void enter_qk_boot(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count >= 2) {
        reset_keyboard();
        reset_tap_dance(state);
    }
}

enum { _BASE = 0, _FN, _MOUSE };

// Tap Dance Definitions
qk_tap_dance_action_t tap_dance_actions[] = {
    // Tap once for Esc, twice for Caps Lock
    [0] = ACTION_TAP_DANCE_FN(enter_qk_boot)
    // Other declarations would go here, separated by commas, if you have them
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

	[_BASE] = LAYOUT(
         KC_GRV,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,  KC_ESC,                                          KC_BSPC,    KC_6,    KC_7,    KC_8,   KC_9,    KC_0, KC_BSLS,
         KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T, KC_MINS,                                           KC_EQL,    KC_Y,    KC_U,    KC_I,   KC_O,    KC_P, KC_QUOT,
        KC_LSFT,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G, KC_LBRC,                                          KC_RBRC,    KC_H,    KC_J,    KC_K,   KC_L, KC_SCLN, KC_RSFT,
                    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,          KC_MUTE,                        ENC_TGR,             KC_N,    KC_M, KC_COMM, KC_DOT, KC_SLSH,
                                            KC_LCTL, KC_LALT, KC_LGUI,  KC_ENT,   MO(1),        MO(1),  KC_SPC, KC_RGUI, KC_RALT, KC_RCTL
    ),

	[_FN] = LAYOUT(
        KC_PAUS,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,  KC_ESC,                                           KC_DEL,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10, KC_PSCR,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                                          KC_TRNS, KC_HOME, KC_PGDN, KC_PGUP,  KC_END, KC_TRNS, KC_TRNS,
        KC_CAPS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                                          KC_TRNS, KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, KC_TRNS,  QK_RBT,
                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          ENC_TGL,                        KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,   TD(0),
                                            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

	[_MOUSE] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                                           KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                                           KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        DRGSCRL, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, SNIPING,                                           KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_BTN3,          KC_TRNS,                         KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                            KC_TRNS, KC_TRNS, KC_TRNS, KC_BTN1, KC_BTN2,       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
};
// clang-format on

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (get_mods() & MOD_MASK_GUI) {
        tap_code(clockwise ? KC_RBRC : KC_LBRC);
        return false;
    }

    if (get_mods() & MOD_MASK_CTRL) {
        tap_code(clockwise ? KC_PGDN : KC_PGUP);
        return false;
    }

    if (get_mods() & MOD_MASK_ALT) {
        tap_code(clockwise ? KC_DOWN : KC_UP);
        return false;
    }

    return true;
}

void pointing_device_init_user(void) {
    set_auto_mouse_layer(_MOUSE);
}

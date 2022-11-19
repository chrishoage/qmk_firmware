#include QMK_KEYBOARD_H
#include "print.h"
#include "usb_device_state.h"
#include <stdio.h>

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {LAYOUT_ortho_1x1(KC_A)};

void suspend_power_down_user(void) {
    print("suspend_power_down_user");
}

void suspend_wakeup_init_user(void) {
    print("suspend_wakeup_init_user");
}

void notify_usb_device_state_change_user(enum usb_device_state usb_device_state) {
    printf("notify_usb_device_state_change_user %u", usb_device_state);
};

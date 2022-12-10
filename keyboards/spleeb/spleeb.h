// Copyright 2022 Chris Hoage (@chrishoage)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "quantum.h"

enum spleeb_keycodes {
#ifdef POINTING_DEVICE_ENABLE
#    ifdef VIA_ENABLE
    POINTER_DEFAULT_DPI_FORWARD = USER00,
#    else
    POINTER_DEFAULT_DPI_FORWARD = SAFE_RANGE,
#    endif // VIA_ENABLE
    POINTER_DEFAULT_DPI_REVERSE,
    POINTER_SNIPING_DPI_FORWARD,
    POINTER_SNIPING_DPI_REVERSE,
    SNIPING_MODE,
    SNIPING_MODE_TOGGLE,
    DRAGSCROLL_MODE,
    DRAGSCROLL_MODE_TOGGLE,
#endif // POINTING_DEVICE_ENABLE
#ifdef ENCODER_ENABLE
    ENC_MODE_LEFT_TOGGLE,
    ENC_MODE_RIGHT_TOGGLE,
#endif // ENCODER_ENABLE
    SPLEEB_SAFE_RANGE,
};

#ifdef POINTING_DEVICE_ENABLE
#    define DF_MOD POINTER_DEFAULT_DPI_FORWARD
#    define DF_RMOD POINTER_DEFAULT_DPI_REVERSE
#    define SP_MOD POINTER_SNIPING_DPI_FORWARD
#    define SP_RMOD POINTER_SNIPING_DPI_REVERSE
#    define SNIPING SNIPING_MODE
#    define SNP_TOG SNIPING_MODE_TOGGLE
#    define DRGSCRL DRAGSCROLL_MODE
#    define DRG_TOG DRAGSCROLL_MODE_TOGGLE
#endif // POINTING_DEVICE_ENABLE
#ifdef ENCODER_ENABLE
#    define ENC_TGL ENC_MODE_LEFT_TOGGLE
#    define ENC_TGR ENC_MODE_RIGHT_TOGGLE
#endif // ENCODER_ENABLE

#ifdef POINTING_DEVICE_ENABLE

/** \brief Return the current DPI value for the pointer's default mode. */
uint16_t spleeb_get_pointer_default_dpi(void);

/**
 * \brief Update the pointer's default DPI to the next or previous step.
 *
 * Increases the DPI value if `forward` is `true`, decreases it otherwise.
 * The increment/decrement steps are equal to SPLEEB_DEFAULT_DPI_CONFIG_STEP.
 *
 * The new value is persisted in EEPROM.
 */
void spleeb_cycle_pointer_default_dpi(bool forward);

/**
 * \brief Same as `spleeb_cycle_pointer_default_dpi`, but do not write to
 * EEPROM.
 *
 * This means that reseting the board will revert the value to the last
 * persisted one.
 */
void spleeb_cycle_pointer_default_dpi_noeeprom(bool forward);

/** \brief Return the current DPI value for the pointer's sniper-mode. */
uint16_t spleeb_get_pointer_sniping_dpi(void);

/**
 * \brief Update the pointer's sniper-mode DPI to the next or previous step.
 *
 * Increases the DPI value if `forward` is `true`, decreases it otherwise.
 * The increment/decrement steps are equal to SPLEEB_SNIPING_DPI_CONFIG_STEP.
 *
 * The new value is persisted in EEPROM.
 */
void spleeb_cycle_pointer_sniping_dpi(bool forward);

/**
 * \brief Same as `spleeb_cycle_pointer_sniping_dpi`, but do not write to
 * EEPROM.
 *
 * This means that reseting the board will revert the value to the last
 * persisted one.
 */
void spleeb_cycle_pointer_sniping_dpi_noeeprom(bool forward);

/** \brief Whether sniper-mode is enabled. */
bool spleeb_get_pointer_sniping_enabled(void);

/**
 * \brief Enable/disable sniper mode.
 *
 * When sniper mode is enabled the dpi is reduced to slow down the pointer for
 * more accurate movements.
 */
void spleeb_set_pointer_sniping_enabled(bool enable);

/** \brief Whether drag-scroll is enabled. */
bool spleeb_get_pointer_dragscroll_enabled(void);

/**
 * \brief Enable/disable drag-scroll mode.
 *
 * When drag-scroll mode is enabled, horizontal and vertical pointer movements
 * are translated into horizontal and vertical scroll movements.
 */
void spleeb_set_pointer_dragscroll_enabled(bool enable);
#endif // POINTING_DEVICE_ENABLE

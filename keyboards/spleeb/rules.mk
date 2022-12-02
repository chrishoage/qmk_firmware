# MCU name
# MCU = RP2040

# Board (build with Blok but any promicro pinout RP2040 should work)
BOARD = QMK_PM2040

# Bootloader selection
# BOOTLOADER = rp2040

SERIAL_DRIVER = vendor
# WS2812_DRIVER = vendor

# Build Options
#   change yes to no to disable
#
# BOOTMAGIC_ENABLE = yes      # Enable Bootmagic Lite
# MOUSEKEY_ENABLE = yes        # Mouse keys
# EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = yes         # Console for debug
# COMMAND_ENABLE = no         # Commands for debug and configuration
# NKRO_ENABLE = no            # Enable N-Key Rollover
# BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
# RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
# AUDIO_ENABLE = no           # Audio output
# AUDIO_SUPPORTED = no        # Audio is not supported
# ENCODER_ENABLE = yes
# ENCODER_MAP_ENABLE = yes
# SPLIT_KEYBOARD = yes
TAP_DANCE_ENABLE = yes

POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = cirque_pinnacle_i2c

OLED_ENABLE = yes
OLED_DRIVER = SSD1306

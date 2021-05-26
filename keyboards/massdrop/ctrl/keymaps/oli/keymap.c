#include QMK_KEYBOARD_H
#include "printf.h"

enum ctrl_keycodes {
    U_T_AUTO = SAFE_RANGE, //USB Extra Port Toggle Auto Detect / Always Active
    U_T_AGCR,              //USB Toggle Automatic GCR control
    DBG_TOG,               //DEBUG Toggle On / Off
    DBG_MTRX,              //DEBUG Toggle Matrix Prints
    DBG_KBD,               //DEBUG Toggle Keyboard Prints
    DBG_MOU,               //DEBUG Toggle Mouse Prints
    MD_BOOT,               //Restart into bootloader after hold timeout

    TEST_OUTPUT_LAYER,
};

enum layout_names {
    LAYOUT_BASE_DVORAK = 0,
    LAYOUT_STANDARD_FN = 1,
    LAYOUT_BASE_USANSI = 2,
    LAYOUT_INLINE_CURSOR = 3,
};

#define MILLISECONDS_IN_SECOND 1000


// Some colors

#define RED {HSV_RED}
#define CORAL {HSV_CORAL}
#define ORANGE {HSV_ORANGE}
#define GOLDEN {HSV_GOLDENROD}
#define GOLD {HSV_GOLD}
#define YELLOW {HSV_YELLOW}
#define CHART {HSV_CHARTREUSE}
#define GREEN {HSV_GREEN}
#define SPRING {HSV_SPRINGGREEN}
#define TURQ {HSV_TURQUOISE}
#define TEAL {HSV_TEAL}
#define CYAN {HSV_CYAN}
#define AZURE {HSV_AZURE}
#define BLUE {HSV_BLUE}
#define PURPLE {HSV_PURPLE}
#define MAGENT {HSV_MAGENTA}
#define PINK {HSV_PINK}

static uint16_t idle_timer;             // Idle LED timeout timer
static uint8_t idle_second_counter;     // Idle LED seconds counter, counts seconds not milliseconds
static uint8_t key_event_counter;       // This counter is used to check if any keys are being held

#define RGB_DEFAULT_TIME_OUT 30
#define RGB_FAST_MODE_TIME_OUT 3
#define RGB_TIME_OUT_MAX 600
#define RGB_TIME_OUT_MIN 10
#define RGB_TIME_OUT_STEP 10


extern bool g_suspend_state;
extern rgb_config_t rgb_matrix_config;
bool disable_layer_color;

bool rgb_enabled_flag;                  // Current LED state flag. If false then LED is off.
bool rgb_time_out_enable;               // Idle LED toggle enable. If false then LED will not turn off after idle timeout.
bool rgb_time_out_fast_mode_enabled;    // Enable flag for RGB timeout fast mode
bool rgb_time_out_user_value;           // This holds the toggle value set by user with ROUT_TG. It's necessary as RGB_TOG changes timeout enable.
uint16_t rgb_time_out_seconds;          // Idle LED timeout value, in seconds not milliseconds
uint16_t rgb_time_out_saved_seconds;    // The saved user config for RGB timeout period
led_flags_t rgb_time_out_saved_flag;    // Store LED flag before timeout so it can be restored when LED is turned on again.

enum tapdance_keycodes {
    TD_CAPS_CURSOR
};

void caps_cursor_tapdance_finished(qk_tap_dance_state_t *state, void * user_data);
void caps_cursor_tapdance_reset(qk_tap_dance_state_t * state, void * user_data);

qk_tap_dance_action_t tap_dance_actions[] = {
    //[TD_CAPS_CURSOR] = ACTION_TAP_DANCE_DOUBLE(KC_LCTL, MO(LAYOUT_INLINE_CURSOR)),
    [TD_CAPS_CURSOR] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, caps_cursor_tapdance_finished, caps_cursor_tapdance_reset),
};



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYOUT_BASE_DVORAK] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,             KC_PSCR, KC_PSCR, KC_PSCR, \
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_LBRC, KC_RBRC, KC_BSPC,   KC_HOME,  KC_HOME, KC_PGUP, \
        KC_TAB,  KC_QUOT, KC_COMM, KC_DOT,  KC_P,    KC_Y,    KC_F,    KC_G,    KC_C,    KC_R,    KC_L,    KC_SLSH, KC_EQL,  KC_BSLS,   KC_DEL,  KC_END,  KC_PGDN, \
        TD(TD_CAPS_CURSOR), KC_A,    KC_O,    KC_E,    KC_U,    KC_I,    KC_D,    KC_H,    KC_T,    KC_N,    KC_S,    KC_MINS, KC_ENT, \
        KC_LSPO, KC_SCLN, KC_Q,    KC_J,    KC_K,    KC_X,    KC_B,    KC_M,    KC_W,    KC_V,    KC_Z,    KC_RSPC,                              KC_UP, \
        KC_LCTL, KC_LGUI, KC_LALT,                   KC_SPC,                             KC_RALT, MO(LAYOUT_STANDARD_FN),   /*TEST_OUTPUT_LAYER*/KC_APP,  KC_RCTL,            KC_LEFT, KC_DOWN, KC_RGHT \
    ),
    [LAYOUT_STANDARD_FN] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, TO(LAYOUT_BASE_USANSI) /*F12*/,              _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, KC_VOLU /*PgUp*/, \
        _______, _______, _______, _______, _______, _______, _______, _______,_______,_______, _______, _______, _______, _______,   KC_MPRV, KC_MNXT, KC_VOLD/*PgDn*/, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
        _______, _______, _______, _______, _______, MD_BOOT /*B*/, _______, _______, _______, _______, _______, _______,                              _______, \
        _______, _______, _______,                   _______,                            _______, _______, _______, _______,            _______, _______, _______ \
    ),

    /* US ANSI LAYOUT - for special cases */
    [LAYOUT_BASE_USANSI] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,             KC_PSCR, KC_SLCK, KC_PAUS, \
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,   KC_INS,  KC_HOME, KC_PGUP, \
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,   KC_DEL,  KC_END,  KC_PGDN, \
        KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT, \
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,                              KC_UP, \
        KC_LCTL, KC_LGUI, KC_LALT,                   KC_SPC,                             KC_RALT, TG(LAYOUT_BASE_USANSI),   KC_APP,  KC_RCTL,            KC_LEFT, KC_DOWN, KC_RGHT \
    ),

    [LAYOUT_INLINE_CURSOR] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,              _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, KC_PGUP, KC_HOME, KC_UP/*I*/ ,KC_END, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______, _______, \
        KC_LSFT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_RSFT,                              _______, \
        KC_LCTL, _______, _______,                   _______,                            _______, _______, _______, KC_RCTL,            _______, _______, _______ \
    ),

    /*
    [X] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, NK_TOGG, _______, _______, _______, _______, _______,                              _______, \
        _______, _______, _______,                   _______,                            _______, _______, _______, _______,            _______, _______, _______ \
    ),
    */
};


#ifdef _______
#undef _______
#define _______ {0, 0, 0}

const uint8_t PROGMEM ledmap[][DRIVER_LED_TOTAL][3] = {
    [LAYOUT_STANDARD_FN] = {
        // 16 keys
        _______, _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______, _______,  YELLOW /*F12*/, _______, _______, _______,

        // 17 keys
        _______, _______, _______, _______, _______, _______,    _______, _______, _______, _______, _______, _______,   _______, _______, _______, _______, YELLOW /*PgUp*/,

        // 17 keys
        _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______,   _______, _______, _______, _______, _______, YELLOW /*Del*/,  YELLOW /*End*/,  YELLOW /*PgDn*/,

        // 13 keys
        _______, _______, _______, _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______,

        // 13 keys
        _______, _______,  _______, _______,   _______, YELLOW /*B*/, _______,   _______, _______, _______, _______, _______,                         _______,

        // 11 keys
        _______, _______, _______,                   _______,                            _______, _______,    _______, _______,            _______, _______, _______,

        // outer LEDs (32)
        YELLOW, YELLOW, YELLOW, 
        YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, 
        YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, 
        YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, 
        YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, 
        YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, 
        YELLOW, YELLOW, YELLOW, YELLOW, // 119
    },
    [LAYOUT_BASE_USANSI] = {
        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, 

        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, ORANGE, 

        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, ORANGE, 

        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, ORANGE, ORANGE, 

        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, ORANGE, ORANGE, 

        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        RED /*Fn*/, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, 

        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE,  
        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE,  
        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE,  
        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE,  
        ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, 
        ORANGE, ORANGE, 

    },

    [LAYOUT_INLINE_CURSOR] = {
        // 16 keys
        _______, _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______, _______,  _______, _______, _______, _______,

        // 17 keys
        _______, _______, _______, _______, _______, _______,    _______, _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______,

        // 17 keys
        _______,  _______,  _______,  _______, _______,  _______,  CYAN, CYAN, CYAN, CYAN, _______, _______, _______, _______, _______,  _______,  _______,

        // 13 keys
        _______, _______, _______, _______, _______, _______,  CYAN, CYAN, CYAN, CYAN, _______, _______, _______,

        // 13 keys
        CYAN, _______,  _______, _______,   _______, _______, _______,   _______, _______, _______, _______, CYAN,                         _______,

        // 11 keys
        CYAN, _______, _______,                   _______,                            _______, _______,    _______, CYAN,            _______, _______, _______,

        // outer LEDs (32)
        CYAN, CYAN, CYAN, 
        CYAN, CYAN, CYAN, CYAN, CYAN, 
        CYAN, CYAN, CYAN, CYAN, CYAN, 
        CYAN, CYAN, CYAN, CYAN, CYAN, 
        CYAN, CYAN, CYAN, CYAN, CYAN, 
        CYAN, CYAN, CYAN, CYAN, CYAN, 
        CYAN, CYAN, CYAN, CYAN,  // 119
    },
};

#undef _______
#define _______ KC_TRNS
#endif


// Runs just one time when the keyboard initializes.
void matrix_init_user(void) {
    // Oli: deactivating this for now - it does not work.
    // Whenever anything is printed, the keyboard hangs.
    // I'm not sure how this is supposed to work.
    // This issue claims it does not work at all: 
    // https://github.com/qmk/qmk_firmware/issues/3808
    // Instead I'm supposed to use a serial terminal,
    // but I don't have any available port on my machine - 
    // not that I remember the  first thing about how to connect
    // to serial ports. Oh well. Then again, perhaps things
    // have changed - I find it peculiar that the standard
    // config of the keyboard has all sorts of debug commands
    // and then it just hangs? Odd.
    debug_enable = false;

    idle_second_counter = 0;                            // Counter for number of seconds keyboard has been idle.
    key_event_counter = 0;                              // Counter to determine if keys are being held, neutral at 0.
    rgb_time_out_seconds = RGB_DEFAULT_TIME_OUT;        // RGB timeout initialized to its default configure in keymap.h
    rgb_time_out_enable = false;                        // Disable RGB timeout by default. Enable using toggle key.
    rgb_time_out_user_value = false;                    // Has to have the same initial value as rgb_time_out_enable.
    rgb_enabled_flag = true;                            // Initially, keyboard RGB is enabled. Change to false config.h initializes RGB disabled.
    rgb_time_out_fast_mode_enabled = false;             // RGB timeout fast mode disabled initially.
    rgb_time_out_saved_flag = rgb_matrix_get_flags();   // Save RGB matrix state for when keyboard comes back from ide.
};

void keyboard_post_init_user(void) {
    rgb_matrix_enable();
}


// Runs constantly in the background, in a loop.
void matrix_scan_user(void) {
    if(rgb_time_out_enable && rgb_enabled_flag) {
        // If the key event counter is not zero then some key was pressed down but not released, thus reset the timeout counter.
        if (key_event_counter) {
            idle_second_counter = 0;
        } else if (timer_elapsed(idle_timer) > MILLISECONDS_IN_SECOND) {
            idle_second_counter++;
            idle_timer = timer_read();
        }

        if (idle_second_counter >= rgb_time_out_seconds) {
            rgb_time_out_saved_flag = rgb_matrix_get_flags();
            rgb_matrix_set_flags(LED_FLAG_NONE);
            rgb_matrix_disable_noeeprom();
            rgb_enabled_flag = false;
            idle_second_counter = 0;
        }
    }
};

#define MODS_SHIFT  (get_mods() & MOD_MASK_SHIFT)
#define MODS_CTRL   (get_mods() & MOD_MASK_CTRL)
#define MODS_ALT    (get_mods() & MOD_MASK_ALT)

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint32_t key_timer;

    // Increment key event counter for every press and decrement for every release.
    if (record->event.pressed) {
        key_event_counter++;
    } else {
        key_event_counter--;
    }


    if (rgb_time_out_enable) {
        idle_timer = timer_read();
        // Reset the seconds counter. Without this, something like press> leave x seconds> press, would be x seconds on the effective counter not 0 as it should.
        idle_second_counter = 0;
        if (!rgb_enabled_flag) {
            rgb_matrix_enable_noeeprom();
            rgb_matrix_set_flags(rgb_time_out_saved_flag);
            rgb_enabled_flag = true;
        }
    }


    switch (keycode) {
        case TEST_OUTPUT_LAYER:
            if (record->event.pressed) {
                int layer = get_highest_layer(layer_state);
                char layerS[10];
                snprintf(layerS, 10, "%d", layer);
                SEND_STRING("Highest layer: ");
                send_string(layerS);
            }
            return false;


        case U_T_AUTO:
            if (record->event.pressed && MODS_SHIFT && MODS_CTRL) {
                TOGGLE_FLAG_AND_PRINT(usb_extra_manual, "USB extra port manual mode");
            }
            return false;
        case U_T_AGCR:
            if (record->event.pressed && MODS_SHIFT && MODS_CTRL) {
                TOGGLE_FLAG_AND_PRINT(usb_gcr_auto, "USB GCR auto mode");
            }
            return false;
        case DBG_TOG:
            if (record->event.pressed) {
                // Not doing this - see comment above
                //TOGGLE_FLAG_AND_PRINT(debug_enable, "Debug mode");
            }
            return false;
        case DBG_MTRX:
            if (record->event.pressed) {
                TOGGLE_FLAG_AND_PRINT(debug_matrix, "Debug matrix");
            }
            return false;
        case DBG_KBD:
            if (record->event.pressed) {
                TOGGLE_FLAG_AND_PRINT(debug_keyboard, "Debug keyboard");
            }
            return false;
        case DBG_MOU:
            if (record->event.pressed) {
                TOGGLE_FLAG_AND_PRINT(debug_mouse, "Debug mouse");
            }
            return false;
        case MD_BOOT:
            if (record->event.pressed) {
                key_timer = timer_read32();
            } else {
                if (timer_elapsed32(key_timer) >= 500) {
                    reset_keyboard();
                }
            }
            return false;
        // case RGB_TOG:
        //     if (record->event.pressed) {
        //       switch (rgb_matrix_get_flags()) {
        //         case LED_FLAG_ALL: {
        //             rgb_matrix_set_flags(LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER | LED_FLAG_INDICATOR);
        //             rgb_matrix_set_color_all(0, 0, 0);
        //           }
        //           break;
        //         case (LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER | LED_FLAG_INDICATOR): {
        //             rgb_matrix_set_flags(LED_FLAG_UNDERGLOW);
        //             rgb_matrix_set_color_all(0, 0, 0);
        //           }
        //           break;
        //         case LED_FLAG_UNDERGLOW: {
        //             rgb_matrix_set_flags(LED_FLAG_NONE);
        //             rgb_matrix_disable_noeeprom();
        //           }
        //           break;
        //         default: {
        //             rgb_matrix_set_flags(LED_FLAG_ALL);
        //             rgb_matrix_enable_noeeprom();
        //           }
        //           break;
        //       }
        //     }
        //     return false;
        default:
            return true; //Process all other keycodes normally
    }
}

// Oli: I have no idea why we do all this calculating here
// since RGB colors are also supported.
// https://beta.docs.qmk.fm/using-qmk/hardware-features/lighting/feature_rgb_matrix#colors-id-colors

void set_layer_color(int layer) {
    // Don't uprintf here - it hangs.
    //uprintf("Oli: set_layer_color(%d)\n", layer);
    if (layer == 0) { return; }
    for (int i = 0; i < DRIVER_LED_TOTAL; i++) {
        HSV hsv = {
            .h = pgm_read_byte(&ledmap[layer][i][0]),
            .s = pgm_read_byte(&ledmap[layer][i][1]),
            .v = pgm_read_byte(&ledmap[layer][i][2]),
        };
        if (hsv.h || hsv.s || hsv.v) {
            RGB rgb = hsv_to_rgb(hsv);
            float f = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
            rgb_matrix_set_color(i, f * rgb.r, f * rgb.g, f * rgb.b);
        } else /*if (layer != 1)*/ {
            rgb_matrix_set_color(i, 0, 0, 0);
        }
    }
}

void rgb_matrix_indicators_user(void) {
    if (g_suspend_state || disable_layer_color ||
        rgb_matrix_get_flags() == LED_FLAG_NONE ||
        rgb_matrix_get_flags() == LED_FLAG_UNDERGLOW) {
            return;
        }
    set_layer_color(get_highest_layer(layer_state));
}

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
} td_state_t;

typedef struct {
    // Oli: I have no idea at all what this field is for - 
    // copied from doc samples. It does not seem to be used.
    bool is_press_action;
    td_state_t state;
} td_tap_t;

static td_tap_t cctd_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

td_state_t cur_dance(qk_tap_dance_state_t * state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        else return TD_SINGLE_HOLD;
    }
    else if (state->count == 2) { 
        if (state->interrupted || !state->pressed) return TD_DOUBLE_TAP;
        else return TD_DOUBLE_HOLD;
    }
    else return TD_UNKNOWN;
}

void caps_cursor_tapdance_finished(qk_tap_dance_state_t *state, void * user_data) {
    cctd_tap_state.state = cur_dance(state);
    switch (cctd_tap_state.state) {
        case TD_SINGLE_TAP:
            // nothing
            break;
        case TD_SINGLE_HOLD:
            // set modifier control
            register_code(KC_LCTL);
            break;
        case TD_DOUBLE_TAP:
            // activate layer permanently - but how do I get out?
            // maybe just toggle the layer, then I can switch
            // it off the same way
            if (layer_state_is(LAYOUT_INLINE_CURSOR))
                layer_off(LAYOUT_INLINE_CURSOR);
            else
                layer_on(LAYOUT_INLINE_CURSOR);
            break;
        case TD_DOUBLE_HOLD:
            layer_on(LAYOUT_INLINE_CURSOR);
            break;
        case TD_NONE:
        case TD_UNKNOWN:
            break;
    }
}

void caps_cursor_tapdance_reset(qk_tap_dance_state_t * state, void * user_data) {
    if (cctd_tap_state.state == TD_SINGLE_HOLD) {
        unregister_code(KC_LCTL);
    }
    else if (cctd_tap_state.state == TD_DOUBLE_HOLD) {
        layer_off(LAYOUT_INLINE_CURSOR);
    }    
    cctd_tap_state.state = TD_NONE;
}

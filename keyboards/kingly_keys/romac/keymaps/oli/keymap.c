
#include QMK_KEYBOARD_H
#include "sendstring_dvorak.h"

// OL - Oli Layer
// I decided to go with numeric names for the layers, so they
// are easily associated with the keys the keypad has.
enum layers {
  OL_LAYER_BASE = 0,
  OL_LAYER_1,
  OL_LAYER_2,
  OL_LAYER_3
};

// OK - Oli Key
enum keys {
  OK_CODEBLOCK = SAFE_RANGE,
};

const char * LAYER_INFO[] = {
  "Base layer",
  "1. Clipboard",
  "2. Navigation",
  "3. Text Entry",
  "4. ",
  "5. ",
  "6. ",
  "7. ",
  "8. ",
  "9. ",
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  // My idea is to collect keys on the base layer that are
  // also part of (an)other layer(s), but that I need frequently
  // enough so they should be right there without switching.
  // KC_LEAD must obviously remain here, but the other ones
  // are not finalized and I can switch them - just be sure
  // to coordinate with the other layers.
	[OL_LAYER_BASE] = LAYOUT(
		KC_KP_7, KC_KP_8, OK_CODEBLOCK, \
		KC_KP_4, KC_KP_5, KC_KP_6, \
		KC_KP_1, KC_KP_2, KC_KP_3, \
		KC_LEAD, KC_KP_0, KC_KP_DOT \
	),

	[OL_LAYER_1] = LAYOUT(
		C(KC_I), KC_TRNS, C(KC_DOT), \
		KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, C(KC_B), KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS \
	),

	[OL_LAYER_2] = LAYOUT(
		KC_TRNS, KC_HOME, KC_PGUP, \
		KC_TRNS, KC_END, KC_PGDN, \
		KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_ENT \
	),

  // Inherit from base:
  // * OK_CODEBLOCK
	[OL_LAYER_3] = LAYOUT(
		KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS \
	)
};

int currentLayerIndex = 0;

void keyboard_post_init_user(void) {
  debug_enable = true;
  // debug_matrix = true;
  // debug_keyboard = true;
  // debug_mouse = true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
  currentLayerIndex = get_highest_layer(state);
  return state;
}

LEADER_EXTERNS();

void matrix_scan_user(void) {
  LEADER_DICTIONARY() {
    leading = false;
    leader_end();

    // Switch to numbered layers
    SEQ_ONE_KEY(KC_KP_1) {
      uprintf("OLI: Go to layer 1\n");
      layer_move(OL_LAYER_1);
    }
    SEQ_ONE_KEY(KC_KP_2) {
      uprintf("OLI: Go to layer 2\n");
      layer_move(OL_LAYER_2);
    }
    SEQ_ONE_KEY(KC_KP_3) {
      uprintf("OLI: Go to layer 3\n");
    }
    SEQ_ONE_KEY(KC_KP_4) {
      uprintf("OLI: Go to layer 4\n");
    }
    SEQ_ONE_KEY(KC_KP_5) {
      uprintf("OLI: Go to layer 5\n");
    }
    SEQ_ONE_KEY(KC_KP_6) {
      uprintf("OLI: Go to layer 6\n");
    }
    SEQ_ONE_KEY(KC_KP_7) {
      uprintf("OLI: Go to layer 7\n");
    }
    SEQ_ONE_KEY(KC_KP_8) {
      uprintf("OLI: Go to layer 8\n");
    }
    SEQ_ONE_KEY(KC_KP_9) {
      uprintf("OLI: Go to layer 9\n");
    }

    SEQ_ONE_KEY(KC_KP_DOT) {
      uprintf("OLI: Test sequence\n");
      SEND_STRING("!!! 12 !!!");
    }

    // Go back to base layer
    SEQ_ONE_KEY(KC_LEAD) {
      uprintf("OLI: Return to base layer\n");
      layer_move(OL_LAYER_BASE);
    }

    // Output layer info
    SEQ_TWO_KEYS(KC_LEAD, KC_LEAD) {
      uprintf("Layer: %s\n", LAYER_INFO[currentLayerIndex]);
      SEND_STRING("Layer: ");
      send_string(LAYER_INFO[currentLayerIndex]);
    }
  }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
// #ifdef CONSOLE_ENABLE
//     uprintf("KL: kc: 0x%04X, col: %u, row: %u, pressed: %b, time: %u, interrupt: %b, count: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
// #endif 

  switch (keycode) {
    case OK_CODEBLOCK: 
      if (record->event.pressed) {
        SEND_STRING("```");
      }
    break;
  }
  
  return true;
};


#include QMK_KEYBOARD_H
#include "sendstring_dvorak.h"

// OL - Oli Layer
// I decided to go with numeric names for the layers, so they
// are easily associated with the keys the keypad has.
enum layers {
  OL_LAYER_BASE = 0,
  OL_LAYER_1,
  OL_LAYER_2,
  OL_LAYER_3,
  OL_LAYER_4
};

// OK - Oli Key
enum keys {
  OK_CODEBLOCK = SAFE_RANGE,
  OK_WORKSPACE_1_M,
  OK_WORKSPACE_1_C,
  OK_WORKSPACE_1_W,
  OK_WORKSPACE_2_C,
  OK_WORKSPACE_2_M,
  OK_WORKSPACE_2_T,
  OK_WORKSPACE_3_D,
  OK_WORKSPACE_3_A,
  OK_WORKSPACE_3_E,
};

const char * LAYER_INFO[] = {
  "Base layer",
  "1. Clipboard",
  "2. Line Navigation",
  "3. Text Entry",
  "4. Workspace Navigation",
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
		OK_WORKSPACE_1_W, OK_WORKSPACE_2_T, OK_CODEBLOCK,     \
		OK_WORKSPACE_1_C, OK_WORKSPACE_2_M, OK_WORKSPACE_3_A, \
		OK_WORKSPACE_1_M, OK_WORKSPACE_2_C, OK_WORKSPACE_3_D, \
		KC_LEAD, KC_KP_0, KC_KP_DOT \
	),

  // Clipboard
	[OL_LAYER_1] = LAYOUT(
		C(KC_I), KC_TRNS, C(KC_DOT), \
		KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, C(KC_B), KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS \
	),

  // Line navigation
	[OL_LAYER_2] = LAYOUT(
		KC_TRNS, KC_HOME, KC_PGUP, \
		KC_TRNS, KC_END, KC_PGDN, \
		KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_ENT \
	),


  // Text entry
	[OL_LAYER_3] = LAYOUT(
		KC_TRNS, KC_TRNS, OK_CODEBLOCK, \
		KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS \
	),

  // Workspace navigation
	[OL_LAYER_4] = LAYOUT(
		OK_WORKSPACE_1_W, OK_WORKSPACE_2_T, OK_WORKSPACE_3_E, \
		OK_WORKSPACE_1_C, OK_WORKSPACE_2_M, OK_WORKSPACE_3_A, \
		OK_WORKSPACE_1_M, OK_WORKSPACE_2_C, OK_WORKSPACE_3_D, \
		KC_TRNS,          KC_TRNS,          KC_TRNS           \
	),

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
    // Oli - following two lines here based on examples. There
    // are other examples that call leader_end at the end of this 
    // function, after the SEQ_ calls - not sure if there's a 
    // difference. It appears that leading has the correct
    // value inside process_record_user, so it's all good 
    // for my purposes.
    leading = false;
    leader_end();

    // Switch to numbered layers
    SEQ_ONE_KEY(OK_WORKSPACE_1_M) {
      uprintf("OLI: Go to layer 1\n");
      layer_move(OL_LAYER_1);
    }
    SEQ_ONE_KEY(OK_WORKSPACE_2_C) {
      uprintf("OLI: Go to layer 2\n");
      layer_move(OL_LAYER_2);
    }
    SEQ_ONE_KEY(OK_WORKSPACE_3_D) {
      uprintf("OLI: Go to layer 3\n");
      layer_move(OL_LAYER_3);
    }
    SEQ_ONE_KEY(OK_WORKSPACE_1_C) {
      uprintf("OLI: Go to layer 4\n");
      layer_move(OL_LAYER_4);
    }
    SEQ_ONE_KEY(OK_WORKSPACE_2_M) {
      uprintf("OLI: Go to layer 5\n");
    }
    SEQ_ONE_KEY(OK_WORKSPACE_3_A) {
      uprintf("OLI: Go to layer 6\n");
    }
    SEQ_ONE_KEY(OK_WORKSPACE_1_W) {
      uprintf("OLI: Go to layer 7\n");
    }
    SEQ_ONE_KEY(OK_WORKSPACE_2_T) {
      uprintf("OLI: Go to layer 8\n");
    }
    SEQ_ONE_KEY(OK_CODEBLOCK) {
      uprintf("OLI: Go to layer 9\n");
    }

    SEQ_ONE_KEY(KC_KP_DOT) {
      uprintf("OLI: Test sequence\n");
      SEND_STRING("!!! 15 !!!");
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

  // All of these codes are only evaluated if they are not pressed
  // as part of a leader sequence.
  if (!leading) {
    switch (keycode) {
      case OK_CODEBLOCK: 
        if (record->event.pressed) {
          SEND_STRING("```");
        }
      break;

      case OK_WORKSPACE_1_M: 
        if (record->event.pressed) {
          //uprintf("OLI: WORKSPACE_1_M: leading: %b\n", leading);
          SEND_STRING(SS_LGUI("1") "m");
        }
      break;

      case OK_WORKSPACE_1_C: 
        if (record->event.pressed) {
          SEND_STRING(SS_LGUI("1") "c");
        }
      break;

      case OK_WORKSPACE_1_W: 
        if (record->event.pressed) {
          SEND_STRING(SS_LGUI("1") "w");
        }
      break;

      case OK_WORKSPACE_2_T: 
        if (record->event.pressed) {
          SEND_STRING(SS_LGUI("2") "t");
        }
      break;

      case OK_WORKSPACE_2_M: 
        if (record->event.pressed) {
          SEND_STRING(SS_LGUI("2") "m");
        }
      break;

      case OK_WORKSPACE_2_C: 
        if (record->event.pressed) {
          SEND_STRING(SS_LGUI("2") "c");
        }
      break;

      case OK_WORKSPACE_3_E: 
        if (record->event.pressed) {
          SEND_STRING(SS_LGUI("3") "e");
        }
      break;

      case OK_WORKSPACE_3_A: 
        if (record->event.pressed) {
          SEND_STRING(SS_LGUI("3") "a");
        }
      break;

      case OK_WORKSPACE_3_D: 
        if (record->event.pressed) {
          SEND_STRING(SS_LGUI("3") "d");
        }
      break;
    }
  }

  return true;
};

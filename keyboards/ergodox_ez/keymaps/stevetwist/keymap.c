#include QMK_KEYBOARD_H
#include "version.h"

// Static state tracking variables
static bool escPressed = false;
static bool layerComboPressed = false;
static bool shiftPressed = false;
static bool keylockSeek = false; // Seeking for next keypress after keylock key
static bool keylockShift = false;
static bool keylockCtrl = false;
static bool keylockAlt = false;

static uint8_t highestLayer = 0;

enum layers {
    BASE, // default layer
    MSEL, // mouse layer
    NUML, // num-pad layer
};

enum custom_keycodes {
    ST_TOP0 = SAFE_RANGE,
    ST_TOP1,
    ST_TOP2,
    ST_TOP3,
    ST_TOP4,
    ST_TOP5,
    ST_TOP6,
    ST_TOP7,
    ST_TOP8,
    ST_TOP9,
    ST_TOP10,
    ST_TOP11,
    ST_LOCK,
    ST_DCLK,
    ST_START_NUMPAD, // All num-pad keys after this
    ST_NPDV, // /
    ST_NPTM, // *
    ST_NPMN, // -
    ST_NPPL, // +
    ST_NPDT, // .
    ST_NPEN, // enter
    ST_END_NUMPAD, // All num-pad keys before this
};

/* Tap Dance */
enum {
    SINGLE_TAP = 1,
    SINGLE_HOLD,
    DOUBLE_TAP,
    DOUBLE_HOLD,
    DOUBLE_SINGLE_TAP, // Send two single taps
    TRIPLE_TAP,
    TRIPLE_HOLD
};

// Tap dance enums
enum {
    DN_SC, // tap: colon/semi-colon; hold: ctrl+z (undo)
    DN_Q, // tap: q; hold: ctrl+x (cut)
    DN_J, // tap: j; hold: ctrl+c (copy)
    DN_K, // tap: k; hold: ctrl+v (paste)
    DN_A, // tap: a; hold: ctrl+a
    DN_O, // tap: o; hold: ctrl+s (save)
    DN_E, // tap: e; hold: ctrl+d
    
    DN_MS, // mouse-speed selection (single/double/tripple tap)
    
    DN_SU, // scroll-up
    DN_SD, // scroll-down
    DN_SL, // scroll-left
    DN_SR, // scroll-right
       
    NUM_TAP_DANCE_KEYS
};

static uint8_t tapDanceState[NUM_TAP_DANCE_KEYS];

uint8_t cur_dance(qk_tap_dance_state_t *state);

// Pre-declare main tap dance function
void tapDanceFinished(qk_tap_dance_state_t *state, uint8_t danceCode);
void tapDanceReset(qk_tap_dance_state_t *state, uint8_t danceCode);

void mouseTapDanceFinished(qk_tap_dance_state_t *state, uint8_t danceCode);
void mouseTapDanceReset(qk_tap_dance_state_t *state, uint8_t danceCode);

void scrollTapDanceFinished(qk_tap_dance_state_t *state, uint8_t danceCode);
void scrollTapDanceReset(qk_tap_dance_state_t *state, uint8_t danceCode);

// Define each of the tap-dance functions
void dn_sc_finished(qk_tap_dance_state_t *state, void *user_data) { tapDanceFinished(state, DN_SC); }
void dn_sc_reset(qk_tap_dance_state_t *state, void *user_data) { tapDanceReset(state, DN_SC); }

void dn_q_finished(qk_tap_dance_state_t *state, void *user_data) { tapDanceFinished(state, DN_Q); }
void dn_q_reset(qk_tap_dance_state_t *state, void *user_data) { tapDanceReset(state, DN_Q); }

void dn_j_finished(qk_tap_dance_state_t *state, void *user_data) { tapDanceFinished(state, DN_J); }
void dn_j_reset(qk_tap_dance_state_t *state, void *user_data) { tapDanceReset(state, DN_J); }

void dn_k_finished(qk_tap_dance_state_t *state, void *user_data) { tapDanceFinished(state, DN_K); }
void dn_k_reset(qk_tap_dance_state_t *state, void *user_data) { tapDanceReset(state, DN_K); }

void dn_a_finished(qk_tap_dance_state_t *state, void *user_data) { tapDanceFinished(state, DN_A); }
void dn_a_reset(qk_tap_dance_state_t *state, void *user_data) { tapDanceReset(state, DN_A); }

void dn_o_finished(qk_tap_dance_state_t *state, void *user_data) { tapDanceFinished(state, DN_O); }
void dn_o_reset(qk_tap_dance_state_t *state, void *user_data) { tapDanceReset(state, DN_O); }

void dn_e_finished(qk_tap_dance_state_t *state, void *user_data) { tapDanceFinished(state, DN_E); }
void dn_e_reset(qk_tap_dance_state_t *state, void *user_data) { tapDanceReset(state, DN_E); }

void dn_ms_finished(qk_tap_dance_state_t *state, void *user_data) { mouseTapDanceFinished(state, DN_MS); }
void dn_ms_reset(qk_tap_dance_state_t *state, void *user_data) { mouseTapDanceReset(state, DN_MS); }

void dn_su_finished(qk_tap_dance_state_t *state, void *user_data) { scrollTapDanceFinished(state, DN_SU); }
void dn_su_reset(qk_tap_dance_state_t *state, void *user_data) { scrollTapDanceReset(state, DN_SU); }

void dn_sd_finished(qk_tap_dance_state_t *state, void *user_data) { scrollTapDanceFinished(state, DN_SD); }
void dn_sd_reset(qk_tap_dance_state_t *state, void *user_data) { scrollTapDanceReset(state, DN_SD); }

void dn_sl_finished(qk_tap_dance_state_t *state, void *user_data) { scrollTapDanceFinished(state, DN_SL); }
void dn_sl_reset(qk_tap_dance_state_t *state, void *user_data) { scrollTapDanceReset(state, DN_SL); }

void dn_sr_finished(qk_tap_dance_state_t *state, void *user_data) { scrollTapDanceFinished(state, DN_SR); }
void dn_sr_reset(qk_tap_dance_state_t *state, void *user_data) { scrollTapDanceReset(state, DN_SR); }


qk_tap_dance_action_t tap_dance_actions[] = {
    [DN_SC] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dn_sc_finished, dn_sc_reset, 300),
    [DN_Q] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dn_q_finished, dn_q_reset, 300),
    [DN_J] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dn_j_finished, dn_j_reset, 300),
    [DN_K] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dn_k_finished, dn_k_reset, 300),
    [DN_A] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dn_a_finished, dn_a_reset, 300),
    [DN_O] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dn_o_finished, dn_o_reset, 300),
    [DN_E] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dn_e_finished, dn_e_reset, 300),
    [DN_MS] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dn_ms_finished, dn_ms_reset, 300),
    [DN_SU] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dn_su_finished, dn_su_reset, 300),
    [DN_SD] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dn_sd_finished, dn_sd_reset, 300),   
    [DN_SL] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dn_sl_finished, dn_sl_reset, 300),
    [DN_SR] = ACTION_TAP_DANCE_FN_ADVANCED_TIME(NULL, dn_sr_finished, dn_sr_reset, 300),  
};


/* Key layouts */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[BASE] = LAYOUT_ergodox_pretty(
  // left hand
  KC_ESC,  ST_TOP0,  ST_TOP1,  ST_TOP2,  ST_TOP3, ST_TOP4, ST_TOP5,              ST_TOP6,  ST_TOP7, ST_TOP8, ST_TOP9,  ST_TOP10,  ST_TOP11,  KC_PSCREEN,
  ST_LOCK, KC_QUOT,  KC_COMMA, KC_DOT,   KC_P,    KC_Y,    KC_HOME,              KC_END,   KC_F,    KC_G,    KC_C,     KC_R,      KC_L,      KC_PGUP,
  KC_NO,   TD(DN_A), TD(DN_O), TD(DN_E), KC_U,    KC_I,                                    KC_D,    KC_H,    KC_T,     KC_N,      KC_S,      KC_PGDN,
  KC_LALT, TD(DN_SC),TD(DN_Q), TD(DN_J), TD(DN_K),KC_X,    KC_DEL,               KC_BSPC,  KC_B,    KC_M,    KC_W,     KC_V,      KC_Z,      KC_LCTL,
  KC_NO,   TD(DN_SL),TD(DN_SU),TD(DN_SD),TD(DN_SR),                                                 KC_LEFT, KC_UP,    KC_DOWN,   KC_RGHT,   KC_LCTL,
                                                  MO(NUML),KC_NO,                KC_NO,    KC_NO,
                                                           KC_NO,                KC_NO,
                                         KC_LSFT, KC_TAB,  KC_LWIN,              KC_NO,    KC_ENT,  KC_SPC
),

[MSEL] = LAYOUT_ergodox_pretty(
  // left hand
  TO(BASE), KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,        KC_NO,    KC_NO,    KC_NO,   KC_NO,    KC_NO,   KC_NO,   KC_NO,  
  ST_LOCK,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,        KC_NO,    KC_NO,    KC_NO,   KC_MS_U,  KC_NO,   KC_NO,   KC_NO,  
  TD(DN_SU),KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                           TD(DN_SU),KC_MS_L, KC_NO,    KC_MS_R, KC_NO,   KC_NO,  
  TD(DN_SD),KC_BTN4, KC_BTN2, KC_BTN3, KC_BTN1, ST_DCLK, KC_NO,       KC_NO,    TD(DN_SD),KC_NO,   KC_MS_D,  KC_NO,   KC_NO,   KC_NO,  
  KC_NO,    KC_LCTL, KC_LALT, C(KC_C), C(KC_V),                                           KC_ACL0, KC_ACL1,  KC_ACL2, KC_NO,   KC_NO,  
 
                                                KC_NO,    KC_NO,         KC_NO,   KC_NO,  
                                                          KC_NO,         KC_NO,  
                                       KC_LSFT, KC_SPC,   KC_NO,         KC_NO,   KC_NO,   KC_NO
),

[NUML] = LAYOUT_ergodox_pretty(
  // left hand
  TO(BASE), KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,       KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_CALC,
  KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS, KC_NO,   KC_7,    KC_8,    KC_9,    ST_NPMN, ST_NPDV,
  KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                       KC_NO,   KC_4,    KC_5,    KC_6,    ST_NPPL, ST_NPTM,
  KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS, KC_NO,   KC_1,    KC_2,    KC_3,    KC_NO,   ST_NPEN,
  KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                                         KC_0,    ST_NPDT, KC_NO,   KC_NO,   KC_NO,

                                                KC_TRNS, KC_TRNS,     KC_TRNS, KC_TRNS,
                                                         KC_TRNS,     KC_TRNS,
                                       KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS, KC_TRNS, KC_TRNS
),

};


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keylockSeek && record->event.pressed) {
        switch (keycode) {
            case KC_LSFT:
                keylockShift = true;
                shiftPressed = true;
                keylockSeek = false;
                ergodox_right_led_1_on();                
                return true; // Issue shift key on, like normal
                break;
            
            case KC_LALT:
                keylockAlt = true;
                keylockSeek = false;
                ergodox_right_led_3_on();
                return true; // Issue alt key on, like normal
                break;
                
            case KC_LCTL:
                keylockCtrl = true;
                keylockSeek = false;
                ergodox_right_led_2_on();
                return true; // Issue Ctrl key on, like normal
                break;
                
            default:
                // Any other key-press, cancel the keylock seek
                keylockSeek = false;
        }
    }
    
    
    switch(keycode) {
        case ST_LOCK:
            if (record->event.pressed) {
                keylockSeek = true;
            }
            return false;
            break;
        
        case KC_ESC:
            if (record->event.pressed){
                escPressed = true;          
                layerComboPressed = false;
                return false; // Consume the keypress - we handle it depending on if we layer combo or not, upon release
            } else {
                escPressed = false;
                if (!layerComboPressed) {
                    // No combo issued whilst esc held, issue an escape keypress
                    SEND_STRING(SS_TAP(X_ESC));
                }                
                layerComboPressed = false;               
                return false;
            }
            break;  

        case ST_TOP0:
            if (escPressed && record->event.pressed){
                // Combo key!
                layerComboPressed = true; // Disable sending of escape key                
                layer_move(MSEL);
                return false;
                
            } 
            break;

        case ST_TOP1:
            if (escPressed && record->event.pressed){
                // Combo key!
                layerComboPressed = true; // Disable sending of escape key
                layer_move(NUML);
                return false;                
            } 
            break;
        
        
        
        
        case KC_LSFT:
            if (record->event.pressed){
                shiftPressed = true;
                keylockShift = false;
                ergodox_right_led_1_off();
            } else {
                if (keylockShift) {
                    // Ignore the release of the shift key
                    return false;
                }
                
                shiftPressed = false;
            }
            break;            
            
        case KC_LALT:
            if (record->event.pressed) {
                keylockAlt = false;
                ergodox_right_led_3_off();
            } else {
                if (keylockAlt) {
                    // Ignore the release of the alt key
                    return false;
                }
            }
            break;
            
        case KC_LCTL:
            if (record->event.pressed) {
                keylockCtrl = false;
                ergodox_right_led_2_off();
            } else {
                if (keylockCtrl) {
                    // Ignore the release of the ctrl key
                    return false;
                }
            }
            break;
            
        case ST_DCLK:
            if (record->event.pressed) {
                SEND_STRING(SS_TAP(X_BTN1) SS_DELAY(10) SS_TAP(X_BTN1));
            }
    }
    
    
  if (record->event.pressed) {
    switch (keycode) {
        case ST_TOP0:
            if (shiftPressed) {
                SEND_STRING("?");
                // Sending a capitalized symbol results in releasing shift, we must press it again
                SEND_STRING(SS_DOWN(X_LSFT));
            } else {
                SEND_STRING("!");                
            }
            return false; // Do not emit actual key press
            
        case ST_TOP1:
            if (shiftPressed) {
                SEND_STRING("@");
                // Sending a capitalized symbol results in releasing shift, we must press it again
                SEND_STRING(SS_DOWN(X_LSFT));
            } else {
                SEND_STRING("(");
            }
            return false; // Do not emit actual key press
            
        case ST_TOP2:
            if (shiftPressed) {
                SEND_STRING("#");
                // Sending a capitalized symbol results in releasing shift, we must press it again
                SEND_STRING(SS_DOWN(X_LSFT));
            } else {
                SEND_STRING("[");
            }
            return false; // Do not emit actual key press
            
        case ST_TOP3:
            if (shiftPressed) {
                SEND_STRING("$");
                // Sending a capitalized symbol results in releasing shift, we must press it again
                SEND_STRING(SS_DOWN(X_LSFT));
            } else {
                SEND_STRING("{");
            }
            return false; // Do not emit actual key press
            
        case ST_TOP4:
            if (shiftPressed) {
                SEND_STRING("%");
                // Sending a capitalized symbol results in releasing shift, we must press it again
                SEND_STRING(SS_DOWN(X_LSFT));
            } else {
                SEND_STRING("_");
            }
            return false; // Do not emit actual key press
        
        case ST_TOP5:
            if (shiftPressed) {
                SEND_STRING("~");
                // Sending a capitalized symbol results in releasing shift, we must press it again
                SEND_STRING(SS_DOWN(X_LSFT));
            } else {
                SEND_STRING("`");
            }
            return false; // Do not emit actual key press
            
        case ST_TOP6:
            if (shiftPressed) {
                SEND_STRING("|");
                // Sending a capitalized symbol results in releasing shift, we must press it again
                SEND_STRING(SS_DOWN(X_LSFT));
            } else {
                SEND_STRING("^");
            }
            return false; // Do not emit actual key press
            
        case ST_TOP7:
            if (shiftPressed) {
                SEND_STRING("&");
                // Sending a capitalized symbol results in releasing shift, we must press it again
                SEND_STRING(SS_DOWN(X_LSFT));
            } else {
                SEND_STRING("*");
            }
            return false; // Do not emit actual key press
            
        case ST_TOP8:
            if (shiftPressed) {
                // We must release shift to send this key
                SEND_STRING(SS_UP(X_LSFT));
                SEND_STRING("-");
                SEND_STRING(SS_DOWN(X_LSFT));
            } else {
            SEND_STRING("}");
            }
            return false; // Do not emit actual key press
            
        case ST_TOP9:
            if (shiftPressed) {
                SEND_STRING("+");
                // Sending a capitalized symbol results in releasing shift, we must press it again
                SEND_STRING(SS_DOWN(X_LSFT));
            } else {
                SEND_STRING("]");
            }
            return false; // Do not emit actual key press
            
        case ST_TOP10:
            if (shiftPressed) {
                // We must release shift to send this key
                SEND_STRING(SS_UP(X_LSFT));
                SEND_STRING("=");
                SEND_STRING(SS_DOWN(X_LSFT));
            } else {
                SEND_STRING(")");
            }
            return false; // Do not emit actual key press
            
        case ST_TOP11:
            if (shiftPressed) {
                // We must release shift to send this key
                SEND_STRING(SS_UP(X_LSFT));
                SEND_STRING("\\");
                SEND_STRING(SS_DOWN(X_LSFT));
            } else {
                SEND_STRING("/");
            }
            return false; // Do not emit actual key press
    }
    
    if (keycode > ST_START_NUMPAD && keycode < ST_END_NUMPAD) {
        if (shiftPressed) {
            SEND_STRING(SS_UP(X_LSFT));
        }
        
        switch (keycode) {
            case ST_NPDV:
                SEND_STRING("/");
                break;
                
            case ST_NPTM:
                SEND_STRING("*");
                break;
                
            case ST_NPMN:
                SEND_STRING("-");
                break;
                
            case ST_NPPL:
                SEND_STRING("+");
                break;
                
            case ST_NPDT:
                SEND_STRING(".");
                break;
                
            case ST_NPEN:
                SEND_STRING(SS_TAP(X_ENT));
                break;
        }
        
        if (shiftPressed) {
            SEND_STRING(SS_DOWN(X_LSFT));
        }
        
        return false; // Do not emit actual key press
        
    }
    
    
  } 
  return true;
}


void clear_led_colors(void) {
    for (int i = 0; i < DRIVER_LED_TOTAL; i++) {
        rgb_matrix_set_color(i, 0, 0, 0);
    }
}

// LED RGB colors for each layer
void set_led_rgb_layer0(void) {   
}


void set_led_rgb_layer1(void) {
    // Mouse layer
    
    // Mouse movement
    rgb_matrix_set_color(7, 0x00, 0xFF, 0xFF); // cyan
    rgb_matrix_set_color(11, 0x00, 0xFF, 0xFF);
    rgb_matrix_set_color(13, 0x00, 0xFF, 0xFF);
    rgb_matrix_set_color(17, 0x00, 0xFF, 0xFF);
    
    // Mouse speed
    rgb_matrix_set_color(20, 0x00, 0x00, 0xFF); // blue
    rgb_matrix_set_color(21, 0x00, 0x00, 0xFF); 
    rgb_matrix_set_color(22, 0x00, 0x00, 0xFF); 
    
    // WASD
    rgb_matrix_set_color(32, 0xFF, 0x8D, 0x00); // orange
    rgb_matrix_set_color(36, 0xFF, 0x8D, 0x00);
    rgb_matrix_set_color(37, 0xFF, 0x8D, 0x00);
    rgb_matrix_set_color(38, 0xFF, 0x8D, 0x00);
    
    // Mouse buttons
    rgb_matrix_set_color(39, 0x00, 0xFF, 0xFF); // cyan
    rgb_matrix_set_color(40, 0x00, 0xFF, 0xFF);
    rgb_matrix_set_color(41, 0x00, 0xFF, 0xFF);
    rgb_matrix_set_color(42, 0x00, 0xFF, 0xFF);
    
    // Copy, paste
    rgb_matrix_set_color(44, 0xFF, 0x8D, 0x00); // orange
    rgb_matrix_set_color(45, 0xFF, 0x8D, 0x00);    
    
    // Ctrl, alt
    rgb_matrix_set_color(46, 0x00, 0x00, 0xFF); // blue
    rgb_matrix_set_color(47, 0x00, 0x00, 0xFF);    
}


void set_led_rgb_layer2(void) {
    // F-keys
    rgb_matrix_set_color(0, 0x00, 0xFF, 0x00); // green
    rgb_matrix_set_color(1, 0x00, 0xFF, 0x00);
    rgb_matrix_set_color(2, 0x00, 0xFF, 0x00);
    rgb_matrix_set_color(3, 0x00, 0xFF, 0x00);
    rgb_matrix_set_color(4, 0x00, 0xFF, 0x00);
    
    rgb_matrix_set_color(24, 0x00, 0xFF, 0x00);
    rgb_matrix_set_color(25, 0x00, 0xFF, 0x00);
    rgb_matrix_set_color(26, 0x00, 0xFF, 0x00);
    rgb_matrix_set_color(27, 0x00, 0xFF, 0x00);
    rgb_matrix_set_color(28, 0x00, 0xFF, 0x00);
    
    // Num-pad keys
    rgb_matrix_set_color(6, 0xFF, 0x8D, 0x00); // orange
    rgb_matrix_set_color(7, 0xFF, 0x8D, 0x00);
    rgb_matrix_set_color(8, 0xFF, 0x8D, 0x00);
    rgb_matrix_set_color(11, 0xFF, 0x8D, 0x00);
    rgb_matrix_set_color(12, 0xFF, 0x8D, 0x00);
    rgb_matrix_set_color(13, 0xFF, 0x8D, 0x00);
    rgb_matrix_set_color(16, 0xFF, 0x8D, 0x00);
    rgb_matrix_set_color(17, 0xFF, 0x8D, 0x00);
    rgb_matrix_set_color(18, 0xFF, 0x8D, 0x00);
    rgb_matrix_set_color(20, 0xFF, 0x8D, 0x00);
    
    // Num-pad minus/plus/dot
    rgb_matrix_set_color(9, 0x00, 0x00, 0xFF); // blue
    rgb_matrix_set_color(14, 0x00, 0x00, 0xFF);
    rgb_matrix_set_color(21, 0x00, 0x00, 0xFF);
}


void rgb_matrix_indicators_user(void) {
    clear_led_colors();
    switch (highestLayer) {
        case 0:
            set_led_rgb_layer0();
            break;
        case 1:
            set_led_rgb_layer1();
            break;
        case 2:
            set_led_rgb_layer2();
            break;            
    }    
}


// Runs just one time when the keyboard initializes.
void keyboard_post_init_user(void) {
    rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
    set_led_rgb_layer0();
};


// Runs whenever there is a layer state change.
layer_state_t layer_state_set_user(layer_state_t state) {
  highestLayer = get_highest_layer(state);
  
  return state;
};


// Tap-dance: main functions
/* Return an integer that corresponds to what kind of tap dance should be executed.
 *
 * How to figure out tap dance state: interrupted and pressed.
 *
 * Interrupted: If the state of a dance dance is "interrupted", that means that another key has been hit
 *  under the tapping term. This is typically indicitive that you are trying to "tap" the key.
 *
 * Pressed: Whether or not the key is still being pressed. If this value is true, that means the tapping term
 *  has ended, but the key is still being pressed down. This generally means the key is being "held".
 *
 * One thing that is currenlty not possible with qmk software in regards to tap dance is to mimic the "permissive hold"
 *  feature. In general, advanced tap dances do not work well if they are used with commonly typed letters.
 *  For example "A". Tap dances are best used on non-letter keys that are not hit while typing letters.
 *
 * Good places to put an advanced tap dance:
 *  z,q,x,j,k,v,b, any function key, home/end, comma, semi-colon
 *
 * Criteria for "good placement" of a tap dance key:
 *  Not a key that is hit frequently in a sentence
 *  Not a key that is used frequently to double tap, for example 'tab' is often double tapped in a terminal, or
 *    in a web form. So 'tab' would be a poor choice for a tap dance.
 *  Letters used in common words as a double. For example 'p' in 'pepper'. If a tap dance function existed on the
 *    letter 'p', the word 'pepper' would be quite frustating to type.
 *
 * For the third point, there does exist the 'DOUBLE_SINGLE_TAP', however this is not fully tested
 *
 */
uint8_t cur_dance(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return SINGLE_TAP;
        // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
        else return SINGLE_HOLD;
    } else if (state->count == 2) {
        // DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
        // action when hitting 'pp'. Suggested use case for this return value is when you want to send two
        // keystrokes of the key, and not the 'double tap' action/macro.
        if (state->interrupted) return DOUBLE_SINGLE_TAP;
        else if (state->pressed) return DOUBLE_HOLD;
        else return DOUBLE_TAP;
    }

    // Assumes no one is trying to type the same letter three times (at least not quickly).
    // If your tap dance key is 'KC_W', and you want to type "www." quickly - then you will need to add
    // an exception here to return a 'TRIPLE_SINGLE_TAP', and define that enum just like 'DOUBLE_SINGLE_TAP'
    if (state->count == 3) {
        if (state->interrupted || !state->pressed) return TRIPLE_TAP;
        else return TRIPLE_HOLD;
    } else return 8; // Magic number. At some point this method will expand to work for more presses
}


uint8_t normalTapDanceKeycode(uint8_t danceCode) {
    switch (danceCode) {
        case DN_SC:
            return KC_SCLN;
        
        case DN_Q:
            return KC_Q;
        
        case DN_J:
            return KC_J;
            
        case DN_K:
            return KC_K;
            
        case DN_A:
            return KC_A;
            
        case DN_O:
            return KC_O;
            
        case DN_E:
            return KC_E;
            
        default:
            return KC_ESC;
    }
}

uint8_t holdTapDanceKeycode(uint8_t danceCode) {
    switch (danceCode) {
        case DN_SC:
            return KC_Z;
        
        case DN_Q:
            return KC_X;
        
        case DN_J:
            return KC_C;
            
        case DN_K:
            return KC_V;
            
        case DN_A:
            return KC_A;
            
        case DN_O:
            return KC_S;
            
        case DN_E:
            return KC_D;
            
        default:
            return KC_ESC;
    }
}

uint8_t scrollTapDanceKeycode(uint8_t danceCode) {
    switch (danceCode) {           
        case DN_SU:
            return KC_WH_U;
            
        case DN_SD:
            return KC_WH_D;
            
        case DN_SL:
            return KC_WH_L;
            
        case DN_SR:
            return KC_WH_R;
            
        default:
            return KC_ESC;
    }
}

void tapDanceFinished(qk_tap_dance_state_t *state, uint8_t danceCode) {
    tapDanceState[danceCode] = cur_dance(state);    
    uint8_t normalKeycode = normalTapDanceKeycode(danceCode);
    uint8_t holdKeycode = holdTapDanceKeycode(danceCode);
    
    switch (tapDanceState[danceCode]) {        
        case TRIPLE_TAP:
        case TRIPLE_HOLD:
            tap_code(normalKeycode);
            // Fall-through:
        case DOUBLE_TAP:
        case DOUBLE_SINGLE_TAP:
        case DOUBLE_HOLD:
            tap_code(normalKeycode);
            // Fall-through:        
        case SINGLE_TAP: 
            register_code(normalKeycode);
            break;
            
        case SINGLE_HOLD:     
            register_code(KC_LCTL);
            tap_code(holdKeycode);
            unregister_code(KC_LCTL);
            break;               
    }    
}


void tapDanceReset(qk_tap_dance_state_t *state, uint8_t danceCode) {
    if (tapDanceState[danceCode] != SINGLE_HOLD) {
        uint8_t normalKeycode = normalTapDanceKeycode(danceCode);
        unregister_code(normalKeycode);
    }
}

/*
  NOTE: Currently unused. Using 3 dedicated speed keys, instead.
 */
void mouseTapDanceFinished(qk_tap_dance_state_t *state, uint8_t danceCode) {
    tapDanceState[danceCode] = cur_dance(state);
    switch (tapDanceState[danceCode]) {        
        case TRIPLE_TAP:
        case TRIPLE_HOLD:
            tap_code(KC_ACL2); // Fast-speed
            break;
            
        case DOUBLE_TAP:
        case DOUBLE_SINGLE_TAP:
        case DOUBLE_HOLD:
            tap_code(KC_ACL1); // Medium-speed
            break;
            
        case SINGLE_TAP: 
        case SINGLE_HOLD:     
            tap_code(KC_ACL0); // Slow-speed            
            break;            
    }    
}

void mouseTapDanceReset(qk_tap_dance_state_t *state, uint8_t danceCode) {
}

void scrollTapDanceFinished(qk_tap_dance_state_t *state, uint8_t danceCode) {
    tapDanceState[danceCode] = cur_dance(state);   
    uint8_t scrollKeycode = scrollTapDanceKeycode(danceCode);
    
     switch (tapDanceState[danceCode]) {        
        case TRIPLE_TAP:
            tap_code(KC_ACL0); // Slow speed
            tap_code(scrollKeycode);
            tap_code(scrollKeycode);
            tap_code(scrollKeycode);
            tap_code(KC_ACL1); // Reset to medium speed (default)
            break;
            
        case DOUBLE_TAP:
        case DOUBLE_SINGLE_TAP:
            tap_code(KC_ACL0); // Slow speed
            tap_code(scrollKeycode);            
            tap_code(scrollKeycode);
            tap_code(KC_ACL1); // Reset to medium speed (default)
            break;
        
        case SINGLE_TAP: 
            tap_code(KC_ACL0); // Slow speed            
            tap_code(scrollKeycode);
            tap_code(KC_ACL1); // Reset to medium speed (default)
            break;
        
        case SINGLE_HOLD:   
            tap_code(KC_ACL1); // Medium speed
            register_code(scrollKeycode);
            break;           
        
        case DOUBLE_HOLD:
        case TRIPLE_HOLD:
            tap_code(KC_ACL2); // Fast speed (reset to medium on "tap dance reset")
            register_code(scrollKeycode);
            break;
    }    
}

void scrollTapDanceReset(qk_tap_dance_state_t *state, uint8_t danceCode) {
    uint8_t scrollKeycode = scrollTapDanceKeycode(danceCode);
    unregister_code(scrollKeycode);
    
    tap_code(KC_ACL1); // Set speed to medium (default)
}

#pragma once

/* disable action features */
#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION

/* Mouse Keys */

#define MOUSEKEY_DELAY 300 // Delay between pressing a movement key and cursor movement
#define MOUSEKEY_INTERVAL 16 // Time between cursor movements (60hz = 16ms)

#define MOUSEKEY_MAX_SPEED 10 // Max cursor speed @ which acceleration stops
#define MOUSEKEY_TIME_TO_MAX 60 // Time until maximum cursor speed is reached

#define MOUSEKEY_WHEEL_DELAY 300 // Delay between pressing a wheel key and wheel movement
#define MOUSEKEY_WHEEL_INTERVAL 100 // Time between wheel movements

#define MOUSEKEY_WHEEL_MAX_SPEED 8 // Max number of scroll steps per scroll action
#define MOUSEKEY_WHEEL_TIME_TO_MAX 10 // Time until max scroll speed is reached


#define MK_3_SPEED // Constant cursor speeds

#define MK_C_OFFSET_0 1 // Cursor offset per movement (KC_ACL0)
#define MK_C_INTERVAL_0 32 // Time between cursor movements (KC_ACL0)
#define MK_C_OFFSET_1 6 // Cursor offset per movement (KC_ACL1)
#define MK_C_INTERVAL_1 16 // Time between cursor movements (KC_ACL1)
#define MK_C_OFFSET_2 48 // Cursor offset per movement (KC_ACL2)
#define MK_C_INTERVAL_2 16 // Time between cursor movements (KC_ACL2)

#define MK_W_OFFSET_0 1 // Scroll steps per scroll action (KC_ACL0)
#define MK_W_INTERVAL_0 360 // Time between scroll steps (KC_ACL0)
#define MK_W_OFFSET_1 1 // Scroll steps per scroll action (KC_ACL1)
#define MK_W_INTERVAL_1 20 // Time between scroll steps (KC_ACL1)
#define MK_W_OFFSET_2 4 // Scroll steps per scroll action (KC_ACL2)
#define MK_W_INTERVAL_2 20 // Time between scroll steps (KC_ACL2)
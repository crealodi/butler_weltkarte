#ifndef __PUZZLE_STATE_MACHINE__
#define __PUZZLE_STATE_MACHINE__

/******************************************************************************************
 * Includes
 ******************************************************************************************/
#include <Arduino.h>

/******************************************************************************************
 * Defines
 ******************************************************************************************/
typedef void(*funcPtr)();			// Function pointer definitions for state machines
typedef funcPtr(*StateFunc)();


#define RELAY_ON             (HIGH)
#define RELAY_OFF            (LOW)

#define RELAY_ON_MS   (400)
#define RELAY_OFF_MS  (400)

#define RELAY_TIMEOUT_MS (5000)

#define WAIT_TIME_IN_PUZZLE_SOLVED_MS (10000)

#define DEBOUNCE_MS (20)

#define BEEP_LONG_MS (600)
#define BEEP_SHORT_MS (100)

#define PIN_IN_SCHLOSS  (48)
#define PIN_IN_TUER_AUF_TASTE (49)

/*
#define PIN_IN_1  (22)
#define PIN_IN_2  (23)
#define PIN_IN_3  (24)
#define PIN_IN_4  (25)

#define PIN_IN_5  (26)
#define PIN_IN_6  (27)
#define PIN_IN_7  (28)
#define PIN_IN_8  (29)

#define PIN_IN_9  (30)
#define PIN_IN_10  (31)
#define PIN_IN_11  (32)
#define PIN_IN_12  (33)

#define PIN_IN_13  (34)
#define PIN_IN_14  (35)
#define PIN_IN_15  (36)
#define PIN_IN_16  (37)

#define PIN_IN_17  (38)
#define PIN_IN_18  (39)
#define PIN_IN_19  (40)
#define PIN_IN_20  (41)

#define PIN_IN_21  (42)
#define PIN_IN_22  (43)
#define PIN_IN_23  (44)
#define PIN_IN_24  (45)
*/ //NOT USED SINCE ALL IS DONE IN LOOPS i = 22 to i<46

#define PIN_OUT_RELAY (46)
#define PIN_OUT_BEEP  (47)

const uint8_t c_correctInputs[] = {22,24,32,33,38,40,42}; 

/******************************************************************************************
 * Declarations
 ******************************************************************************************/
StateFunc puzzle_init();

StateFunc puzzle_gameMode();

StateFunc puzzle_openRelay();

StateFunc puzzle_solved();

uint8_t getInputState();


#endif

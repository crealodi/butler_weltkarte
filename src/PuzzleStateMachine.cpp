#include "PuzzleStateMachine.h"

/******************************************************************************************
 * Declaration 
 ******************************************************************************************/




/******************************************************************************************
 * Implementation
 ******************************************************************************************/
StateFunc puzzle_init()
{


  
  Serial.begin(115200);

  pinMode(PIN_OUT_BEEP, OUTPUT);
  pinMode(PIN_OUT_RELAY, OUTPUT);

  digitalWrite(PIN_OUT_BEEP, LOW);
  digitalWrite(PIN_OUT_RELAY, LOW);

  pinMode(PIN_IN_SCHLOSS, INPUT);
  pinMode(PIN_IN_TUER_AUF_TASTE, INPUT);

  for (int i = 22; i<46; i++){
    Serial.println(i);
  }

	return (StateFunc)puzzle_gameMode;
}

StateFunc puzzle_gameMode()
{
  static unsigned long lastMillis = millis();

  uint8_t inputState = getInputState();

  


  if (lastMillis + 1000 < millis()){
    lastMillis = millis();

  }
  
  return (StateFunc)puzzle_gameMode;
}


StateFunc puzzle_openRelay()
{
  
  static bool firstRun = true;
  static unsigned long lastMillis = millis();
  static unsigned long lastOpenImpulseMillis = millis();

  if (firstRun){
    Serial.println("open State");
    lastMillis = millis();
    firstRun = false;
  }

  uint8_t inputState = getInputState();

  if (!(inputState & 0b1000)){
    //Schloss ist auf
    firstRun = true;

    digitalWrite(PIN_OUT_RELAY, LOW);
    return (StateFunc)puzzle_solved;
  }

  if (lastMillis + RELAY_TIMEOUT_MS < millis()){
    firstRun = true;
    digitalWrite(PIN_OUT_RELAY, LOW);
    return (StateFunc)puzzle_solved;
  }

  if (lastOpenImpulseMillis + RELAY_ON_MS + RELAY_OFF_MS < millis()){
    lastOpenImpulseMillis = millis();
    digitalWrite(PIN_OUT_RELAY, HIGH);
  }

  if (lastOpenImpulseMillis + RELAY_ON_MS < millis()){
    digitalWrite(PIN_OUT_RELAY, LOW);
  }

  
  return (StateFunc)puzzle_openRelay;
}

StateFunc puzzle_solved()
{
  Serial.println("Solved State");
  //Do something specific or just wait a bit before returning to gamemode.
  delay(WAIT_TIME_IN_PUZZLE_SOLVED_MS);
  return (StateFunc)puzzle_init;
}

uint8_t getInputState(){
  //returns state of all inputs binary coded for mechanical switches a debounce is done
  //must be called at least twice for debounce, when all countryinputs are correct it returns 1 on bit 0
  //   128    64     32     16     8       4      2      1
  //    7      6      5      4      3      2      1      0
  //   NU     NU      NU    SWITCH  LOCK  NU     NU  ALLCORRECT
  // SWITCH IS WITH PULLUP - INVERT IT for logical function (high when pressed)
  static unsigned long lockStateDebounceMillis = millis();
  static unsigned long openDoorSwitchDebounceMillis = millis();
  static bool lastStateLock = false;
  static bool lastStateSwitch = false;

  uint8_t inputState = 0;

  


  if (digitalRead(PIN_IN_SCHLOSS) != lastStateLock)
  {
    lastStateLock = digitalRead(PIN_IN_SCHLOSS);
    lockStateDebounceMillis = millis();
  }  
    
  if(lockStateDebounceMillis + DEBOUNCE_MS < millis()){
      inputState |= (lastStateLock << 3);
  }

  if (digitalRead(PIN_IN_TUER_AUF_TASTE) != lastStateSwitch)
  {
    lastStateSwitch = digitalRead(PIN_IN_TUER_AUF_TASTE);
    openDoorSwitchDebounceMillis = millis();
  }  
    
  if (openDoorSwitchDebounceMillis + DEBOUNCE_MS < millis()){
      inputState |= (!lastStateSwitch << 4);
  }

return inputState;
}


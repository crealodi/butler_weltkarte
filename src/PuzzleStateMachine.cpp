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

    pinMode(i, INPUT);
    Serial.println(i);
  }

	return (StateFunc)puzzle_gameMode;
}

StateFunc puzzle_gameMode()
{
  static unsigned long lastMillis = millis();
  uint8_t inputState = getInputState();
  


  if (inputState & 0b1 || inputState & 0b10000){ //alle inputs Richtig oder switch gedrückt --> Türe auf!
    return (StateFunc)puzzle_openRelay;
  }
  
  if (lastMillis + 1000 < millis()){
    lastMillis = millis();
    //Serial.println(getInputState() & 0b1);

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
  // SWITCH IS OVER OPTO - Don't invert it for logical function (high when pressed)
  static unsigned long lockStateDebounceMillis = millis();
  static unsigned long openDoorSwitchDebounceMillis = millis();
  static bool lastStateLock = false;
  static bool lastStateSwitch = false;

  uint8_t inputState = 0;

  if (digitalRead(PIN_IN_SCHLOSS) != lastStateLock) //debounce routine 
  {
    lastStateLock = digitalRead(PIN_IN_SCHLOSS);
    lockStateDebounceMillis = millis();
  }  
    
  if(lockStateDebounceMillis + DEBOUNCE_MS < millis()){
      inputState |= (lastStateLock << 3);
  }

  if (digitalRead(PIN_IN_TUER_AUF_TASTE) != lastStateSwitch) //debounce routine
  {
    lastStateSwitch = digitalRead(PIN_IN_TUER_AUF_TASTE);
    openDoorSwitchDebounceMillis = millis();
  }  
    
  if (openDoorSwitchDebounceMillis + DEBOUNCE_MS < millis()){
      inputState |= (lastStateSwitch << 4); //for switches with inverted logic just do (!lastStateSwitch << 4); instead
  }


  bool alleInputsRichtig = true; //zu Beginn geht man davon dass alles stimmt.
  //invertierte Logik
  
  for (int i = 22; i<46; i++){
    int dieserInputMuss1Sein = false;
    for (int j=0; j<sizeof(c_correctInputs); j++){
      if (c_correctInputs[j]==i){
        Serial.print("Dieser Input muss 1 sein: ");
        Serial.println(i);
        dieserInputMuss1Sein = true;
      }
    }
    uint8_t aktuellerInput = !digitalRead(i); //switches sind active-low
    if (aktuellerInput){
    Serial.print("PIN: ");
    Serial.print(i);
    Serial.print(" State: ");
    Serial.println(aktuellerInput);
    }
    if (alleInputsRichtig){ // nur wenn alle bisher richtig waren muss das noch weiter angeschaut werden.
      if (dieserInputMuss1Sein){ //input muss 1 sein
        if (!aktuellerInput){ ///ist aber 0
          alleInputsRichtig = false;
        }
      }
      else{//input muss 0 sein
        if (aktuellerInput){ //...ist aber 1
          alleInputsRichtig = false;
        }
      } 

    }  
  }

  if (alleInputsRichtig){
    inputState |= 0b1;
  }
  else {
    inputState &= 0x11111110;
  }



return inputState;
}


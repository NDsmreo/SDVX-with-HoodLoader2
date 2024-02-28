
#define MAX_BUTTONS 7


#include <Encoder.h>

int const BTA = 0;
int const BTB = 12;
int const BTC = 6;
int const BTD = 11;
int const FXL = 5;
int const FXR = 7;
int const START = 10;


int const BTLA = A5;
int const BTLB = A4;
int const BTLC = A2;
int const BTLD = A0;
int const FXLL = 8;
int const FXLR = 13;
int const STARTL = A3;


int const VOLLA = 2;
int const VOLLB = 4;
int const VOLRA = 3;
int const VOLRB = 9;


uint8_t keyBTA = 's'; //s
uint8_t keyBTB = 'd'; //d
uint8_t keyBTC = 'k'; //k
uint8_t keyBTD = 'l'; //l

uint8_t keyFXL = 'c'; //c
uint8_t keyFXR = 'm'; //m


uint8_t keySTART = 'e'; //Enter

uint8_t keyVOLLCCW = 'q'; //q
uint8_t keyVOLLCW = 'w'; //w
uint8_t keyVOLRCCW = 'o'; //o
uint8_t keyVOLRCW = 'p'; //p


unsigned const long debounceDelay = 5; 

unsigned const long spinTolerance = 4; 



Encoder encL(VOLLA,VOLLB);
Encoder encR(VOLRA,VOLRB);
long encLPosition, encRPosition = 0;

uint8_t buf[9] = { 0 }; 

typedef enum KnobState {
  CW,
  CCW,
  Stop
} KnobState;

bool BTAState, BTBState, BTCState, BTDState, FXLState, FXRState, StartState = LOW;
bool BTAHwState, BTBHwState, BTCHwState, BTDHwState, FXLHwState, FXRHwState, StartHwState = LOW;
KnobState VOLLState, VOLRState = Stop;
unsigned long BTADebounce, BTBDebounce, BTCDebounce, BTDDebounce, FXLDebounce, FXRDebounce, StartDebounce = 0;

void setup() {

  Serial.begin(115200);

  setupButton(BTA);
  setupButton(BTB);
  setupButton(BTC);
  setupButton(BTD);
  setupButton(FXL);
  setupButton(FXR);
  setupButton(START);

  setupLED(BTLA);
  setupLED(BTLB);
  setupLED(BTLC);
  setupLED(BTLD);
  setupLED(FXLL);
  setupLED(FXLR);
  setupLED(STARTL);
}

void setupButton(int button)
{
  pinMode(button, INPUT_PULLUP);
}

void setupLED(int led)
{
  pinMode(led, OUTPUT);
}

bool buttonStates[MAX_BUTTONS] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW};

void checkButton(int button)
{
  int hardwareState = digitalRead(button);

  if (hardwareState != getButtonHwState(button))
  {
    setDebounce(button, millis());
  }

  if ((millis() - getDebounce(button)) > debounceDelay)
  {
    if (hardwareState == HIGH)
    {
      if (getButtonState(button) == HIGH)
      {
        lightOff(button);
        setButtonState(button, LOW);
        buttonReleaseAction(button);
      }
    }
    else
    {
      if (getButtonState(button) == LOW)
      {
        lightOn(button);
        releaseKey(button);
        setButtonState(button, HIGH);
        buttonPressAction(button);
        Serial.write(button);
      }
    }
  }
  setButtonHwState(button, hardwareState);
}

void lightOff(int inputPin)
{
  digitalWrite(buttonToLight(inputPin), LOW);
}

void lightOn(int inputPin)
{
  digitalWrite(buttonToLight(inputPin), HIGH);  
}

void checkKnob(int knobA, int knobB)
{
  int readPosition;
  if(knobA == VOLLA || knobB == VOLLB)
  {
    readPosition = encL.read();
    
    if(readPosition > (long)(encLPosition + spinTolerance))
    {
      knobLActionCCW();
      encLPosition = readPosition;
    }
    else if(readPosition < (long)(encLPosition - spinTolerance) )
    {
      knobLActionCW();
      encLPosition = readPosition;
    }
    else
    {
      knobLActionStop();
    }
    
  }
  else
  {
    readPosition = encR.read();
    
    if(readPosition > (long)(encRPosition + spinTolerance))
    {
      knobRActionCCW();
      encRPosition = readPosition;
    }
    else if(readPosition < (long)(encRPosition - spinTolerance) )
    {
      knobRActionCW();
      encRPosition = readPosition;
    }
    else
    {
      knobRActionStop();
    }

  }
}

void knobLActionCW()
{
  debugInput("Spin",VOLLA);
  setVolLState(CW);
}

void knobLActionCCW()
{
  debugInput("Spin",VOLLB);
  setVolLState(CCW);
}

void knobRActionCW()
{
  debugInput("Spin",VOLRA);
  setVolRState(CW);
}

void knobRActionCCW()
{
  debugInput("Spin",VOLRB);
  setVolRState(CCW);
}

void knobLActionStop()
{
  setVolLState(Stop);
}

void knobRActionStop()
{
  setVolRState(Stop);
}

void setVolLState(KnobState state)
{

  if(VOLLState != state)
  {
    VOLLState = state;
  }
}

void setVolRState(KnobState state)
{

  if(VOLRState != state)
  {
    VOLRState = state;
  }
}

void buttonPressAction(int button)
{
  debugInput("Pressed", button);
  buf[2] = getKey(button);  // 버튼이 눌릴 때 버퍼에 해당 키 값 저장
}

void buttonReleaseAction(int button)
{
  debugInput("Released", button);
  buf[2] = 0;  // 버튼이 떼어질 때 버퍼 초기화
  Serial.write(buf, 9);
}

void serialFromState()
{
  int buttons[7] = {BTA,BTB,BTC,BTD,FXL,FXR,START};
  int slotIndex = 2;
  for( int i = 0; i < 7; i++)
  {
    if(getButtonState(buttons[i]) == HIGH)
    {
      buf[slotIndex] = getKey(buttons[i]);
      slotIndex++;
    }
  }

  if(slotIndex < 9) // 버퍼 크기만큼 루프 추가
  {
    if(VOLLState == CW)
    {
      buf[slotIndex] = getKey(VOLLB);
      slotIndex++;
    }
    else if (VOLLState == CCW)
    {
      buf[slotIndex] = getKey(VOLLA);
      slotIndex++;
    }
  }

  if(slotIndex < 9) // 버퍼 크기만큼 루프 추가
  {
    if(VOLRState == CW)
    {
      buf[slotIndex] = getKey(VOLRB);
      slotIndex++;
    }
    else if (VOLRState == CCW)
    {
      buf[slotIndex] = getKey(VOLRA);
      slotIndex++;
    }
  }

  for(int i = slotIndex; i < 9; i++) // 버퍼 크기만큼 루프 추가
  {
    buf[slotIndex] = 0;
  }

  // 메시지 전송
  Serial.write(buf, 9); // 버퍼 크기만큼 수정
}

uint8_t getKey(int inputPin)
{
  switch(inputPin)
  {
    case BTA:
    {
      return keyBTA;
      break;
    }
    case BTB:
    {
      return keyBTB;
      break;
    }
    case BTC:
    {
      return keyBTC;
      break;
    }
    case BTD:
    {
      return keyBTD;
      break;
    }
    case FXL:
    {
      return keyFXL;
      break;
    }
    case FXR:
    {
      return keyFXR;
      break;
    }
    case START:
    {
      return keySTART;
      break;
    }
    case VOLLA:
    {
      return keyVOLLCCW;
      break;
    }
    case VOLLB:
    {
      return keyVOLLCW;
      break;
    }
    case VOLRA:
    {
      return keyVOLRCCW;
      break;
    }
    case VOLRB:
    {
      return keyVOLRCW;
      break;
    }
  }
}

void debugInput(String message, int inputPort)
{
  #ifdef SERIAL_DEBUGGING
  Serial.print(message);
  Serial.print(" - ");
  Serial.println(getName(inputPort));
  #endif
}

void setButtonState(int button, bool state)
{
  switch (button)
  {
  case BTA:
    buttonStates[0] = state;
    break;
  case BTB:
    buttonStates[1] = state;
    break;
  case BTC:
    buttonStates[2] = state;
    break;
  case BTD:
    buttonStates[3] = state;
    break;
  case FXL:
    buttonStates[4] = state;
    break;
  case FXR:
    buttonStates[5] = state;
    break;
  case START:
    buttonStates[6] = state;
    break;
  }
}


bool getButtonState(int button)
{
  switch (button)
  {
  case BTA:
    return buttonStates[0];
  case BTB:
    return buttonStates[1];
  case BTC:
    return buttonStates[2];
  case BTD:
    return buttonStates[3];
  case FXL:
    return buttonStates[4];
  case FXR:
    return buttonStates[5];
  case START:
    return buttonStates[6];
  }
}


String getName(int inputPort)
{
  switch(inputPort)
  {
    case BTA:
    {
      return "BTA";
      break;
    }
    case BTB:
    {
      return "BTB";
      break;
    }
    case BTC:
    {
      return "BTC";
      break;
    }
    case BTD:
    {
      return "BTD";
      break;
    }
    case FXL:
    {
      return "FXL";
      break;
    }
    case FXR:
    {
      return "FXR";
      break;
    }
    case START:
    {
      return "START";
      break;
    }
    case VOLLA:
    {
      return "VOL-L CW";
      break;
    }
    case VOLLB:
    {
      return "VOL-L CCW";
      break;
    }
    case VOLRA:
    {
      return "VOL-R CW";
      break;
    }
    case VOLRB:
    {
      return "VOL-R CCW";
      break;
    }
  }
}

bool getButtonHwState(int button)
{
  switch(button)
  {
    case BTA:
    {
      return BTAHwState;
      break;
    }
    case BTB:
    {
      return BTBHwState;
      break;
    }
    case BTC:
    {
      return BTCHwState;
      break;
    }
    case BTD:
    {
      return BTDHwState;
      break;
    }
    case FXL:
    {
      return FXLHwState;
      break;
    }
    case FXR:
    {
      return FXRHwState;
      break;
    }
    case START:
    {
      return StartHwState;
      break;
    }
  }
}

void setButtonHwState(int button, bool state)
{
  switch(button)
  {
    case BTA:
    {
      BTAHwState = state;
      break;
    }
    case BTB:
    {
      BTBHwState = state;
      break;
    }
    case BTC:
    {
      BTCHwState = state;
      break;
    }
    case BTD:
    {
      BTDHwState = state;
      break;
    }
    case FXL:
    {
      FXLHwState = state;
      break;
    }
    case FXR:
    {
      FXRHwState = state;
      break;
    }
    case START:
    {
      StartHwState = state;
      break;
    }
  }
}

void setDebounce(int button, unsigned long value)
{
  switch(button)
  {
    case BTA:
    {
      BTADebounce = value;
      break;
    }
    case BTB:
    {
      BTBDebounce = value;
      break;
    }
    case BTC:
    {
      BTCDebounce = value;
      break;
    }
    case BTD:
    {
      BTDDebounce = value;
      break;
    }
    case FXL:
    {
      FXLDebounce = value;
      break;
    }
    case FXR:
    {
      FXRDebounce = value;
      break;
    }
    case START:
    {
      StartDebounce = value;
      break;
    }
  }
}

unsigned long getDebounce(int button)
{
  switch(button)
  {
    case BTA:
    {
      return BTADebounce;
      break;
    }
    case BTB:
    {
      return BTBDebounce;
      break;
    }
    case BTC:
    {
      return BTCDebounce;
      break;
    }
    case BTD:
    {
      return BTDDebounce;
      break;
    }
    case FXL:
    {
      return FXLDebounce;
      break;
    }
    case FXR:
    {
      return FXRDebounce;
      break;
    }
    case START:
    {
      return StartDebounce;
    }
  }
}

int buttonToLight(int button)
{
    switch(button)
  {
    case BTA:
    {
      return BTLA;
      break;
    }
    case BTB:
    {
      return BTLB;
      break;
    }
    case BTC:
    {
      return BTLC;
      break;
    }
    case BTD:
    {
      return BTLD;
      break;
    }
    case FXL:
    {
      return FXLL;
      break;
    }
    case FXR:
    {
      return FXLR;
      break;
    }
    case START:
    {
      return STARTL;
      break;
    }
  }
}

void loop()
{
  checkButton(BTA);
  checkButton(BTB);
  checkButton(BTC);
  checkButton(BTD);
  checkButton(FXL);
  checkButton(FXR);
  checkButton(START);

  checkKnob(VOLLA, VOLLB);
  checkKnob(VOLRA, VOLRB);

  if (hasStateChanged())
  {
    serialFromState();
    delay(5); // 5ms 딜레이
  }
}

bool hasStateChanged() {

  int buttons[7] = {BTA, BTB, BTC, BTD, FXL, FXR, START};
  bool buttonStates[7] = {BTAState, BTBState, BTCState, BTDState, FXLState, FXRState, StartState};

  for (int i = 0; i < 7; i++) {
    if (buttonStates[i] != getButtonState(buttons[i])) {
      return true;
    }
  }

  return VOLLState != Stop || VOLRState != Stop;
}





void pressKey()
{
  //buf[2] = 128;
  //Serial.write(buf, 8);
}

void releaseKey(int button)
{
  buf[2] = getKey(button);  // 버튼이 떼어질 때 해당 키 값 저장
  Serial.write(buf, 9);
}

#include <HID-Project.h>
#include <HID-Settings.h>

// 버튼 변수
int BTA;
int BTB;
int BTC;
int BTD;
int FXL;
int FXR;
int START;
int VOLLA;
int VOLLB;
int VOLRA;
int VOLRB;
int VOLL = 0; // 왼쪽 볼륨 변수
int VOLR = 0; // 오른쪽 볼륨 변수

// 각 버튼에 대응하는 키 코드 상수
int const keyBTA = 1;
int const keyBTB = 2;
int const keyBTC = 3;
int const keyBTD = 4;
int const keyFXL = 5;
int const keyFXR = 6;
int const keySTART = 7;
int const keyVOLLA = 8;
int const keyVOLLB = 9;
int const keyVOLRA = 10;
int const keyVOLRB = 11;

// 토큰을 나누기 위한 문자열 상수
String AA = ",";
String BB = "@";
String CC = "|";
String DD = "$";
String EE = "%";
String FF = "^";
String GG = "&";
String HH = "*";
String II = ":";
String JJ = "`";
String KK = "~";

void setup() {
  // 시리얼 통신 및 게임패드 초기화
  Serial1.begin(115200);
  Gamepad.begin();

  // 버튼에 해당하는 핀을 입력 모드로 설정합니다.
  pinMode(1, INPUT);   // BTA
  pinMode(2, INPUT);  // BTB
  pinMode(3, INPUT);   // BTC
  pinMode(4, INPUT);  // BTD
  pinMode(5, INPUT);   // FXL
  pinMode(6, INPUT);   // FXR
  pinMode(7, INPUT);  // START
  pinMode(8, INPUT);   // VOLLA
  pinMode(9, INPUT);   // VOLLB
  pinMode(10, INPUT);   // VOLRA
  pinMode(11, INPUT);   // VOLRB
}

void loop() {
  // 시리얼에서 데이터를 읽어옵니다.
  String receive = Serial1.readStringUntil('~');
  Serial.println(receive);

  // 받은 문자열을 분해하여 디지털 신호로 설정하고 버튼이 눌렸는지 확인합니다.
  checkAndHandleButtonPress(receive);
  Gamepad.releaseAll();
}

void checkAndHandleButtonPress(String packet) {
  // 토큰 구분자를 정의합니다.
  const char* delimiters = ",@|$%^&*:~";

  // 문자열을 char 배열로 변환합니다.
  char charArray[packet.length() + 1];
  packet.toCharArray(charArray, sizeof(charArray));

  // strtok 함수를 사용하여 토큰을 추출합니다.
  char* token = strtok(charArray, delimiters);

  // 추출한 각 토큰을 순회하면서 버튼의 상태를 설정합니다.
  int pin = 0;
  while (token != nullptr) {
    // 현재 토큰을 정수로 변환하여 버튼이 눌렸으면 press 함수를 호출합니다.
    int buttonState = atoi(token);
    if (buttonState == HIGH) {
      press(pin);
    }

    // 다음 토큰을 추출합니다.
    token = strtok(nullptr, delimiters);

    // 다음 핀으로 이동합니다.
    pin++;
  }
}

// 버튼이 눌렸을 때 호출되는 함수
void press(int buttonIndex) {
  // 버튼이 눌렸을 때 수행할 동작을 여기에 추가합니다.
  // 버튼 인덱스에 따라 다른 동작을 수행하도록 구현할 수 있습니다.
  switch (buttonIndex) {
    case 1: // BTA
      Gamepad.press(keyBTA);
      break;
    case 2: // BTB
      Gamepad.press(keyBTB);
      break;
    case 3: // BTC
      Gamepad.press(keyBTC);
      break;
    case 4: // BTD
      Gamepad.press(keyBTD);
      break;
    case 5: // FXL
      Gamepad.press(keyFXL);
      break;
    case 6: // FXR
      Gamepad.press(keyFXR);
      break;
    case 7: // START
      Gamepad.press(keySTART);
      break;
    case 8: // VOLLA
      xAxis(VOLL);
      break;
    case 9: // VOLLB
      xAxis(-VOLL);
      break;
    case 10: // VOLRA
      yAxis(VOLR);
      break;
    case 11: // VOLRB
      yAxis(-VOLR);
      break;
    // ... (다른 버튼에 대한 동작 추가)
    default:
      // 예외 처리
      break;
  }
}

// X축 값 설정 함수
void xAxis(int value) {
  Gamepad.xAxis(value);
}

// Y축 값 설정 함수
void yAxis(int value) {
  Gamepad.yAxis(value);
}
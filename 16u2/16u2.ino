#include <HID-Project.h>
#include <HID-Settings.h>

void setup() {
  Serial1.begin(115200);
  NKROKeyboard.begin();
  Serial.begin(115200);
}

void loop() {
  while (Serial1.available() > 0) {
    char c = Serial1.read();
    if (c >= 'a' && c <= 'z') {
      // 'e'를 받으면 Enter 키 입력
      if (c == 'e') {
        NKROKeyboard.write(KEY_RETURN);
      } else {
        Serial.print(F("USB: "));
        Serial.print(c);
        NKROKeyboard.releaseAll();
        NKROKeyboard.print(c);
      }
    }
  }
  delay(5);
}

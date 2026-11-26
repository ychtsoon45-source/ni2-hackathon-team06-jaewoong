/*
  라인가드(LineGuard) 분류 장치 예제 - 아두이노 UNO
  6팀 해커트리오 (부천대 전자공학과·스마트팩토리전공)

  라인가드 웹앱에서 "분류 장치 연결 (USB)"로 연결하면
  판정할 때마다 시리얼(9600bps)로 아래 신호가 들어와요.
    OK  -> 양품함 방향으로 서보 회전
    NG  -> 불량함 방향으로 서보 회전
    RE  -> 재검함 방향으로 서보 회전

  배선
    서보모터 신호선 -> D9, 전원 5V, GND
    (선택) 컨베이어 모터 릴레이 -> D7 (HIGH = 동작)
    (선택) 상태 LED: 양품 D3(초록), 불량 D4(빨강), 재검 D5(노랑)
*/
#include <Servo.h>

const int SERVO_PIN = 9;
const int CONVEYOR_PIN = 7;
const int LED_OK = 3, LED_NG = 4, LED_RE = 5;

// 함 위치에 맞게 각도를 조정하세요
const int ANGLE_OK = 30;
const int ANGLE_RE = 90;
const int ANGLE_NG = 150;
const int HOLD_MS = 800;   // 게이트를 열어 두는 시간

Servo gate;
String buf = "";

void setLeds(int ok, int ng, int re) {
  digitalWrite(LED_OK, ok); digitalWrite(LED_NG, ng); digitalWrite(LED_RE, re);
}

void sortTo(int angle, int ok, int ng, int re) {
  setLeds(ok, ng, re);
  gate.write(angle);
  delay(HOLD_MS);
  gate.write(ANGLE_RE);      // 가운데로 복귀
  setLeds(LOW, LOW, LOW);
}

void setup() {
  Serial.begin(9600);
  gate.attach(SERVO_PIN);
  gate.write(ANGLE_RE);
  pinMode(CONVEYOR_PIN, OUTPUT);
  pinMode(LED_OK, OUTPUT); pinMode(LED_NG, OUTPUT); pinMode(LED_RE, OUTPUT);
  digitalWrite(CONVEYOR_PIN, HIGH);   // 컨베이어 동작 시작
  Serial.println("LineGuard sorter ready");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      buf.trim();
      if (buf == "OK")      sortTo(ANGLE_OK, HIGH, LOW, LOW);
      else if (buf == "NG") sortTo(ANGLE_NG, LOW, HIGH, LOW);
      else if (buf == "RE") sortTo(ANGLE_RE, LOW, LOW, HIGH);
      if (buf.length()) { Serial.print("ACK "); Serial.println(buf); }
      buf = "";
    } else if (buf.length() < 8) {
      buf += c;
    }
  }
}

// pca9685_arduino.ino
// Firmware cho Arduino Mega + PCA9685, giao thức dòng lệnh:
//  PING -> PONG
//  CFG N ch0 ... ch{N-1}  (N: 1..16, kênh 0..15, không trùng) -> OK (đồng thời HOME 0°)
//  HOME -> OK
//  SET i angle (0..N-1, 0..180) -> OK
//  SETALL a0 ... a{N-1} -> OK

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

#define SERVO_MIN_TICK 150
#define SERVO_MAX_TICK 600
#define SERVO_MIN_DEG  0
#define SERVO_MAX_DEG  180
#define MAX_CHANNELS   16

int  numServo = 0;
int  servoChannels[MAX_CHANNELS];
int  currentAngles[MAX_CHANNELS];

void printlnOK()    { Serial.println(F("OK")); }
void printlnERR(const __FlashStringHelper* msg) { Serial.print(F("ERR ")); Serial.println(msg); }
void printlnERRs(const String& msg) { Serial.print(F("ERR ")); Serial.println(msg); }

String readLine() {
  while (true) {
    while (Serial.available() == 0) { /* wait */ }
    String s = Serial.readStringUntil('\n');
    s.trim();
    if (s.length() > 0) return s;
  }
}

int clampi(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

void setServoDegRawChannel(int channel, int angleDeg) {
  angleDeg = clampi(angleDeg, SERVO_MIN_DEG, SERVO_MAX_DEG);
  int pulselen = map(angleDeg, SERVO_MIN_DEG, SERVO_MAX_DEG, SERVO_MIN_TICK, SERVO_MAX_TICK);
  if (pulselen < 0) pulselen = 0;
  if (pulselen > 4095) pulselen = 4095;
  pwm.setPWM(channel, 0, pulselen);
}

void homeAll() {
  for (int i = 0; i < numServo; ++i) {
    currentAngles[i] = 0;
    setServoDegRawChannel(servoChannels[i], 0);
    delay(5);
  }
}

bool nextToken(const String& line, int& start, String& out) {
  int n = line.length();
  while (start < n && (line[start] == ' ' || line[start] == '\t' || line[start] == ',')) start++;
  if (start >= n) return false;
  int s = start;
  while (start < n && !(line[start] == ' ' || line[start] == '\t' || line[start] == ',')) start++;
  out = line.substring(s, start);
  out.trim();
  return out.length() > 0;
}

bool parseIntTok(const String& tok, long& out) {
  if (tok.length() == 0) return false;
  bool neg = false; int i = 0;
  if (tok[0] == '-') { neg = true; i = 1; if (i >= tok.length()) return false; }
  for (; i < (int)tok.length(); ++i) {
    if (tok[i] < '0' || tok[i] > '9') return false;
  }
  out = tok.toInt();
  return true;
}

void handleLine(const String& line) {
  int pos = 0; String tok;
  if (!nextToken(line, pos, tok)) return;
  tok.toUpperCase();

  if (tok == F("PING")) { Serial.println(F("PONG")); return; }

  if (tok == F("CFG")) {
    String t; long N;
    if (!nextToken(line, pos, t) || !parseIntTok(t, N)) { printlnERR(F("CFG missing N")); return; }
    if (N < 1 || N > MAX_CHANNELS) { printlnERR(F("N out of range 1..16")); return; }
    int tmpCh[MAX_CHANNELS]; bool used[MAX_CHANNELS] = {false};
    for (int i = 0; i < N; ++i) {
      if (!nextToken(line, pos, t)) { printlnERR(F("CFG missing channels")); return; }
      long ch;
      if (!parseIntTok(t, ch)) { printlnERR(F("channel not int")); return; }
      if (ch < 0 || ch > 15) { printlnERR(F("channel out of 0..15")); return; }
      if (used[ch]) { printlnERR(F("duplicate channel")); return; }
      used[ch] = true; tmpCh[i] = (int)ch;
    }
    numServo = (int)N;
    for (int i = 0; i < numServo; ++i) { servoChannels[i] = tmpCh[i]; currentAngles[i] = 0; }
    homeAll();
    printlnOK();
    return;
  }

  if (tok == F("HOME")) {
    if (numServo <= 0) { printlnERR(F("not configured")); return; }
    homeAll(); printlnOK(); return;
  }

  if (tok == F("SET")) {
    if (numServo <= 0) { printlnERR(F("not configured")); return; }
    String t; long idx, ang;
    if (!nextToken(line, pos, t) || !parseIntTok(t, idx)) { printlnERR(F("SET missing index")); return; }
    if (!nextToken(line, pos, t) || !parseIntTok(t, ang)) { printlnERR(F("SET missing angle")); return; }
    if (idx < 0 || idx >= numServo) { printlnERR(F("index out of range")); return; }
    int a = clampi((int)ang, SERVO_MIN_DEG, SERVO_MAX_DEG);
    setServoDegRawChannel(servoChannels[idx], a);
    currentAngles[idx] = a;
    printlnOK();
    return;
  }

  if (tok == F("SETALL")) {
    if (numServo <= 0) { printlnERR(F("not configured")); return; }
    String t; long ang;
    for (int i = 0; i < numServo; ++i) {
      if (!nextToken(line, pos, t) || !parseIntTok(t, ang)) { printlnERR(F("SETALL missing angles")); return; }
      int a = clampi((int)ang, SERVO_MIN_DEG, SERVO_MAX_DEG);
      setServoDegRawChannel(servoChannels[i], a);
      currentAngles[i] = a;
      delay(2);
    }
    printlnOK();
    return;
  }

  printlnERR(F("unknown command"));
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pwm.begin();
  pwm.setPWMFreq(50);
  delay(10);
  Serial.println(F("READY"));
}

void loop() {
  if (Serial.available()) {
    String line = readLine();
    handleLine(line);
  }
}

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>

#define OLED_SDA 21
#define OLED_SCL 22
#define JOY_X 34
#define JOY_Y 35
#define BTN_JOY 25
#define BTN_LEFT 26
#define BTN_RIGHT 27
#define BTN_SELECT 14
#define BTN_BACK 13
#define BUZZER_PIN 4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define CELL 8
#define PLAY_OFFSET_X 16
#define PLAY_OFFSET_Y 0

enum GameState { MENU, PLAYING, PAUSED, GAME_OVER };
GameState currentState = MENU;

const int TOTAL_GAMES = 10;
const char* gameNames[] = {
  "Pong", "Snake", "Breakout", "Invaders", "Flappy",
  "Racer", "Whack-A-Mole", "Simon", "Tetris", "Lights Out"
};
const char* gameDescriptions[] = {
  "Classic ball duel", "Eat and grow", "Smash the bricks",
  "Defend the earth", "Fly through pipes", "Dodge the traffic",
  "Test your reflexes", "Follow the pattern", "Stack and clear",
  "Solve the puzzle"
};
int selectedGame = 0, currentGame = 0, highScore = 0;

bool btnLeft, btnRight, btnSelect, btnBack, btnJoy;
bool prevLeft, prevRight, prevSelect, prevBack, prevJoy;
unsigned long lastDebounce = 0;
unsigned long backPressTime = 0;
unsigned long joyPressTime = 0;
unsigned long selectPressTime = 0;
const unsigned long debounceDelay = 40;
const unsigned long longPressDelay = 600;

bool backLongFired = false;
bool joyLongFired = false;
bool selectLongFired = false;

int score = 0;
bool soundEnabled = true;
bool gameOverSoundPlayed = false;
Preferences prefs;

unsigned long soundMsgTime = 0;
bool showSoundMsg = false;

struct Note {
  int freq;
  int duration;
  int pause;
};

#define MELODY_MAX 16
Note melody[MELODY_MAX];
int melodyLen = 0;
int melodyIdx = 0;
unsigned long noteStartTime = 0;
bool melodyPlaying = false;

void playMelody(const Note* notes, int len) {
  if (!soundEnabled) return;
  melodyPlaying = false;
  noTone(BUZZER_PIN);
  int n = len < MELODY_MAX ? len : MELODY_MAX;
  for (int i = 0; i < n; i++) melody[i] = notes[i];
  melodyLen = n;
  melodyIdx = 0;
  melodyPlaying = true;
  noteStartTime = millis();
  tone(BUZZER_PIN, melody[0].freq, melody[0].duration);
}

void updateSound() {
  if (!melodyPlaying) return;
  unsigned long elapsed = millis() - noteStartTime;
  int total = melody[melodyIdx].duration + melody[melodyIdx].pause;
  if (elapsed >= (unsigned long)total) {
    melodyIdx++;
    if (melodyIdx >= melodyLen) {
      melodyPlaying = false;
      noTone(BUZZER_PIN);
      return;
    }
    noteStartTime = millis();
    tone(BUZZER_PIN, melody[melodyIdx].freq, melody[melodyIdx].duration);
  }
}

void soundTone(int freq, int dur) {
  if (!soundEnabled || melodyPlaying) return;
  tone(BUZZER_PIN, freq, dur);
}

void soundGameStart() {
  Note n[] = {{440, 100, 10}, {660, 100, 10}, {880, 150, 0}};
  playMelody(n, 3);
}

void soundGameOver() {
  Note n[] = {{400, 200, 20}, {300, 200, 20}, {200, 400, 0}};
  playMelody(n, 3);
}

void soundVictory() {
  Note n[] = {{523, 100, 10}, {659, 100, 10}, {784, 100, 10}, {1047, 300, 0}};
  playMelody(n, 4);
}

void soundScore() {
  Note n[] = {{600, 50, 10}, {1200, 50, 0}};
  playMelody(n, 2);
}

void soundPause() {
  Note n[] = {{800, 60, 10}, {1000, 60, 0}};
  playMelody(n, 2);
}

void soundLineClear() {
  Note n[] = {{800, 50, 10}, {1200, 50, 10}, {1600, 100, 0}};
  playMelody(n, 3);
}

void soundEat()       { soundTone(1000, 50); }
void soundShoot()     { soundTone(2000, 30); }
void soundBounce()    { soundTone(800, 40); }
void soundBreakBrick(){ soundTone(1500, 40); }
void soundFlap()      { soundTone(600, 30); }
void soundHit()       { soundTone(1200, 80); }
void soundDrop()      { soundTone(400, 40); }
void soundToggle()    { soundTone(1000, 30); }
void soundMenuClick() { soundTone(1200, 20); }
void soundMenuSelect(){ soundTone(880, 80); }
void soundMenuBack()  { soundTone(600, 60); }

void clearPlayArea() {
  display.fillRect(PLAY_OFFSET_X, PLAY_OFFSET_Y, 64, 64, SSD1306_BLACK);
}

void setPixel(int x, int y, bool state) {
  if (x < 0 || x >= 8 || y < 0 || y >= 8) return;
  if (!state) return;
  display.fillRect(PLAY_OFFSET_X + x * CELL, PLAY_OFFSET_Y + y * CELL,
                   CELL - 1, CELL - 1, SSD1306_WHITE);
}

void drawSidebar(const char* label, int value) {
  display.fillRect(0, 0, PLAY_OFFSET_X, 64, SSD1306_BLACK);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 2);
  display.println(label);
  display.setTextSize(2);
  display.setCursor(0, 16);
  display.println(value);
}

void readInputs() {
  prevLeft = btnLeft;
  prevRight = btnRight;
  prevSelect = btnSelect;
  prevBack = btnBack;
  prevJoy = btnJoy;
  bool rawLeft = !digitalRead(BTN_LEFT);
  bool rawRight = !digitalRead(BTN_RIGHT);
  bool rawSelect = !digitalRead(BTN_SELECT);
  bool rawBack = !digitalRead(BTN_BACK);
  bool rawJoy = !digitalRead(BTN_JOY);
  if (millis() - lastDebounce > debounceDelay) {
    if (rawLeft != btnLeft) { btnLeft = rawLeft; lastDebounce = millis(); }
    if (rawRight != btnRight) { btnRight = rawRight; lastDebounce = millis(); }
    if (rawSelect != btnSelect) { btnSelect = rawSelect; lastDebounce = millis(); }
    if (rawBack != btnBack) { btnBack = rawBack; lastDebounce = millis(); }
    if (rawJoy != btnJoy) { btnJoy = rawJoy; lastDebounce = millis(); }
  }
  if (btnBack && !prevBack) { backPressTime = millis(); backLongFired = false; }
  if (btnJoy && !prevJoy) { joyPressTime = millis(); joyLongFired = false; }
  if (btnSelect && !prevSelect) { selectPressTime = millis(); selectLongFired = false; }
  if (!btnBack) backLongFired = false;
  if (!btnJoy) joyLongFired = false;
  if (!btnSelect) selectLongFired = false;
}

bool pressedLeft()  { return btnLeft && !prevLeft; }
bool pressedRight() { return btnRight && !prevRight; }
bool pressedSelect(){ return btnSelect && !prevSelect; }
bool pressedBack()  { return btnBack && !prevBack; }
bool pressedJoy()   { return btnJoy && !prevJoy; }

bool longPressedBack() {
  if (!btnBack) return false;
  if (backLongFired) return false;
  if (millis() - backPressTime >= longPressDelay) {
    backLongFired = true;
    return true;
  }
  return false;
}

bool longPressedJoy() {
  if (!btnJoy) return false;
  if (joyLongFired) return false;
  if (millis() - joyPressTime >= longPressDelay) {
    joyLongFired = true;
    return true;
  }
  return false;
}

bool longPressedSelect() {
  if (!btnSelect) return false;
  if (selectLongFired) return false;
  if (millis() - selectPressTime >= longPressDelay) {
    selectLongFired = true;
    return true;
  }
  return false;
}

namespace Pong {
  int ballX = 4, ballY = 4, dx = 1, dy = 1;
  int paddleY = 3, aiY = 3;
  unsigned long lastMove = 0;

  void init() { ballX = 4; ballY = 4; dx = 1; dy = 1; paddleY = 3; aiY = 3; score = 0; lastMove = 0; }

  void control() {
    int y = analogRead(JOY_Y);
    if (y < 1000) paddleY = max(0, paddleY - 1);
    if (y > 3000) paddleY = min(6, paddleY + 1);
  }

  void update() {
    if (millis() - lastMove < 90) return;
    lastMove = millis();
    ballX += dx; ballY += dy;
    if (ballY <= 0 || ballY >= 7) { dy = -dy; soundBounce(); }
    if (aiY + 1 < ballY) aiY++; else if (aiY > ballY) aiY--;
    if (ballX <= 0) {
      if (ballY >= paddleY && ballY <= paddleY + 1) { dx = 1; soundBounce(); }
      else { currentState = GAME_OVER; return; }
    }
    if (ballX >= 7) {
      if (ballY >= aiY && ballY <= aiY + 1) { dx = -1; soundBounce(); }
      else { score++; soundScore(); ballX = 4; ballY = 4; dx = 1; dy = 1; }
    }
  }

  void draw() {
    clearPlayArea();
    setPixel(0, paddleY, true); setPixel(0, paddleY + 1, true);
    setPixel(7, aiY, true); setPixel(7, aiY + 1, true);
    setPixel(ballX, ballY, true);
    drawSidebar("PTS", score);
  }
}

namespace Snake {
  int sx[64], sy[64];
  int len = 3, dir = 1;
  int fx, fy;
  unsigned long lastMove = 0;

  void spawnFood() {
    int attempts = 0;
    bool conflict = true;
    while (conflict && attempts < 100) {
      conflict = false;
      fx = random(0, 8); fy = random(0, 8);
      for (int i = 0; i < len; i++) if (sx[i] == fx && sy[i] == fy) conflict = true;
      attempts++;
    }
  }

  void init() {
    len = 3; dir = 1; score = 0; lastMove = 0;
    sx[0] = 3; sy[0] = 3;
    sx[1] = 2; sy[1] = 3;
    sx[2] = 1; sy[2] = 3;
    spawnFood();
  }

  void control() {
    int x = analogRead(JOY_X), y = analogRead(JOY_Y);
    if (x < 1000 && dir != 1) dir = 3;
    if (x > 3000 && dir != 3) dir = 1;
    if (y < 1000 && dir != 2) dir = 0;
    if (y > 3000 && dir != 0) dir = 2;
  }

  void update() {
    if (millis() - lastMove < 200) return;
    lastMove = millis();
    for (int i = len - 1; i > 0; i--) { sx[i] = sx[i - 1]; sy[i] = sy[i - 1]; }
    if (dir == 0) sy[0]--;
    if (dir == 1) sx[0]++;
    if (dir == 2) sy[0]++;
    if (dir == 3) sx[0]--;
    if (sx[0] < 0 || sx[0] > 7 || sy[0] < 0 || sy[0] > 7) { currentState = GAME_OVER; return; }
    for (int i = 1; i < len; i++) if (sx[0] == sx[i] && sy[0] == sy[i]) { currentState = GAME_OVER; return; }
    if (sx[0] == fx && sy[0] == fy) { len++; score += 10; spawnFood(); soundEat(); }
  }

  void draw() {
    clearPlayArea();
    for (int i = 0; i < len; i++) setPixel(sx[i], sy[i], true);
    setPixel(fx, fy, true);
    drawSidebar("LEN", len);
  }
}

namespace Breakout {
  int paddleX = 3;
  int ballX = 4, ballY = 6;
  int dx = 1, dy = -1;
  bool brickMap[8];
  unsigned long lastMove = 0;

  void init() {
    paddleX = 3; ballX = 4; ballY = 6; dx = 1; dy = -1; score = 0; lastMove = 0;
    for (int i = 0; i < 8; i++) brickMap[i] = true;
  }

  int brickCount() {
    int c = 0;
    for (int i = 0; i < 8; i++) if (brickMap[i]) c++;
    return c;
  }

  void control() {
    int x = analogRead(JOY_X);
    if (x < 1000) paddleX = max(0, paddleX - 1);
    if (x > 3000) paddleX = min(6, paddleX + 1);
  }

  void update() {
    if (millis() - lastMove < 90) return;
    lastMove = millis();
    ballX += dx; ballY += dy;
    if (ballX <= 0 || ballX >= 7) { dx = -dx; soundBounce(); }
    if (ballY <= 0) {
      if (brickMap[ballX]) { brickMap[ballX] = false; score += 5; soundBreakBrick(); }
      dy = 1;
      if (brickCount() == 0) { soundVictory(); currentState = GAME_OVER; return; }
    }
    if (ballY >= 7) {
      if (ballX >= paddleX && ballX <= paddleX + 2) { dy = -1; soundBounce(); }
      else { currentState = GAME_OVER; return; }
    }
  }

  void draw() {
    clearPlayArea();
    setPixel(paddleX, 7, true); setPixel(paddleX + 1, 7, true); setPixel(paddleX + 2, 7, true);
    setPixel(ballX, ballY, true);
    for (int i = 0; i < 8; i++) if (brickMap[i]) setPixel(i, 0, true);
    drawSidebar("PTS", score);
  }
}

namespace Invaders {
  int playerX = 3, alienX = 0, alienDir = 1;
  int alienY = 1;
  int bulletX = -1, bulletY = -1;
  unsigned long lastMove = 0;
  unsigned long lastDescend = 0;
  unsigned long descendInterval = 5000;

  void init() {
    playerX = 3; alienX = 0; alienDir = 1; alienY = 1;
    bulletX = -1; bulletY = -1; score = 0; lastMove = 0;
    lastDescend = millis();
    descendInterval = 5000;
  }

  void control() {
    int x = analogRead(JOY_X);
    if (x < 1000) playerX = max(0, playerX - 1);
    if (x > 3000) playerX = min(7, playerX + 1);
    if (pressedSelect() && bulletX == -1) { bulletX = playerX; bulletY = 6; soundShoot(); }
  }

  void update() {
    if (millis() - lastMove < 120) return;
    lastMove = millis();
    alienX += alienDir;
    if (alienX <= 0 || alienX >= 7) alienDir = -alienDir;
    if (bulletY > 0) bulletY--; else bulletX = -1;
    if (bulletX == alienX && bulletY == alienY) {
      score += 10; bulletX = -1;
      alienX = random(0, 8);
      alienY = 1;
      soundHit();
    }
    if (millis() - lastDescend > descendInterval) {
      lastDescend = millis();
      alienY++;
      if (descendInterval > 2000) descendInterval -= 500;
      if (alienY >= 7) { currentState = GAME_OVER; return; }
    }
    if (alienY == 7 && alienX == playerX) { currentState = GAME_OVER; return; }
  }

  void draw() {
    clearPlayArea();
    setPixel(playerX, 7, true);
    setPixel(alienX, alienY, true);
    if (bulletX != -1) setPixel(bulletX, bulletY, true);
    drawSidebar("PTS", score);
  }
}

namespace Flappy {
  int birdY = 4, pipeX = 7, gapY = 3, vel = 0;
  unsigned long lastFrame = 0;

  void init() { birdY = 4; pipeX = 7; gapY = random(2, 5); vel = 0; score = 0; lastFrame = 0; }

  void control() {
    if (pressedSelect()) { vel = -2; soundFlap(); }
  }

  void update() {
    if (millis() - lastFrame < 120) return;
    lastFrame = millis();
    vel++; birdY += vel; pipeX--;
    if (pipeX < 0) { pipeX = 7; gapY = random(1, 6); score++; soundScore(); }
    if (birdY < 0 || birdY > 7) { currentState = GAME_OVER; return; }
    if (pipeX == 1 && (birdY < gapY || birdY > gapY + 2)) { currentState = GAME_OVER; return; }
  }

  void draw() {
    clearPlayArea();
    setPixel(1, birdY, true);
    for (int i = 0; i < 8; i++) if (i < gapY || i > gapY + 2) setPixel(pipeX, i, true);
    drawSidebar("PTS", score);
  }
}

namespace Racer {
  int carX = 3, obsX = 7, obsY = 0;
  unsigned long lastMove = 0;

  void init() { carX = 3; obsX = 7; obsY = 0; score = 0; lastMove = 0; }

  void control() {
    int x = analogRead(JOY_X);
    if (x < 1000) carX = max(0, carX - 1);
    if (x > 3000) carX = min(7, carX + 1);
  }

  void update() {
    if (millis() - lastMove < 180) return;
    lastMove = millis();
    obsY++;
    if (obsY > 7) { obsY = 0; obsX = random(0, 8); score++; soundScore(); }
    if (obsY == 7 && obsX == carX) { currentState = GAME_OVER; return; }
  }

  void draw() {
    clearPlayArea();
    setPixel(carX, 7, true);
    setPixel(obsX, obsY, true);
    drawSidebar("PTS", score);
  }
}

namespace Whack {
  int moleX = -1, moleY = -1;
  unsigned long lastSpawn = 0;
  unsigned long moleTimer = 0;

  void init() { score = 0; lastSpawn = millis(); moleX = -1; moleY = -1; moleTimer = 0; }

  int zoneOf(int v) {
    if (v < 1500) return 0;
    if (v > 2500) return 2;
    return 1;
  }

  void control() {
    if (pressedSelect() && moleX != -1) {
      int x = analogRead(JOY_X), y = analogRead(JOY_Y);
      int zx = zoneOf(x), zy = zoneOf(y);
      int mzx = moleX / 3, mzy = moleY / 3;
      if (zx == mzx && zy == mzy) {
        score += 10; moleX = -1; moleY = -1; soundHit();
      }
    }
  }

  void update() {
    if (millis() - lastSpawn > 1000 && moleX == -1) {
      moleX = random(0, 8); moleY = random(0, 8);
      lastSpawn = millis();
      moleTimer = millis();
    }
    if (moleX != -1 && millis() - moleTimer > 2500) {
      moleX = -1; moleY = -1;
    }
  }

  void draw() {
    clearPlayArea();
    if (moleX != -1) setPixel(moleX, moleY, true);
    int x = analogRead(JOY_X), y = analogRead(JOY_Y);
    int cx = zoneOf(x) * 3 + 1;
    int cy = zoneOf(y) * 3 + 1;
    if (millis() % 400 < 200) setPixel(cx, cy, true);
    drawSidebar("PTS", score);
  }
}

namespace Simon {
  int seq[8];
  int seqLen = 0, inputIdx = 0;
  unsigned long lastFlash = 0;
  unsigned long nextRoundTime = 0;
  int flashIdx = 0;
  bool showing = false;
  bool waitingNextRound = false;

  void init() {
    seqLen = 0; inputIdx = 0; score = 0; showing = false;
    lastFlash = 0; flashIdx = 0; waitingNextRound = false; nextRoundTime = 0;
  }

  void control() {
    if (showing || waitingNextRound) return;
    if (pressedSelect()) {
      int x = analogRead(JOY_X), y = analogRead(JOY_Y);
      int input = -1;
      if (x < 1500 && y < 1500) input = 0;
      else if (x > 2500 && y < 1500) input = 1;
      else if (x < 1500 && y > 2500) input = 2;
      else if (x > 2500 && y > 2500) input = 3;
      if (input != -1 && seqLen > 0) {
        if (input == seq[inputIdx]) {
          inputIdx++;
          if (inputIdx >= seqLen) {
            score += 10;
            soundScore();
            waitingNextRound = true;
            nextRoundTime = millis();
          }
        } else {
          currentState = GAME_OVER;
        }
      }
    }
  }

  void update() {
    if (waitingNextRound) {
      if (millis() - nextRoundTime > 500) { waitingNextRound = false; seqLen = 0; }
      return;
    }
    if (seqLen == 0) {
      seqLen = min(score / 10 + 1, 8);
      for (int i = 0; i < seqLen; i++) seq[i] = random(0, 4);
      inputIdx = 0;
      showing = true;
      flashIdx = 0;
      lastFlash = millis();
    }
    if (showing && millis() - lastFlash > 500) {
      flashIdx++;
      lastFlash = millis();
      if (flashIdx > seqLen) { showing = false; flashIdx = 0; }
    }
  }

  void draw() {
    clearPlayArea();
    if (showing && flashIdx < seqLen) {
      int c = seq[flashIdx];
      if (c == 0) setPixel(1, 1, true);
      if (c == 1) setPixel(6, 1, true);
      if (c == 2) setPixel(1, 6, true);
      if (c == 3) setPixel(6, 6, true);
    }
    drawSidebar("RND", score / 10);
  }
}

namespace Tetris {
  int blockX = 3, blockY = 0;
  int stack[8][8];
  unsigned long lastDrop = 0;

  void init() { blockX = 3; blockY = 0; score = 0; lastDrop = 0; memset(stack, 0, sizeof(stack)); }

  void control() {
    int x = analogRead(JOY_X);
    if (x < 1000) blockX = max(0, blockX - 1);
    if (x > 3000) blockX = min(7, blockX + 1);
  }

  void update() {
    if (millis() - lastDrop < 500) return;
    lastDrop = millis();
    if (blockY < 7 && stack[blockY + 1][blockX] == 0) {
      blockY++;
    } else {
      stack[blockY][blockX] = 1;
      blockY = 0; blockX = 3;
      soundDrop();
      bool full = true;
      for (int i = 0; i < 8; i++) if (stack[7][i] == 0) full = false;
      if (full) {
        score += 100;
        for (int r = 6; r >= 0; r--)
          for (int c = 0; c < 8; c++)
            stack[r + 1][c] = stack[r][c];
        for (int c = 0; c < 8; c++) stack[0][c] = 0;
        soundLineClear();
      }
      for (int i = 0; i < 8; i++) if (stack[0][i] == 1) { currentState = GAME_OVER; return; }
    }
  }

  void draw() {
    clearPlayArea();
    for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) if (stack[r][c]) setPixel(c, r, true);
    setPixel(blockX, blockY, true);
    drawSidebar("PTS", score);
  }
}

namespace LightsOut {
  bool grid[8][8];
  int cursorX = 0, cursorY = 0;
  unsigned long lastMove = 0;

  void init() {
    score = 0; cursorX = 0; cursorY = 0; lastMove = 0;
    for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) grid[r][c] = random(0, 2);
  }

  void toggle(int x, int y) { if (x >= 0 && x < 8 && y >= 0 && y < 8) grid[y][x] = !grid[y][x]; }

  void control() {
    if (millis() - lastMove < 150) return;
    int x = analogRead(JOY_X), y = analogRead(JOY_Y);
    bool moved = false;
    if (x < 1000) { cursorX = max(0, cursorX - 1); moved = true; }
    if (x > 3000) { cursorX = min(7, cursorX + 1); moved = true; }
    if (y < 1000) { cursorY = max(0, cursorY - 1); moved = true; }
    if (y > 3000) { cursorY = min(7, cursorY + 1); moved = true; }
    if (moved) lastMove = millis();
    if (pressedSelect()) {
      toggle(cursorX, cursorY);
      toggle(cursorX + 1, cursorY); toggle(cursorX - 1, cursorY);
      toggle(cursorX, cursorY + 1); toggle(cursorX, cursorY - 1);
      score++;
      soundToggle();
      bool win = true;
      for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) if (grid[r][c]) win = false;
      if (win) { soundVictory(); currentState = GAME_OVER; }
    }
  }

  void update() {}

  void draw() {
    clearPlayArea();
    for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) if (grid[r][c]) setPixel(c, r, true);
    if (millis() % 500 < 250) setPixel(cursorX, cursorY, true);
    drawSidebar("MOV", score);
  }
}

void gameInit(int g) {
  switch (g) {
    case 0: Pong::init(); break;
    case 1: Snake::init(); break;
    case 2: Breakout::init(); break;
    case 3: Invaders::init(); break;
    case 4: Flappy::init(); break;
    case 5: Racer::init(); break;
    case 6: Whack::init(); break;
    case 7: Simon::init(); break;
    case 8: Tetris::init(); break;
    case 9: LightsOut::init(); break;
  }
}

void gameControl(int g) {
  switch (g) {
    case 0: Pong::control(); break;
    case 1: Snake::control(); break;
    case 2: Breakout::control(); break;
    case 3: Invaders::control(); break;
    case 4: Flappy::control(); break;
    case 5: Racer::control(); break;
    case 6: Whack::control(); break;
    case 7: Simon::control(); break;
    case 8: Tetris::control(); break;
    case 9: LightsOut::control(); break;
  }
}

void gameUpdate(int g) {
  switch (g) {
    case 0: Pong::update(); break;
    case 1: Snake::update(); break;
    case 2: Breakout::update(); break;
    case 3: Invaders::update(); break;
    case 4: Flappy::update(); break;
    case 5: Racer::update(); break;
    case 6: Whack::update(); break;
    case 7: Simon::update(); break;
    case 8: Tetris::update(); break;
    case 9: LightsOut::update(); break;
  }
}

void gameDraw(int g) {
  switch (g) {
    case 0: Pong::draw(); break;
    case 1: Snake::draw(); break;
    case 2: Breakout::draw(); break;
    case 3: Invaders::draw(); break;
    case 4: Flappy::draw(); break;
    case 5: Racer::draw(); break;
    case 6: Whack::draw(); break;
    case 7: Simon::draw(); break;
    case 8: Tetris::draw(); break;
    case 9: LightsOut::draw(); break;
  }
}

void showWelcomeScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  display.drawRect(2, 2, 124, 60, SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(28, 8);
  display.println("* NIKOLA *");
  display.setTextSize(2);
  display.setCursor(10, 22);
  display.println("NIKOLAGAME");
  display.setTextSize(1);
  display.setCursor(34, 42);
  display.println("CONSOLE v1.4");
  display.display();
  Note n[] = {{523, 120, 10}, {659, 120, 10}, {784, 120, 10}, {1047, 300, 0}};
  playMelody(n, 4);
  unsigned long start = millis();
  int step = 0;
  while (millis() - start < 2000) {
    updateSound();
    if (millis() - start > 800 && step == 0) {
      display.fillRect(14, 54, 30, 4, SSD1306_WHITE);
      display.display();
      step++;
    }
    if (millis() - start > 1100 && step == 1) {
      display.fillRect(52, 54, 30, 4, SSD1306_WHITE);
      display.display();
      step++;
    }
    if (millis() - start > 1400 && step == 2) {
      display.fillRect(90, 54, 30, 4, SSD1306_WHITE);
      display.display();
      step++;
    }
    delay(20);
  }
}

void drawMenu() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.fillRect(0, 0, 128, 12, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(30, 2);
  display.println("NIKOLAGAME");
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 16);
  display.print("<");
  display.setCursor(122, 16);
  display.print(">");
  display.setTextSize(2);
  int nameLen = strlen(gameNames[selectedGame]);
  int nameX = (128 - nameLen * 12) / 2;
  if (nameX < 10) nameX = 10;
  display.setCursor(nameX, 16);
  display.println(gameNames[selectedGame]);
  display.setTextSize(1);
  int descLen = strlen(gameDescriptions[selectedGame]);
  int descX = (128 - descLen * 6) / 2;
  if (descX < 0) descX = 0;
  display.setCursor(descX, 38);
  display.println(gameDescriptions[selectedGame]);
  display.drawLine(0, 48, 128, 48, SSD1306_WHITE);
  display.setCursor(2, 52);
  display.print("Best: ");
  display.print(highScore);
  display.setCursor(80, 52);
  display.print(selectedGame + 1);
  display.print("/");
  display.print(TOTAL_GAMES);
  if (showSoundMsg) {
    if (millis() - soundMsgTime < 800) {
      display.fillRect(30, 24, 68, 14, SSD1306_BLACK);
      display.drawRect(30, 24, 68, 14, SSD1306_WHITE);
      display.setTextSize(1);
      display.setCursor(36, 28);
      display.print(soundEnabled ? "SOUND: ON" : "SOUND: OFF");
    } else {
      showSoundMsg = false;
    }
  }
  display.display();
}

void drawPaused() {
  display.clearDisplay();
  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(8, 12);
  display.println("PAUSED");
  display.setTextSize(1);
  display.setCursor(8, 34);
  display.println("Select: resume");
  display.setCursor(8, 46);
  display.println("Back: exit to menu");
  display.display();
}

void drawGameOver() {
  display.clearDisplay();
  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(4, 4);
  display.println("GAME OVER");
  display.setTextSize(1);
  display.setCursor(4, 24);
  display.print("Score: ");
  display.println(score);
  display.setCursor(4, 36);
  display.print("Best: ");
  display.println(highScore);
  if (score >= highScore && score > 0) {
    display.setCursor(4, 48);
    display.println("* NEW RECORD *");
  } else {
    display.setCursor(4, 48);
    display.println("Select: retry");
  }
  display.display();
}

void setup() {
  Serial.begin(115200);
  prefs.begin("nikola", false);
  highScore = prefs.getInt("high", 0);
  soundEnabled = prefs.getBool("sound", true);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);
  pinMode(BTN_JOY, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  randomSeed(analogRead(0));
  showWelcomeScreen();
  melodyPlaying = false;
  noTone(BUZZER_PIN);
  currentState = MENU;
}

void loop() {
  readInputs();
  updateSound();

  if (longPressedBack() && (currentState == PLAYING || currentState == PAUSED || currentState == GAME_OVER)) {
    while (digitalRead(BTN_BACK) == LOW) { delay(10); }
    currentState = MENU;
    soundMenuBack();
    display.clearDisplay();
    display.display();
    gameOverSoundPlayed = false;
    melodyPlaying = false;
    noTone(BUZZER_PIN);
    return;
  }

  switch (currentState) {
    case MENU:
      if (longPressedSelect()) {
        soundEnabled = !soundEnabled;
        prefs.putBool("sound", soundEnabled);
        if (soundEnabled) soundMenuSelect();
        showSoundMsg = true;
        soundMsgTime = millis();
      }
      if (pressedLeft()) {
        selectedGame = (selectedGame - 1 + TOTAL_GAMES) % TOTAL_GAMES;
        soundMenuClick();
      }
      if (pressedRight()) {
        selectedGame = (selectedGame + 1) % TOTAL_GAMES;
        soundMenuClick();
      }
      if (pressedSelect()) {
        currentGame = selectedGame;
        currentState = PLAYING;
        score = 0;
        gameOverSoundPlayed = false;
        gameInit(currentGame);
        soundMenuSelect();
        soundGameStart();
      }
      drawMenu();
      break;

    case PLAYING:
      if (longPressedJoy()) {
        while (digitalRead(BTN_JOY) == LOW) { delay(10); }
        currentState = PAUSED;
        soundPause();
        break;
      }
      gameControl(currentGame);
      gameUpdate(currentGame);
      if (currentState == PLAYING) {
        display.clearDisplay();
        gameDraw(currentGame);
        display.display();
      }
      if (currentState == GAME_OVER && !gameOverSoundPlayed) {
        if (score > highScore) {
          highScore = score;
          prefs.putInt("high", highScore);
        }
        soundGameOver();
        gameOverSoundPlayed = true;
      }
      break;

    case PAUSED:
      if (pressedSelect()) {
        currentState = PLAYING;
        soundPause();
      }
      drawPaused();
      break;

    case GAME_OVER:
      if (pressedSelect()) {
        currentState = PLAYING;
        score = 0;
        gameOverSoundPlayed = false;
        melodyPlaying = false;
        noTone(BUZZER_PIN);
        gameInit(currentGame);
        soundMenuSelect();
        soundGameStart();
      }
      drawGameOver();
      break;
  }
  delay(10);
}

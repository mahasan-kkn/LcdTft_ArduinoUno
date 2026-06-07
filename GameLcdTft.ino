#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

// --- กำหนดค่าสี ---
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define CYAN    0x07FF

// --- ตั้งค่าหน้าจอ ---
MCUFRIEND_kbv tft;

// --- ตั้งค่าหน้าจอสัมผัส (มาตรฐานของบอร์ด TFT Shield) ---
#define YP A1
#define XM A2
#define YM 7
#define XP 6
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// --- ค่าสอบเทียบหน้าจอสัมผัส (ปรับได้จากผลการทดสอบก่อนหน้า) ---
#define TS_MINX 256
#define TS_MAXX 848
#define TS_MINY 195
#define TS_MAXY 858

// --- ตัวแปรของเกม ---
enum GameState { MENU, PLAYING, GAMEOVER };
GameState state = MENU;

int score = 0;
int timeLeft = 30;          // เวลาเล่น 30 วินาที
unsigned long lastTimeUpdate = 0;

int targetX = 0, targetY = 0;
int targetR = 25;           // รัศมีของเป้า
bool targetActive = false;
unsigned long targetSpawnTime = 0;

void setup() {
  Serial.begin(9600);
  
  // เริ่มต้นหน้าจอ
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9481;
  if (ID == 0x0000 || ID == 0xFFFF) ID = 0x9341;
  tft.begin(ID);
  tft.setRotation(1); // แนวนอน (320x240)
  
  showMenu();
}

void loop() {
  int touchX, touchY;
  bool isTouched = getTouch(touchX, touchY);

  if (state == MENU) {
    if (isTouched) {
      startGame();
    }
  } 
  else if (state == PLAYING) {
    // อัปเดตเวลาทุก 1 วินาที
    if (millis() - lastTimeUpdate >= 1000) {
      timeLeft--;
      lastTimeUpdate = millis();
      drawHUD(); // อัปเดตเวลาบนหน้าจอ
      
      if (timeLeft <= 0) {
        state = GAMEOVER;
        showGameOver();
      }
    }

    // สร้างเป้าใหม่ถ้ายังไม่มี
    if (!targetActive) {
      spawnTarget();
    }

    // ตรวจสอบการแตะเป้า
    if (isTouched) {
      // คำนวณระยะห่างระหว่างจุดที่แตะกับศูนย์กลางเป้า (Pythagoras)
      int dx = touchX - targetX;
      int dy = touchY - targetY;
      
      if ((dx * dx + dy * dy) <= (targetR * targetR)) {
        // แตะถูกเป้า!
        score++;
        targetActive = false; // ลบเป้าเก่า
        
        // เอฟเฟกต์ภาพเมื่อแตะถูก (กระพริบสีเขียว)
        tft.fillCircle(targetX, targetY, targetR, GREEN);
        delay(100);
        
        drawHUD(); // อัปเดตคะแนน
      }
    }
  } 
  else if (state == GAMEOVER) {
    if (isTouched) {
      showMenu();
    }
  }
  
  delay(50); // หน่วงเวลาเล็กน้อยเพื่อลดการอ่านสัมผัสซ้ำ
}

// --- ฟังก์ชันอ่านค่าสัมผัส ---
bool getTouch(int &x, int &y) {
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (p.z > ts.pressureThreshhold) {
    // สูตรนี้ต้องคงไว้ตามเดิม (สลับ p.y ใส่ mapX และสลับปลายทาง mapY)
    x = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());
    y = map(p.x, TS_MINX, TS_MAXX, tft.height(), 0);
    
    x = constrain(x, 0, tft.width());
    y = constrain(y, 0, tft.height());
    return true;
  }
  return false;
}

// --- ฟังก์ชันจัดการหน้าจอต่างๆ ---
void showMenu() {
  state = MENU;
  tft.fillScreen(BLACK);
  
  tft.setTextColor(YELLOW);
  tft.setTextSize(3);
  tft.setCursor(60, 60);
  tft.print("TAP THE");
  tft.setCursor(60, 100);
  tft.print("TARGET!");
  
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(50, 160);
  tft.print("Touch to Start");
  
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(90, 210);
  tft.print("Arduino Uno Game");
}

void startGame() {
  state = PLAYING;
  score = 0;
  timeLeft = 30;
  targetActive = false;
  lastTimeUpdate = millis();
  
  tft.fillScreen(BLACK);
  drawHUD();
}

void drawHUD() {
  // วาดแถบด้านบน
  tft.fillRect(0, 0, 320, 30, BLUE);
  
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  
  // แสดงคะแนน
  tft.setCursor(10, 8);
  tft.print("Score: ");
  tft.print(score);
  
  // แสดงเวลา
  tft.setCursor(200, 8);
  tft.print("Time: ");
  tft.print(timeLeft);
  tft.print("s ");
}

void spawnTarget() {
  // สุ่มตำแหน่ง X (ระหว่าง 40 ถึง 280) และ Y (ระหว่าง 50 ถึง 210)
  targetX = random(40, 280);
  targetY = random(50, 210);
  
  // วาดเป้า (วงกลมสีแดงมีขอบขาว)
  tft.fillCircle(targetX, targetY, targetR, RED);
  tft.drawCircle(targetX, targetY, targetR, WHITE);
  
  targetActive = true;
  targetSpawnTime = millis();
}

void showGameOver() {
  tft.fillScreen(BLACK);
  
  tft.setTextColor(RED);
  tft.setTextSize(3);
  tft.setCursor(70, 60);
  tft.print("GAME OVER");
  
  tft.setTextColor(YELLOW);
  tft.setTextSize(2);
  tft.setCursor(90, 120);
  tft.print("Final Score:");
  
  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.setCursor(110, 150);
  tft.print(score);
  
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(60, 210);
  tft.print("Touch to Retry");
}
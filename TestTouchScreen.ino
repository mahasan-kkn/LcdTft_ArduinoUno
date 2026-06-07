#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define YELLOW  0xFFE0

MCUFRIEND_kbv tft;

#define YP A1
#define XM A2
#define YM 7
#define XP 6
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// ใส่ค่าประมาณการไปก่อน เพื่อให้เป้าเล็งวิ่งตามนิ้วได้ใกล้เคียง (จะได้จิ้มได้ถูกจุด)
#define TS_MINX 150
#define TS_MAXX 920
#define TS_MINY 120
#define TS_MAXY 890

void setup() {
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9481;
  if (ID == 0x0000 || ID == 0xFFFF) ID = 0x9341;
  
  tft.begin(ID);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  
  tft.setTextColor(YELLOW);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("Calibration");
  
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.println("Open Serial Monitor (9600)");
  tft.println("Tap the center of the RED +");
  delay(2000);
}

void loop() {
  // พิกัด 4 มุมสำหรับให้ผู้ใช้จิ้ม
  int targets[4][2] = {
    {20, 20},    // 1. บนซ้าย
    {300, 20},   // 2. บนขวา
    {20, 220},   // 3. ล่างซ้าย
    {300, 220}   // 4. ล่างขวา
  };
  
  String names[4] = {"Top-Left", "Top-Right", "Bottom-Left", "Bottom-Right"};
  
  for (int i = 0; i < 4; i++) {
    tft.fillScreen(BLACK);
    
    // วาดเป้าให้จิ้ม
    tft.drawFastHLine(targets[i][0] - 10, targets[i][1], 20, RED);
    tft.drawFastVLine(targets[i][0], targets[i][1] - 10, 20, RED);
    tft.fillCircle(targets[i][0], targets[i][1], 3, GREEN);
    
    tft.setTextColor(YELLOW);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print("Step "); tft.print(i + 1); tft.print("/4");
    
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    tft.print("Tap the RED + at ");
    tft.println(names[i]);
    
    bool touched = false;
    while (!touched) {
      TSPoint p = ts.getPoint();
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      
      if (p.z > ts.pressureThreshhold) {
        touched = true;
        
        // ส่งค่า Raw ที่แท้จริงออกไปที่ Serial Monitor
        Serial.print("Corner: "); Serial.println(names[i]);
        Serial.print("  Raw p.x = "); Serial.println(p.x);
        Serial.print("  Raw p.y = "); Serial.println(p.y);
        Serial.println("-------------------------");
        
        // ยืนยันว่ารับค่าได้
        tft.fillScreen(BLACK);
        tft.setTextColor(GREEN);
        tft.setTextSize(2);
        tft.setCursor(50, 100);
        tft.print("Saved!");
        delay(1000);
      }
      delay(50);
    }
  }
  
  // จบการสอบเทียบ
  tft.fillScreen(BLACK);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("DONE!");
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.println("Check Serial Monitor");
  tft.println("Copy the 4 values to your code");
  
  while (true) { delay(1000); } // หยุดโปรแกรม
}
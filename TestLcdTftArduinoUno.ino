#include <Adafruit_GFX.h>    // ไลบรารีกราฟิกหลัก (ต้องเพิ่มบรรทัดนี้)
#include <MCUFRIEND_kbv.h>   // ไลบรารีสำหรับควบคุมจอ
// ตั้งค่าบอร์ดเป็น Arduino Uno ใช้จอ LCD TFT 2.4" แบบ Parallel

// --- กำหนดค่าสี (Color Definitions) ที่ขาดหายไป ---
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
// ---------------------------------------------------

// สร้างออบเจกต์สำหรับควบคุมหน้าจอ
MCUFRIEND_kbv tft;

void setup() {
  // เริ่มการสื่อสารแบบ Serial เพื่อใช้ดูค่า Debug
  Serial.begin(9600);
  Serial.println("Starting MCUFRIEND_kbv Test...");

  // 1. อ่านรหัส Driver ของหน้าจออัตโนมัติ
  uint16_t ID = tft.readID();
  Serial.print("Detected LCD ID: 0x");
  Serial.println(ID, HEX);

  // 2. แก้ไขกรณีอ่าน ID ผิดพลาด (พบบ่อยในจอ Clone ราคาถูก)
  if (ID == 0xD3D3) ID = 0x9481;       // แก้ไขบั๊กการอ่านที่พบบ่อย
  if (ID == 0x0000 || ID == 0xFFFF) {
    Serial.println("Warning: Could not read ID, forcing ILI9341...");
    ID = 0x9341;                        // บังคับใช้ ILI9341 หากอ่านไม่ได้
  }

  // 3. เริ่มต้นการทำงานของหน้าจอด้วย ID ที่ตรวจพบ
  tft.begin(ID);
  
  // 4. ตั้งค่าการหมุนหน้าจอ (0=แนวตั้ง, 1=แนวนอน, 2=แนวตั้งกลับหัว, 3=แนวนอนกลับหัว)
  tft.setRotation(1); 

  // 5. ลบหน้าจอให้เป็นสีดำ
  tft.fillScreen(BLACK);

  // 6. ทดสอบการแสดงผลข้อความ
  tft.setTextColor(WHITE);
  tft.setTextSize(2); // <--- แก้ไขจาก setTextSize2 เป็น setTextSize
  tft.setCursor(10, 10);
  tft.print("Arduino Uno TFT Test");
  
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.setCursor(10, 35);
  tft.print("Library: MCUFRIEND_kbv");
  tft.print(" | ID: 0x");
  tft.println(ID, HEX);

  // 7. ทดสอบการวาดรูปทรงเรขาคณิต
  delay(1000); 
  
  // วาดสี่เหลี่ยมสีแดง
  tft.fillRect(10, 60, 100, 50, RED);
  tft.setCursor(120, 75);
  tft.setTextColor(WHITE);
  tft.print("Rect (RED)");

  // วาดวงกลมสีเขียว
  tft.fillCircle(270, 85, 25, GREEN);
  tft.setCursor(10, 120); 
  tft.print("Circle (GREEN)");

  // วาดสามเหลี่ยมสีน้ำเงิน
  tft.fillTriangle(160, 135, 120, 200, 200, 200, BLUE);
  tft.setCursor(130, 210);
  tft.print("Triangle (BLUE)");

  Serial.println("Setup Complete! Screen should be working.");
}

void loop() {
  // ทดสอบการอัปเดตหน้าจอแบบ Real-time (เช่น การนับตัวเลข)
  static int counter = 0;
  
  // ล้างเฉพาะพื้นที่เล็กๆ เพื่อไม่ให้หน้าจอกระพริบทั้งจอ (ลด Flicker)
  tft.fillRect(10, 240, 150, 30, BLACK); 
  
  tft.setCursor(10, 240);
  tft.setTextColor(CYAN);
  tft.setTextSize(2);
  
  // แสดงค่าตัวนับที่เพิ่มขึ้น
  tft.print("Count: ");
  tft.print(counter);
  
  counter++;
  
  // หน่วงเวลาเล็กน้อย
  delay(500); 
}
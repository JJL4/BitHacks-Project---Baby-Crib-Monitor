//used libraries
#include <WiFiConfig.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>

const int PIN_MP3_RX = 20; 
const int PIN_MP3_TX = 21;
// Initialize someplace to store the values
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define I2C_SDA 8 //pin 6
#define I2C_SCL 9 //pin 7

uint8_t receiverAddress[] = {0x34, 0xB7, 0xDA, 0xF6, 0x3C, 0x94};
HardwareSerial mySerial1(1);
DFRobotDFPlayerMini player;

RemoteData baby_report;

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);
  mySerial1.begin(9600, SERIAL_8N1, PIN_MP3_RX, PIN_MP3_TX);
  delay(1000);

  // Put ESP32 in WiFi mode
  wifi_setup();
  peer_setup(receiverAddress);
  // Configure this ESP32 to receive baby fell/baby moving reports
  set_data_receiver(&baby_report);
  lcd.init(); // initialize the lcd
    lcd.backlight();

  if (!player.begin(mySerial1)){
      Serial.println("Connecting to DFPlayer Mini failed!");
    }
  player.volume(20);
  player.play(3);
}

void loop() {

  Serial.print(baby_report.vx);
  Serial.print(baby_report.vy);
  Serial.print(baby_report.sw);
  //high probability of baby not in crib/has fallem
  //warning
  if (baby_report.vx == 1 || baby_report.sw == 1){
    lcd.clear();
    delay(1000);
    lcd.setCursor(0,0);
    lcd.print("ALERT!!!");
    lcd.setCursor(0, 1);
    lcd.print("Baby not in crib!");
  }

  //motion detected by baby (or near)
  else if (baby_report.vy = 1){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Warning");
    lcd.setCursor(0, 1);
    lcd.print("Baby moving!");
  }

  //baby is ok 
  else if (baby_report.vx == 0 && baby_report.sw == 0 && baby_report.vy == 0){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Baby is OK!");
  }

  //play music to alarm parent
  while(baby_report.vx == 1){
    player.start();
  }
  //stop music
  if (!baby_report.vx) {
    player.stop();
  }

  delay(1000);                 // delay between each new baby report
}

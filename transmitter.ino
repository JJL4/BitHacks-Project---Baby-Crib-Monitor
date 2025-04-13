//used libraries
#include <WiFiConfig.h>
#include <Wire.h>
#include <HX711.h>

//MAC address of other ESP32
uint8_t receiverAddress[] = {0x34, 0xB7, 0xDA, 0xF6, 0x3C, 0x94};
#define calibration_factor -3525.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define LOADCELL_DOUT_PIN  3
#define LOADCELL_SCK_PIN  2
#define PIR_pin 1
#define SoundImpact 0

int value_impact = 0;
int pirstate = LOW;
int value_pir = 0;

RemoteData baby_report;
HX711 scale;

//setup
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("HX711 scale demo");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare();    //Assuming there is no weight on the scale at start up, reset the scale to 0

  pinMode(PIR_pin, INPUT);
  pinMode(SoundImpact, INPUT);

  // Put ESP32 in WiFi Mode
  wifi_setup();
  // Establish the other ESP32 as the peer
  peer_setup(receiverAddress);
}

//loop iteration
void loop() {
  //load cell
  if (scale.get_units() < 5 && scale.get_units() > -5) {
    baby_report.vx = 1;
  }
  else {
    baby_report.vx = 0;
  }

  //PIR Sensor
  value_pir = digitalRead(PIR_pin);
  if (value_pir == HIGH){
    if (pirstate == LOW){
      baby_report.vy = 1;
      pirstate = HIGH;
    }}
    else {
      if (pirstate == HIGH){
        baby_report.vy = 0;
        pirstate = LOW;
      } }

    //Sound Impact Sensor
    value_impact = digitalRead(SoundImpact);
    if (value_impact == HIGH){
      baby_report.sw = 1;
    }
    else {
      baby_report.sw = 0;
    }

    //send reports to other ESP32
    send_data(receiverAddress, baby_report);
    delay(1000);
}

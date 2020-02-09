// basic proof of concept for Probatio + Libmapper
// key features:
// - instantiates a libmapper device
// - constantly try to scan i2c bus for block
// - if block appears, create libmapper signal and update it every loop
// - if block doesn't appear after a certain nubmer of polls, remove the signal



#include "WiFi.h"
//#include <M5StickC.h>
#include "mapper.h"
#include <Wire.h>
#include "esp_wifi.h" //to disable power saving


#define rotaryEncoderAddress 0x2D
#define dualPistonAddress 0x31

const char* ssid     = "johnty";
const char* password = "whatwhat";

mapper_device dev = 0;
mapper_signal input_signal = 0;
mapper_signal output_signal_test = 0;
mapper_signal output_signal_rot = 0;
mapper_signal output_signal_piston = 0;

bool rotSig = false;
int rotTimeout = 0;

float seq_number = 0;
float received_value = 0;

int rotVals[2];
int pistVals[2];


int val_min[2] = {0, 0};
int val_max[2] = {255, 255};

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA, SCL); //tinypico uses 21/22, see board def file

  WiFi.begin(ssid, password);
  esp_wifi_set_ps(WIFI_PS_NONE); //NOTE: this MUST be included after WiFi.begin()!

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  float mymin = 0.0f;
  float mymax = 255.0f;

  dev = mapper_device_new("probatio-hub", 0, 0);
  output_signal_test = mapper_device_add_output_signal(dev, "test", 1, 'f', "V", &mymin, &mymax);
  //output_signal_rot = mapper_device_add_output_signal(dev, "rotary", 2, 'i', "V", &min, &max);
  //output_signal_piston = mapper_device_add_output_signal(dev, "pistons", 2, 'i', "V", &min, &max);
  //input_signal = mapper_device_add_input_signal(dev, "value_received", 1, 'f', 0, &min, &max, input_signal_handler, 0);

}

void loop() {

  mapper_device_poll(dev, 0);
  readBlocks(); //and update global sensor values

  //update test sig
  seq_number += 0.5;
  if (seq_number > 255)
    seq_number = 0.0;
  mapper_signal_update_float(output_signal_test, seq_number);

  //update rotary output signal, if it exists
  if (rotSig)
    mapper_signal_update(output_signal_rot, rotVals, 1, MAPPER_NOW);

  // can do the same for piston... not implemented
  //mapper_signal_update(output_signal_piston, pistVals, 1, MAPPER_NOW);
  
  delay(1); //this may or may not be necessary
}

//void input_signal_handler(mapper_signal sig, mapper_id instance, const void *value, int count, mapper_timetag_t *timetag) {
//  if (value) {
//    float *v = (float*)value;
//    for (int i = 0; i < mapper_signal_length(sig); i++) {
//      received_value = v[i];
//    }
//  }
//}

int count = 0;

void readBlocks()
{
  count++;
  if (count >= 100) count = 0;
  byte valueRE1 = 0;
  byte valueRE2 = 0;
  byte valueDB1 = 0;
  byte valueDB2 = 0;

  Wire.requestFrom(rotaryEncoderAddress, 2);
  if (Wire.available()) {
    rotTimeout = 20; //reset timeout for sensor presence
    valueRE1 = Wire.read();
    valueRE2 = Wire.read();
    //    M5.Lcd.setCursor(20, 20);
    //    M5.Lcd.print(valueRE1);
    //    M5.Lcd.setCursor(20, 40);
    //    M5.Lcd.print(valueRE2);
    if (!rotSig) { //create rotary signal
      output_signal_rot = mapper_device_add_output_signal(dev, "rotary", 2, 'i', "V", &val_min, &val_max);
      rotSig = true;
      Serial.println("adding rotary signal");
    }
  }
  else {
    rotTimeout--;
    if (rotTimeout == 0) {
      if (rotSig) { // remote rotary signal from libmapper
        mapper_device_remove_signal(dev, output_signal_rot);
        Serial.println("removing rotary signal");
        rotSig = false;
      }
    }
  }
  Wire.requestFrom(dualPistonAddress, 2);
  if (Wire.available()) {
    valueDB1 = Wire.read();
    valueDB2 = Wire.read();
    //    M5.Lcd.setCursor(20, 60);
    //    M5.Lcd.print(valueDB1);
    //    M5.Lcd.setCursor(20, 80);
    //    M5.Lcd.print(valueDB2);
  }

  if (count == 99) { //debug i2c, count to prevent spamming serial console
    Serial.print(valueRE1);
    Serial.print(" ");
    Serial.print(valueRE2);
    Serial.print(" ");
    Serial.print(valueDB1);
    Serial.print(" ");
    Serial.print(valueDB2);
    Serial.println();
  }

  //update global sensor values here
  pistVals[0] = valueDB1;
  pistVals[1] = valueDB2;
  rotVals[0] = valueRE1;
  rotVals[1] = valueRE2;
}

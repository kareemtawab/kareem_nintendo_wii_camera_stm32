// Wii Remote IR sensor  test sample code  by kako
// modified output for Wii-BlobTrack program by RobotFreak

#include <Wire.h>

int IRsensorAddress = 0x58;
byte data_buf[16];
int i;

int Ix[4];
int Iy[4];
int s;

void Write_2bytes(byte d1, byte d2)
{
  Wire.beginTransmission(IRsensorAddress);
  Wire.write(d1); Wire.write(d2);
  Wire.endTransmission();
}

HardwareTimer timer(1);

void setup()
{
  timer.setMode(TIMER_CH1, TIMER_PWM);
  timer.pause();
  timer.setPrescaleFactor(1);
  timer.setOverflow(3); // ~24Mhz
  timer.refresh();
  timer.resume();
  pinMode(PA8, PWM);  // setup the pin as PWM
  pwmWrite(PA8, 2);

  Serial.begin();
  while (!Serial)
  {
    delay(100);
  }
  
  Wire.begin();
  
  i2cscan();

  IRsensorAddress = 0x58;   // This results in 0x21 as the address to pass to TWI
  //IRsensorAddress = IRsensorAddress >> 1;   // This results in 0x21 as the address to pass to TWI
  //IRsensorAddress = IRsensorAddress;   // This results in 0x21 as the address to pass to TWI
  // IR sensor initialize
  Write_2bytes(0x30, 0x01); delay(10);
  Write_2bytes(0x30, 0x08); delay(10);
  Write_2bytes(0x06, 0x90); delay(10);
  Write_2bytes(0x08, 0xC0); delay(10);
  Write_2bytes(0x1A, 0x40); delay(10);
  Write_2bytes(0x33, 0x33); delay(10);
  delay(100);
}


void loop()
{
  //IR sensor read
  Wire.beginTransmission(IRsensorAddress);
  Wire.write(0x36);
  Wire.endTransmission();

  Wire.requestFrom(IRsensorAddress, 16);        // Request the 2 byte heading (MSB comes first)
  for (i = 0; i < 16; i++) {
    data_buf[i] = 0;
  }
  i = 0;
  while (Wire.available() && i < 16) {
    data_buf[i] = Wire.read();
    //Serial.println(i);
    i++;
  }

  Ix[0] = data_buf[1];
  Iy[0] = data_buf[2];
  s   = data_buf[3];
  Ix[0] += (s & 0x30) << 4;
  Iy[0] += (s & 0xC0) << 2;

  Ix[1] = data_buf[4];
  Iy[1] = data_buf[5];
  s   = data_buf[6];
  Ix[1] += (s & 0x30) << 4;
  Iy[1] += (s & 0xC0) << 2;

  Ix[2] = data_buf[7];
  Iy[2] = data_buf[8];
  s   = data_buf[9];
  Ix[2] += (s & 0x30) << 4;
  Iy[2] += (s & 0xC0) << 2;

  Ix[3] = data_buf[10];
  Iy[3] = data_buf[11];
  s   = data_buf[12];
  Ix[3] += (s & 0x30) << 4;
  Iy[3] += (s & 0xC0) << 2;

  for (i = 0; i < 4; i++)
  {
    if (Ix[i] < 1000)
      Serial.print(" ");
    if (Ix[i] < 100)
      Serial.print(" ");
    if (Ix[i] < 10)
      Serial.print(" ");
    Serial.print( int(Ix[i]) );
    Serial.print(",");
    if (Iy[i] < 1000)
      Serial.print(" ");
    if (Iy[i] < 100)
      Serial.print(" ");
    if (Iy[i] < 10)
      Serial.print(" ");
    Serial.print( int(Iy[i]) );
    if (i < 3)
      Serial.print(",");
  }
  Serial.println("");
  delay(3);
}

void i2cscan() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.

    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);

      nDevices++;
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("Starting readouts in 5 seconds!");

  delay(5000); // wait 5 seconds for next scan
}

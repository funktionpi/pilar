#include "config.h"
#include "task.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <Wire.h>
#include <helper_3dmath.h>

#define OUTPUT_TEAPOT_OSC
// #define OUTPUT_READABLE_YAWPITCHROLL

#ifdef OUTPUT_TEAPOT_OSC
#include <IPAddress.h>
#include <OSCMessage.h>
#include <WiFiUdp.h>
const IPAddress outIp(10, 0, 0, 99);  // remote IP to receive OSC
const unsigned int outPort = 9999;    // remote port to receive OSC
WiFiUDP Udp;
#endif

#ifdef OUTPUT_READABLE_EULER
float euler[3];  // [psi, theta, phi]    Euler angle container
#endif
#ifdef OUTPUT_READABLE_YAWPITCHROLL
float ypr[3];  // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
#endif

void sensor_loop();
bool sensor_start();
void sensor_end();
void mpu_process_packet();

MPU6050 mpu;

/* =========================================================================
   NOTE: In addition to connection 5/3.3v, GND, SDA, and SCL, this sketch
   depends on the MPU-6050's INT pin being connected to the ESP8266 GPIO15
   pin.
 * ========================================================================= */

// MPU control/status vars
bool dmpReady = false;   // set true if DMP init was successful
uint8_t mpuIntStatus;    // holds actual interrupt status byte from MPU
uint8_t devStatus;       // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;     // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;      // count of all bytes currently in FIFO
uint8_t fifoBuffer[64];  // FIFO storage buffer

// orientation/motion vars
Quaternion q;         // [w, x, y, z]         quaternion container
VectorInt16 aa;       // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;   // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;  // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;  // [x, y, z]            gravity vector

Task tSensor(TASK_SECOND / 120, TASK_FOREVER, &sensor_loop, nullptr, false, &sensor_start, &sensor_end);

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================
volatile bool mpuInterrupt = false;  // indicates whether MPU interrupt pin has gone high
void ICACHE_RAM_ATTR dmpDataReady() {
  mpuInterrupt = true;
}

void sensor_setup() {
#if ENABLE_SENSOR
  Wire.begin(/*I2C_DATA_PIN, I2C_CLK_PIN*/);
  Wire.setClock(400000);  // 400kHz I2C clock. Comment this line if having compilation difficulties

  Serial.println("[I2C] I2C scanner. Scanning ...");
  byte count = 0;

  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      Serial.print("[I2C] Found address: ");
      Serial.print(i, DEC);
      Serial.print(" (0x");
      Serial.print(i, HEX);
      Serial.println(")");
      count++;
    }
  }
  Serial.print("[I2C] Found ");
  Serial.print(count, DEC);
  Serial.println(" device(s).");

  // initialize device
  Serial.println("[SENSOR] Initializing MPU6050 device...");
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);

  // verify connection
  Serial.println("[SENSOR] Testing device connections...");
  Serial.print("[SENSOR] ");
  Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  Serial.println(F("[SENSOR] Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(51);
  mpu.setYGyroOffset(8);
  mpu.setZGyroOffset(21);
  mpu.setXAccelOffset(1150);
  mpu.setYAccelOffset(-50);
  mpu.setZAccelOffset(1060);

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // Calibration Time: generate offsets and calibrate our MPU6050
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    Serial.println();
    mpu.PrintActiveOffsets();
    // turn on the DMP, now that it's ready
    Serial.println(F("[SENSOR] Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.print(F("[SENSOR] Enabling interrupt detection (Arduino external interrupt "));
    Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
    Serial.println(F(")..."));
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("[SENSOR] DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print(F("[SENSOR] DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }

  ts.addTask(tSensor);
  tSensor.enable();
#endif
}

bool sensor_start() {
  return true;
}

void sensor_end() {
}

void sensor_loop() {
  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available
  if (!mpuInterrupt && fifoCount < packetSize) return;

  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();
  // Serial.print(F("[SENSOR] FIFO size: "));
  // Serial.print(fifoCount);
  // Serial.println();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    Serial.println(F("[SENSOR] FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & 0x02) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // process packet until fifo is relatively empty
    while (fifoCount > packetSize) {
      mpu_process_packet();
    }
  }
}

void mpu_process_packet() {
  // read a packet from FIFO
  mpu.getFIFOBytes(fifoBuffer, packetSize);

  // track FIFO count here in case there is > 1 packet available
  // (this lets us immediately read more without waiting for an interrupt)
  fifoCount -= packetSize;

#ifdef OUTPUT_READABLE_QUATERNION
  // display quaternion values in easy matrix form: w x y z
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  Serial.print("quat\t");
  Serial.print(q.w);
  Serial.print("\t");
  Serial.print(q.x);
  Serial.print("\t");
  Serial.print(q.y);
  Serial.print("\t");
  Serial.println(q.z);
#endif

#ifdef OUTPUT_TEAPOT_OSC
#ifndef OUTPUT_READABLE_QUATERNION
  // display quaternion values in easy matrix form: w x y z
  mpu.dmpGetQuaternion(&q, fifoBuffer);
#endif
  // Send OSC message
  OSCMessage msg("/imuquat");
  msg.add((float)q.w);
  msg.add((float)q.x);
  msg.add((float)q.y);
  msg.add((float)q.z);

  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();

  msg.empty();
#endif

#ifdef OUTPUT_READABLE_EULER
  // display Euler angles in degrees
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetEuler(euler, &q);
  Serial.print("euler\t");
  Serial.print(euler[0] * 180 / M_PI);
  Serial.print("\t");
  Serial.print(euler[1] * 180 / M_PI);
  Serial.print("\t");
  Serial.println(euler[2] * 180 / M_PI);
#endif

#ifdef OUTPUT_READABLE_YAWPITCHROLL
  // display Euler angles in degrees
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
  Serial.print("ypr\t");
  Serial.print(ypr[0] * 180 / M_PI);
  Serial.print("\t");
  Serial.print(ypr[1] * 180 / M_PI);
  Serial.print("\t");
  Serial.println(ypr[2] * 180 / M_PI);
#endif

#ifdef OUTPUT_READABLE_REALACCEL
  // display real acceleration, adjusted to remove gravity
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetAccel(&aa, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
  Serial.print("areal\t");
  Serial.print(aaReal.x);
  Serial.print("\t");
  Serial.print(aaReal.y);
  Serial.print("\t");
  Serial.println(aaReal.z);
#endif

#ifdef OUTPUT_READABLE_WORLDACCEL
  // display initial world-frame acceleration, adjusted to remove gravity
  // and rotated based on known orientation from quaternion
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetAccel(&aa, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
  mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
  Serial.print("aworld\t");
  Serial.print(aaWorld.x);
  Serial.print("\t");
  Serial.print(aaWorld.y);
  Serial.print("\t");
  Serial.println(aaWorld.z);
#endif
}

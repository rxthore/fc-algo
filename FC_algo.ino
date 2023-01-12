#include <Wire.h> // Include the Wire library
#include <I2Cdev.h> // Include the I2Cdev library
#include <MPU6050.h> // Include the MPU6050 library
#define DT 0.01
#define GYRO_SENSITIVITY 131
#define ACC_SENSITIVITY 16384 
#define GRAVITATIONAL_ACCELERATION 9.81
#define TIME_INTERVAL 0.1
#define PI 3.14159265

MPU6050 mpu; // Create a MPU6050 object

float gyro_range = 100;

// raw sensor data
int16_t ax, ay, az; // Variables to store accelerometer data
int16_t gx, gy, gz; // Variables to store gyroscope data

// variables to store calibration data
float AcX_cal, AcY_cal, AcZ_cal;
float GyX_cal, GyY_cal, GyZ_cal; 

// offset values
float AcX_offset, AcY_offset, AcZ_offset;
float GyX_offset, GyY_offset, GyZ_offset;

// LowPass filter variables
float gx_filtered, gy_filtered, gz_filtered = 0.0;
float ax_filtered, ay_filtered, az_filtered = 0.0;

// number of samples to use for calibration
int num_samples = 2000;

float roll = 0.0;
float pitch = 0.0;
float yaw = 0.0;

float roll_deg, pitch_deg, yaw_deg = 0.0;

void setup() {
  // for printing on serial monitor
  Serial.begin(9600); // Initialize serial communication at 9600 baud
  while (!Serial); // Wait for serial port to be available

  Wire.begin(); // Initialize the Wire library
  mpu.initialize(); // Initialize the MPU6050

  // Check if the MPU6050 is connected
  if (!mpu.testConnection()) {
    Serial.println("Error: MPU6050 not connected");
    while (1); // Stop the program if the MPU6050 is not connected
  }

  // sum of samples
  int16_t AcX_sum, AcY_sum, AcZ_sum = 0;
  int16_t GyX_sum, GyY_sum, GyZ_sum = 0;

  for(int i = 0; i < num_samples; i++) {
    // reading raw sample data
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // adding to accelerometer sums
    AcX_sum += ax;
    AcY_sum += ay;
    AcZ_sum += az;

    // adding to gyro sums
    GyX_sum += gx;
    GyY_sum += gy;
    GyZ_sum += gz;
  }
  
  AcX_offset = AcX_sum / num_samples;
  AcY_offset = AcY_sum / num_samples;
  AcZ_offset = AcZ_sum / num_samples;
  
  GyX_offset = GyX_sum / num_samples;
  GyY_offset = GyY_sum / num_samples;
  GyZ_offset = GyZ_sum / num_samples;

  Serial.print("Gyro offsets : ");
  Serial.print(GyX_offset);
  Serial.print(" ");
  Serial.print(GyY_offset);
  Serial.print(" ");
  Serial.println(GyZ_offset);

  Serial.print("Accelerometer offsets : ");
  Serial.print(AcX_offset);
  Serial.print(" ");
  Serial.print(AcY_offset);
  Serial.print(" ");
  Serial.println(AcZ_offset);
  
  // indicate on pin 13 that calibration is done
  pinMode(13, OUTPUT);

  for(int i = 0; i < 10; i++) {
    digitalWrite(13, HIGH);
    delay(250);
    digitalWrite(13, LOW);
    delay(250);
  }

  Serial.println("Roll:, Pitch:, Yaw:");
}

void loop() {
  // Read the accelerometer and gyroscope data
  imu_read();
}

int imu_read() {
  digitalWrite(13, HIGH);
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // calculate calibrated readings
  // convert accelerometer readings to g's
  AcX_cal = ((ax - AcX_offset) / 16384) * GRAVITATIONAL_ACCELERATION;
  AcY_cal = ((ay - AcY_offset) / 16384) * GRAVITATIONAL_ACCELERATION;
  AcZ_cal = ((az - AcZ_offset) / 16384) * GRAVITATIONAL_ACCELERATION;
  // convert gyro readings to radians per second
  GyX_cal = ((gx - GyX_offset) / GYRO_SENSITIVITY) * (PI / 180);
  GyY_cal = ((gy - GyY_offset) / GYRO_SENSITIVITY) * (PI / 180);
  GyZ_cal = ((gz - GyZ_offset) / GYRO_SENSITIVITY) * (PI / 180);

  // calculate roll, pitch and yaw using accelerometer
  float roll_acc = atan2(AcY_cal, AcZ_cal);
  float pitch_acc = atan2(-AcX_cal, sqrt(AcY_cal * AcY_cal + AcZ_cal * AcZ_cal));
  float yaw_acc = atan2(AcX_cal, sqrt(AcY_cal * AcY_cal + AcZ_cal * AcZ_cal));

  // calculate angular velocity
  float gyro_roll = GyX_cal * TIME_INTERVAL;
  float gyro_pitch = GyY_cal * TIME_INTERVAL;
  float gyro_yaw = GyZ_cal * TIME_INTERVAL;

  applyComplementaryFilter(roll_acc, pitch_acc, yaw_acc, gyro_roll, gyro_pitch, gyro_yaw);  

  // Serial.print(gyro_roll);
  // Serial.print(",");
  // Serial.print(gyro_pitch);
  // Serial.print(",");
  // Serial.println(gyro_yaw);

  // Serial.print(roll_acc);
  // Serial.print(",");
  // Serial.print(pitch_acc);
  // Serial.print(",");
  // Serial.println(yaw_acc);
  
  Serial.print(roll);
  Serial.print(",");
  Serial.print(pitch);
  Serial.print(",");
  Serial.println(yaw);

  delay(100);
}

void applyLowPassFilter() {
  float alpha = 0.98;
  // apply lowpass on gyro
  // gx_filtered = alpha * gx_filtered + (1.0 - alpha) * GyX_cal;
  // gy_filtered = alpha * gy_filtered + (1.0 - alpha) * GyY_cal;
  // gz_filtered = alpha * gz_filtered + (1.0 - alpha) * GyZ_cal;
  // // apply lowpass on accelerometer
  // ax_filtered = alpha * ax_filtered + (1.0 - alpha) * AcX_cal;
  // ay_filtered = alpha * ay_filtered + (1.0 - alpha) * AcY_cal;
  // az_filtered = alpha * az_filtered + (1.0 - alpha) * AcZ_cal;
    
}

void applyComplementaryFilter(float roll_acc, float pitch_acc, float yaw_acc, float gyro_roll, float gyro_pitch, float gyro_yaw) {
  float ALPHA = 0.8;

  roll = ALPHA * (roll + gyro_roll) + (1.0 - ALPHA) * roll_acc;
  pitch = ALPHA * (pitch + gyro_pitch) + (1.0 - ALPHA) * pitch_acc;
  yaw = ALPHA * (yaw + gyro_yaw) + (1.0 - ALPHA) * yaw_acc;

  // radToDeg();
}

void radToDeg() {
  roll = roll * 180 / PI;
  pitch = pitch * 180 / PI;
  yaw = yaw * 180 / PI;
}

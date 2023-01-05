#include <Wire.h> // Include the Wire library
#include <I2Cdev.h> // Include the I2Cdev library
#include <MPU6050.h> // Include the MPU6050 library

MPU6050 mpu; // Create a MPU6050 object

// raw sensor data
int16_t ax, ay, az; // Variables to store accelerometer data
int16_t gx, gy, gz; // Variables to store gyroscope data

// variables to store calibration data
float AcX_cal, AcY_cal, AcZ_cal;
float GyX_cal, GyY_cal, GyZ_cal; 

// offset values
float AcX_offset, AcY_offset, AcZ_offset;
float GyX_offset, GyY_offset, GyZ_offset;

// number of samples to use for calibration
int num_samples = 1000;

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
  Serial.print(GyZ_offset);

  Serial.print("Accelerometer offsets : ");
  Serial.print(AcX_offset);
  Serial.print(" ");
  Serial.print(AcY_offset);
  Serial.print(" ");
  Serial.print(AcZ_offset);
  
  // indicate on pin 13 that calibration is done
  pinMode(13, OUTPUT);

  for(int i = 0; i < 10; i++) {
    digitalWrite(13, HIGH);
    delay(250);
    digitalWrite(13, LOW);
    delay(250);
  }
}

void loop() {
  // Read the accelerometer and gyroscope data
  imu_read();
}

int imu_read() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  delay(500); // Delay for 250 milliseconds
}

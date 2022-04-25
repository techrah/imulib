# imulib

Library for InvenSense MPU-65xx/9250/9255 Inertial Measurement Unit ICs.

NOTE: This is a work in progress.

## What

C++ utility classes for MPU-65xx/9250/9255 on Arduino and Raspberry Pi.

Details of SPI and I2C communications protocols along with their platform-specific nuances have been abstracted away in the respective `SPI` and `I2C` classes.

## Usage

### Magnotometer Trivial Example (Arduino)

```cpp
#include <TR_I2C.hpp>
#include <TR_AK8963.hpp>
#include <TR_linalg.hpp>

// Manual configuration
AK8963::Config magConfig;

void setup() {
    // You should retrieve these from persistent storage like EEPROM.
    magConfig.asa = {175, 175, 164};
    magConfig.offset = {0, 0, 0};

    // Transform axis readings, if desired,
    // for example, to align axes with accelerometer and gyroscope
    magConfig.transform = {1, 0, 0,
                           0, 1, 0,
                           0, 0, 1};


    // Access the magnetometer through the dedicated I2C pins of an MPU-92xx. Also good for stand-alone AK8963 chips.
    I2C *i2c = new I2C(0x0C);

    AK8963 mag = new AK8963(i2c);
    mag->setConfig(magConfig);

    mag->startup(AK8963::CNTL1FlagsMode::MODE_CONTINUOUS_MEASUREMENT_1_8HZ);
    BLA::Vector<3> values = mag->getSensorValues();
    mag->shutdown();
}

void loop() {}

```

### MPU-9250 Example pseudo-code

```cpp
I2C *i2c_mpu = new I2C(0x68); // You can also use SPI here
I2C *i2c_mag = new I2C(0x0C);

// No need to use I2C pins for magnotometer. The internal I2C connection will be used automatically!
MPU9250 *mpu = new MPU9250(i2c_mpu, i2c_mag);

// Set mag config as before
AK8963::Config magConfig;
magConfig.asa = {175, 175, 164};
magConfig.offset = {0, 0, 0};
magConfig.transform = {1, 0, 0, 0, 1, 0, 0, 0, 1};
mpu->setMagConfig(magConfig);

mpu->startup();

// Get accelerometer values
Values<float> values = mpu->getAccelSensorValues();

// Get access to magnetometer
AK8963 *mag = mpu->getMag();

mpu->shutdown();
```

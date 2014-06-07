/*
	calibrate.cpp

	Calibrates the sensors through Drive::calibrate(), saving the values to
	calibration.ini.

	NOTE: the calibration values are probably dependent on the range specified
	for the sensors. Change the program to match the ranges that you are using
	for each sensor.

	Any program that wishes to use the outputted calibration should run with
	this file alongside it.
*/

#include <string>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include <unistd.h>

#include "radio.h"
#include "radiouart.h"
#include "radioconnection.h"
#include "packetdiagnostic.h"
#include "geometry.h"
#include "accelerometer.h"
#include "gyroscope.h"
#include "pwm.h"
#include "drive.h"

int main(int argc, char **argv) {
	try {
		I2C i2c("/dev/i2c-1");
		PWM pwm(&i2c, 0x40);
		Accelerometer accel(&i2c, 0x53, Accelerometer::RANGE_2G,
				Accelerometer::SRATE_100HZ);
		Gyroscope gyro(&i2c, 0x69, Gyroscope::RANGE_250DPS,
				Gyroscope::SRATE_100HZ);

		Drive drive(&pwm, &accel, &gyro, 15, 15, 15, 15, 100);
		
		std::cout << "Calibrating for 2 seconds. Keep sensors still!"
				<< std::endl
				<< "  Accelerometer : Range=2G" << std::endl
				<< "      Gyroscope : Range=250dps" << std::endl
				<< "..." << std::endl;
		drive.calibrate(2000);

	} catch (I2CException &e) {
		std::cerr << "I2C EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	} catch (RadioException &e) {
		std::cerr << "RADIO EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	} catch (Exception &e) {
		std::cerr << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}

	std::cout << "Done!" << std::endl;
	return 0;
}


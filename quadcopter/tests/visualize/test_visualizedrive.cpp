/*
	test_visualizedrive.cpp

	Test that sends the perceived orientation (according to Drive class) over
	the radio to be visualized by the remote (using remote test_visualize).	
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

		RadioUART radio(57600, Radio::PARITY_EVEN);
		RadioConnection connection(&radio);

//		std::cout << "Waiting for connection..." << std::endl;
//		connection.connect();
//		std::cout << "Connected!" << std::endl;

		Drive drive(&pwm, &accel, &gyro, 15, 15, 15, 15, 5);

		std::string input;
		bool running = true;
		while (running) {
			values = accel.read();

			Vector2<float> xz(values.x, values.z);
			Vector2<float> yz(values.y, values.z);
			float y_angle = asin(values.x / magnitude(xz)) * 180.0 / PI;
			float x_angle = asin(values.y / magnitude(yz)) * 180.0 / PI;

			printf("Sending Roll : %+08f | Pitch : %+08f | Yaw : %+08f\n",
					values.x, values.y, values.z);
			PacketDiagnostic pkt(0, x_angle, y_angle, 0.0f);
			connection.send(&pkt);

			usleep(10000); // 100 Hz
		}

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


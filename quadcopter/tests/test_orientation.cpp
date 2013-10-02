/*
	test_orientation.cpp

	Test for Accelerometer, and determining orientation from readings
*/

#include <iostream>
#include <math.h>
#include <unistd.h>

#include "i2c.h"
#include "accelerometer.h"
#include "geometry.h"

int main(int argc, char **argv) {
	try {

		I2C i2c("/dev/i2c-1");
		Accelerometer accel(&i2c, 0x53, Accelerometer::RANGE_2G,
				Accelerometer::SRATE_50HZ);

		Vector3<float> ref = accel.read();

		Vector2<float> xz(ref.x, ref.z);
		Vector2<float> yz(ref.y, ref.z);

		double ref_y_angle = (180.0 * asin(ref.x / magnitude(xz))) / PI;
		double ref_x_angle = (180.0 * asin(ref.y / magnitude(yz))) / PI;

		std::cout << "Reference:" << std::endl
				<< "  x = " << ref.x << std::endl
				<< "  y = " << ref.y << std::endl
				<< "  z = " << ref.z << std::endl
				<< "  roll  = " << ref_y_angle << " deg" << std::endl
				<< "  pitch = " << ref_x_angle << " deg" << std::endl;

		while (true) {
			Vector3<float> curr = accel.read();

			xz.x = curr.x;
			xz.y = curr.z;
			yz.x = curr.y;
			yz.y = curr.z;

			double y_angle = (180.0 * asin(curr.x / magnitude(xz))) / PI;
//					- ref_y_angle;
			double x_angle = (180.0 * asin(curr.y / magnitude(yz))) / PI;
//					- ref_x_angle;

			std::cout << "Current:  roll = " << y_angle << " deg | "
					<< " pitch = " << x_angle << " deg" << std::endl;

			usleep(50000);
		}

	} catch (Exception &e) {
		std::cout << "EXCEPTION: " << e.getDescription() << std::endl;
		return -1;
	}

	std::cout << "Done!" << std::endl;
	return 0;
}


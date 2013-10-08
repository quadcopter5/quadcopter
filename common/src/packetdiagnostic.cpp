/*
	packetdiagnostic.h

	PacketDiagnostic class - packet type that holds diagnostic information.
		[Outgoing] This type is sent from quadcopter to remote

	Fields:
		battery : uint8_t, battery charge. 255 = full charge; 0 = none
		accel_x : 32-bit float, accelerometer value in x-axis
		accel_y : 32-bit float, accelerometer value in y-axis
		accel_z : 32-bit float, accelerometer value in z-axis

	See packet.h for a description of the virtual member functions.
*/

#include <string>
#include <stdint.h>

#include "endianness.h"
#include "packet.h"
#include "packetdiagnostic.h"

PacketDiagnostic::PacketDiagnostic() {
	mBattery = 0;
	for (int i = 0; i < 3; ++i)
		mAccel[i] = 0.0f;
	mCurrentField = 0;
}

PacketDiagnostic::PacketDiagnostic(uint8_t battery, float accel_x,
		float accel_y, float accel_z) {
	setBattery(battery);
	setAccelX(accel_x);
	setAccelY(accel_y);
	setAccelZ(accel_z);

	mCurrentField = 0;
}

char PacketDiagnostic::getHeader() const {
	return PKT_DIAGNOSTIC;
}

bool PacketDiagnostic::feedData(std::string &buffer) {
	if (buffer.size() > 0) {
		// Restart from beginning if currently complete
		if (mCurrentField == 4)
			mCurrentField = 0;

		while (mCurrentField < 4 && buffer.size() > 0) {
			if (mCurrentField == 0) {
				mBattery = buffer[0];
				buffer.erase(0, 1);
				++mCurrentField;
			} else if (buffer.size() >= 4) {
				// Read 32-bit float only if there are 4 bytes available
				LEToHost(&mAccel[mCurrentField - 1], (float *)&buffer[0], 4);
				buffer.erase(0, 4);
				++mCurrentField;
			}
		}

		if (mCurrentField == 4)
			return true;
	}
	return false;
}

bool PacketDiagnostic::getComplete() const {
	return (mCurrentField == 4);
}

std::string PacketDiagnostic::serialize() const {
	std::string result;
	result.push_back((char)mBattery);
	for (int i = 0; i < 3; ++i) {
		float swapped;
		hostToLE(&swapped, &mAccel[i], 4);
		result.append((char *)&swapped, 4);
	}
	return result;
}

uint8_t PacketDiagnostic::getBattery() const {
	return mBattery;
}

float PacketDiagnostic::getAccelX() const {
	return mAccel[0];
}

float PacketDiagnostic::getAccelY() const {
	return mAccel[1];
}

float PacketDiagnostic::getAccelZ() const {
	return mAccel[2];
}

void PacketDiagnostic::setBattery(uint8_t battery) {
	mBattery = battery;
}

void PacketDiagnostic::setAccelX(float accel_x) {
	mAccel[0] = accel_x;
}

void PacketDiagnostic::setAccelY(float accel_y) {
	mAccel[1] = accel_y;
}

void PacketDiagnostic::setAccelZ(float accel_z) {
	mAccel[2] = accel_z;
}


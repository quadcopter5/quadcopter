/*
	packetmotion.cpp

	PacketMotion class - packet type that holds motion information.
*/

#include <string>
#include <stdint.h>
#include <string.h>

#include "packet.h"
#include "packetmotion.h"

PacketMotion::PacketMotion() {
	// Initialize fields to 0
	memset(mFields, 0, 4);

	mCurrentField = 0;
}

PacketMotion::PacketMotion(int8_t x, int8_t y, int8_t z, int8_t rot) {
	setX(x);
	setY(y);
	setZ(z);
	setRot(rot);

	mCurrentField = 0;
}

char PacketMotion::getHeader() const {
	return PKT_MOTION;
}

bool PacketMotion::feedData(std::string &buffer) {
	if (buffer.length() > 0) {
		// Reset if already complete
		if (mCurrentField == 4)
			mCurrentField = 0;

		while (mCurrentField < 4 && buffer.length() > 0) {
			mFields[mCurrentField] = (int8_t)buffer.at(0);
			buffer.erase(0, 1); // Erase first byte
			++mCurrentField;
		}

		if (mCurrentField == 4)
			return true;
	}

	return false;
}

bool PacketMotion::getComplete() const {
	return (mCurrentField == 4);
}

std::string PacketMotion::serialize() const {
	std::string result;
	for (int i = 0; i < 4; ++i)
		result.push_back((char)mFields[i]);
	return result;
}

int8_t PacketMotion::getX() const {
	return mFields[0];
}

int8_t PacketMotion::getY() const {
	return mFields[1];
}

int8_t PacketMotion::getZ() const {
	return mFields[2];
}

int8_t PacketMotion::getRot() const {
	return mFields[3];
}

void PacketMotion::setX(int8_t x) {
	mFields[0] = x;
}

void PacketMotion::setY(int8_t y) {
	mFields[1] = y;
}

void PacketMotion::setZ(int8_t z) {
	mFields[2] = z;
}

void PacketMotion::setRot(int8_t rot) {
	mFields[3] = rot;
}


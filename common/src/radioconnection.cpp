/*
	radioconnection.cpp

	RadioConnection class - represents a connection over wireless radio.
*/

#include <string>
#include <unistd.h>

#include "exception.h"
#include "radio.h"
#include "packet.h"
#include "radioconnection.h"

#include "packetmotion.h"

RadioConnection::RadioConnection(Radio *radio) {
	if (!radio)
		THROW_EXCEPT(RadioException,
				"Invalid Radio passed to RadioConnection");

	mRadio = radio;
	mCurrentPacket = 0;
}

RadioConnection::~RadioConnection() {
	delete mCurrentPacket;
}

void RadioConnection::connect() {
	std::string response;
	mRadio->write("Hi");

	size_t index;
	while ((index = response.find("Hi")) == std::string::npos) {
		mRadio->read(response, 0);
		usleep(500000);
	}

	// Keep the data after the acknowledge
	mUnhandledData.assign(response, index + 2, std::string::npos);

	mRadio->write("Hi");
}

Packet *RadioConnection::receive() {
	std::string data;
	mRadio->read(data, 0); // Receive all available data
	mUnhandledData.append(data);

	if (mCurrentPacket) {
		if (mCurrentPacket->feedData(mUnhandledData)) {
			// Packet is complete; return it and set current packet
			// to null to start a new one
			Packet *result = mCurrentPacket;
			mCurrentPacket = 0;
			return result;
		} else
			return 0;
	} else {
		// Look for the start of a packet
		size_t startindex = 0;
		bool found = false;
		while (!found && mUnhandledData.length() >= 3) {

			startindex = mUnhandledData.find(0x2A);
			if (startindex != std::string::npos
					&& startindex <= mUnhandledData.length() - 3) {

				// We have enough data to determine if this is actually
				// the start of a packet
				if ((unsigned char)mUnhandledData.at(startindex + 1) == 0xA2) {
					// Determine packet type
					found = true;
					switch (mUnhandledData.at(startindex + 2)) {
						case PKT_MOTION:
							mCurrentPacket = new PacketMotion();
							break;

						default:
							found = false;
							break;
					}

					if (found) {
						// Erase packet start and header
						mUnhandledData.erase(0, startindex + 3);

						// Try to start reading into Packet
						if (mCurrentPacket->feedData(mUnhandledData)) {
							// Packet is complete
							Packet *result = mCurrentPacket;
							mCurrentPacket = 0;
							return result;
						}
					} else {
						// Erase until where header was supposed to be
						mUnhandledData.erase(0, startindex + 2);
					}

				} else {
					// This 0x2A is a false positive
					// Erase up to/including 0x2A
					mUnhandledData.erase(0, startindex + 1);
				}
			} else if (startindex != std::string::npos) {
				// No 0x2A's were found
				// Discard this data
				mUnhandledData.clear();
			} else {
				// Not enough information after 0x2A
				// Wait until next time when we have more data
				mUnhandledData.erase(0, startindex);
			}
		}
	}

	// If we got this far, Packet is not complete
	return 0;
}

void RadioConnection::send(const Packet *p) {
	std::string message;

	// Prepend with packet start and packet header
	message.push_back((unsigned char)0x2A);
	message.push_back((unsigned char)0xA2);
	message.push_back(p->getHeader());

	message.append(p->serialize());
	mRadio->write(message);
}


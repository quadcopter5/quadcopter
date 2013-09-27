/*
	radioconnection.cpp

	RadioConnection class - represents a connection over wireless radio.
*/

#include <string>

#include "exception.h"
#include "radio.h"
#include "packet.h"
#include "radioconnection.h"

#include "packetmotion.h"

RadioConnection::RadioConnection(Radio *radio) {
	if (!radio)
		THROW_EXCEPT(RadioException,
				"Invalid radio passed to RadioConnection");

	mRadio = radio;
	mCurrentPacket = 0;
}

RadioConnection::~RadioConnection() {
	delete mCurrentPacket;
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
		size_t index = 0;
		bool found = false;
		while (!found && index != std::string::npos) {

			index = mUnhandledData.find(0x2A);
			if (index != std::string::npos) {

				if (mUnhandledData.length() >= 3 &&
						index <= mUnhandledData.length() - 3) {
					++index;
					// We have enough data to determine if this is actually
					// the start of a packet
					if ((unsigned char)mUnhandledData.at(index) == 0xA2) {
						// Determine packet type
						++index;
						found = true;
						switch (mUnhandledData.at(index)) {
							case PKT_MOTION:
								mCurrentPacket = new PacketMotion();
								break;

							default:
								found = false;
								break;
						}

						if (found) {
							// Erase packet start and header
							mUnhandledData.erase(0, index + 1);

							// Try to start reading into Packet
							if (mCurrentPacket->feedData(mUnhandledData)) {
								// Packet is complete
								Packet *result = mCurrentPacket;
								mCurrentPacket = 0;
								return result;
							}
						}
					}
				}

				if (!found) {
					// Erase used data
					mUnhandledData.erase(0, index + 1);
				}

			} else {
				// No 0x2A's were found
				// Discard this data
				mUnhandledData.clear();
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


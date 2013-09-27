/*
	radioconnection.h

	RadioConnection class - represents a connection over wireless radio.

	Uses Packets to send and retrieve data.
*/

#ifndef RADIOCONNECTION_H
#define RADIOCONNECTION_H

#include "exception.h"
#include "radio.h"
#include "packet.h"

class RadioConnection {
	public:
		/**
			Constructor

			Starts a connection using the provided Radio object.

			Throws RadioException if radio is not valid.
		*/
		RadioConnection(Radio *radio);

		/**
			Destructor
		*/
		~RadioConnection();

		/**
			Reads in the latest data received from the radio.

			Returns a Packet allocated with new if a complete packet has been
			received. Otherwise, null is returned. It is possible that a
			packet is in the middle of being received.

			The user of this function is responsible for delete'ing the
			returned pointer!

			Throws RadioException
		*/
		Packet *receive();

		/**
			Sends the given packet over the radio.

			Throws RadioException
		*/
		void send(const Packet *p);

	private:
		Radio       *mRadio;
		Packet      *mCurrentPacket;
		std::string mUnhandledData;
};

#endif


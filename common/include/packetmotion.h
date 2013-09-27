/*
	packetmotion.h

	PacketMotion class - packet type that holds motion information.

	Fields:
		x   : uint8_t, velocity in (-)left, right(+) axis
		y   : uint8_t, velocity in (-)backward, forward(+) axis
		z   : uint8_t, velocity in (-)down, up(+) axis
		rot : uint8_t, rotational velocity in (-)CCW, CW(+) axis

	See packet.h for a description of the virtual member functions.
*/

#ifndef PACKETMOTION_H
#define PACKETMOTION_H

#include <string>
#include <stdint.h>

#include "packet.h"

#define PKT_MOTION (char)0b10100000

class PacketMotion : public Packet {
	public:
		/**
			Constructor

			Initializes fields to 0
		*/
		PacketMotion();

		/**
			Gets the header of this specific Packet. In this case, this will
			return PKT_MOTION
		*/
		virtual char getHeader() const;

		virtual bool feedData(std::string &buffer);

		virtual bool getComplete() const;

		virtual std::string serialize() const;

		/**
			Getters/Setters for packet fields
		*/
		uint8_t getX() const;
		uint8_t getY() const;
		uint8_t getZ() const;
		uint8_t getRot() const;

		void setX(uint8_t x);
		void setY(uint8_t y);
		void setZ(uint8_t z);
		void setRot(uint8_t rot);

	private:
		// Packet fields
		//    0 : x
		//    1 : y
		//    2 : z
		//    3 : rot
		uint8_t mFields[4];

		int mCurrentField;
};

#endif


/*
	packetmotion.h

	PacketMotion class - packet type that holds motion information.
		[Incoming] This type is sent from remote to quadcopter

	Fields:
		x   : int8_t, velocity in (-)left, right(+) axis
		y   : int8_t, velocity in (-)backward, forward(+) axis
		z   : int8_t, velocity in (-)down, up(+) axis
		rot : int8_t, rotational velocity in (-)CCW, CW(+) axis

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
			Constructor

			Initializes fields to given values
		*/
		PacketMotion(int8_t x,
				int8_t y,
				int8_t z,
				int8_t rot);

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
		int8_t getX() const;
		int8_t getY() const;
		int8_t getZ() const;
		int8_t getRot() const;

		void setX(int8_t x);
		void setY(int8_t y);
		void setZ(int8_t z);
		void setRot(int8_t rot);

	private:
		// Packet fields
		//    0 : x
		//    1 : y
		//    2 : z
		//    3 : rot
		int8_t mFields[4];

		int mCurrentField;
};

#endif


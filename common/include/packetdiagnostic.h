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

#ifndef PACKETDIAGNOSTIC_H
#define PACKETDIAGNOSTIC_H

#include <string>
#include <stdint.h>

#include "packet.h"

#define PKT_DIAGNOSTIC (char)0b10100001

class PacketDiagnostic : public Packet {
	public:
		/**
			Constructor

			Initializes fields to 0
		*/
		PacketDiagnostic();

		/**
			Constructor

			Initializes fields to given values
		*/
		PacketDiagnostic(uint8_t battery,
				float accel_x,
				float accel_y,
				float accel_z);

		/**
			Gets the header of this specific Packet. In this case, this will
			return PKT_DIAGNOSTIC
		*/
		virtual char getHeader() const;

		virtual bool feedData(std::string &buffer);

		virtual bool getComplete() const;

		virtual std::string serialize() const;

		/**
			Getters/Setters for packet fields
		*/
		uint8_t getBattery() const;
		float getAccelX() const;
		float getAccelY() const;
		float getAccelZ() const;

		void setBattery(uint8_t battery);
		void setAccelX(float accel_x);
		void setAccelY(float accel_y);
		void setAccelZ(float accel_z);

	private:
		// battery field
		uint8_t mBattery;

		// Accelerometer fields
		//    0 : accel_x
		//    1 : accel_y
		//    2 : accel_z
		float mAccel[3];

		int mCurrentField;
};

#endif


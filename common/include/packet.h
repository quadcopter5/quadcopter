/*
	packet.h

	Packet class - abstract base class for a single packet of information
		transferred over a serial connection.

	A Packet can be used for both sending and receiving packets. To receive
	packets, use the feedData() function. To send packets, use the serialize()
	function.

	Derived classes must implement feedData(), getComplete(), and serialize().
	They should define getters for all fields associated with the Packet
	type, as well as setters to set data in an outgoing packet before
	serialize()'ing the Packet.

	Also, each subtype of Packet should #define a PKT_* constant that can be
	compared against getHeader() to determine the type of Packet at runtime.
	Its value should be the 8-bit header for the packet type.
*/

#ifndef PACKET_H
#define PACKET_H

#include <string>

#define PKT_UNKNOWN	(char)0b00000000

class Packet {
	public:
		/**
			Gets the header of this specific Packet, which is a single 8-bit
			bit string.
		*/
		virtual char getHeader() const = 0;

		/**
			Feed serial data into the Packet. As data is fed, it is divided
			and stored into appropriate instance variables.

			Any amount of data can be fed per call; if the given buffer does
			not fill all fields of the packet, subsequent calls to
			feedData() will continue where previous calls left off.

			If the given buffer ends partway through a packet field, the
			Packet can choose to either consume the partial field and use
			it when more data comes in, or it can leave the data in buffer.
			In any case, it is expected that any data remaining in buffer
			after this call is at the beginning of the buffer in the next
			call to feedData().

			When a packet is completed, buffer is set to contain all data
			that was not consumed by the Packet. If the packet was not
			completed (therefore all of buffer was used), then buffer is set
			to the empty string. buffer may also still hold data if the Packet
			did not have enough data to continue filling in fields.

			Calling the function when the Packet is already complete will
			start filling the Packet again from the beginning, effectively
			setting the Packet back to the incomplete state.

			Returns true if the packet is completed by this call; false if
			more data is required to complete the packet.
		*/
		virtual bool feedData(std::string &buffer) = 0;

		/**
			Returns true if the Packet is currently complete.

			A Packet is complete if feedData() was called until all packet
			fields were filled. It is also complete if 
		*/
		virtual bool getComplete() const = 0;

		/**
			Serializes the data currently stored within the Packet instance.
			This does NOT include the packet header! Only the packet fields.

			Data fields should be set using setters (defined in subclasses)
			before calling serialize(). The value of uninitialized fields is
			determined by subclasses, but do not depend on it.

			serialize() ignores the Packet's state of completeness. The
			Packet's state is not modified by this function.

			Returns an std::string containing the serialized data.
		*/
		virtual std::string serialize() const = 0;
};

#endif


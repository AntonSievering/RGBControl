#pragma once

#include <cstdint>
#include <string>
#include "Writer.hpp"
#include "Parser.hpp"

enum class MessageHeader : uint8_t
{
	// GETs
	GET_LED_COUNT  = 0,
	GET_PIXEL      = 1,
	GET_ALL_PIXELS = 2,
	GET_JOB_COUNT  = 3,
	GET_JOB        = 4,
	GET_JOBS       = 5,

	// SETs
	SET_PIXEL              = 10,
	SET_ALL_PIXELS         = 11,
	SET_ALL_PIXELS_UNIFORM = 12,
	ADD_JOB                = 13,
	REMOVE_JOB             = 14,

	// RETURNs
	RETURN_OK      = 20,
	RETURN_ERROR   = 21,
	RETURN_FAILURE = 22,

	// PING
	PING = 30,
};

union int_t
{
	uint8_t  u8[4];
	uint16_t u16[2];
	uint32_t u32;
};

struct Pixel
{
	uint8_t r, g, b;
};


struct Packet
{
	MessageHeader header{};
	uint32_t      nMessageSize{};
	std::string   sMessage{};

public:
	Packet() noexcept = default;

	Packet(const MessageHeader header, const std::string sMessage) noexcept
	{
		this->header = header;
		nMessageSize = sMessage.size();
		this->sMessage = sMessage;
	}

	Packet(const MessageHeader header) noexcept
	{
		this->header = header;
		nMessageSize = 0;
	}

public:
	std::string asString() const noexcept
	{
		Writer writer;
		writer.write(header);
		writer.write(nMessageSize);

		return writer.getContent() + sMessage;
	}
};

static Packet createGetLedCountPacket() noexcept
{
	return Packet(MessageHeader::GET_LED_COUNT);
}

static Packet createGetPixelPacket(const uint32_t index) noexcept
{
	Writer writer;
	writer.write(index);
	return Packet(MessageHeader::GET_PIXEL, writer.getContent());
}

static Packet createGetAllPixelsPacket() noexcept
{
	return Packet(MessageHeader::GET_ALL_PIXELS);
}

static Packet createSetPixelPacket(const uint32_t index, const Pixel pixel) noexcept
{
	Writer writer;
	writer.write(index);
	writer.write(pixel);
	return Packet(MessageHeader::SET_PIXEL, writer.getContent());
}

static Packet createSetAllPixelsPacket(const std::vector<Pixel> &vPixels) noexcept
{
	std::string msg = std::string(vPixels.size() * 3, ' ');
	for (size_t i = 0; i < vPixels.size(); i++)
	{
		msg.at(i * 3 + 0) = vPixels.at(i).r;
		msg.at(i * 3 + 1) = vPixels.at(i).g;
		msg.at(i * 3 + 2) = vPixels.at(i).b;
	}

	return Packet(MessageHeader::SET_ALL_PIXELS, msg);
}

static Packet createSetAllPixelsUniformPacket(const Pixel col) noexcept
{
	Writer writer;
	writer.write(col);
	return Packet(MessageHeader::SET_ALL_PIXELS_UNIFORM, writer.getContent());
}

#pragma once

#include <cstdint>
#include <string>
#include "Writer.hpp"
#include "Parser.hpp"

enum class MessageHeader : uint8_t
{
	// GETs
	GET_LED_COUNT = 0,
	GET_PIXEL = 1,
	GET_ALL_PIXELS = 2,

	// SETs
	SET_PIXEL = 10,
	SET_ALL_PIXELS = 11,
	SET_ALL_PIXELS_UNIFORM = 12,

	// RETURNs
	RETURN_OK = 20,
	RETURN_ERROR = 21,
	RETURN_FAILURE = 22,

	// PING
	PING = 30
};

union int_t
{
	uint8_t  u8[4];
	uint16_t u16[2];
	uint32_t u32;
};

static std::string u32ToString(const uint32_t index) noexcept
{
	int_t v{};
	v.u32 = index;

	std::string s(4, ' ');
	for (int i = 0; i < 4; i++)
		s.at(i) = v.u8[3 - i];

	return s;
}

static uint32_t readu32(const std::string &src, size_t offset) noexcept
{
	int_t v{};
	
	for (size_t i = 0; i < 4; i++)
		v.u8[3 - i] = src.at(i + offset);

	return v.u32;
}

struct Pixel
{
	uint8_t r, g, b;
};

static Pixel readPixel(const std::string &src, size_t offset) noexcept
{
	Pixel p;
	p.r = src.at(offset);
	p.g = src.at(offset + 1);
	p.b = src.at(offset + 2);
	return p;
}

static std::string colorToString(const Pixel &pixel) noexcept
{
	std::string s(3, pixel.r);
	s.at(1) = pixel.g;
	s.at(2) = pixel.b;

	return s;
}


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
		std::string s(1, (uint8_t)header);
		s += u32ToString(nMessageSize);
		return s + sMessage;
	}
};

static Packet createGetLedCountPacket() noexcept
{
	return Packet(MessageHeader::GET_LED_COUNT);
}

static Packet createGetPixelPacket(const uint32_t index) noexcept
{
	return Packet(MessageHeader::GET_PIXEL, u32ToString(index));
}

static Packet createGetAllPixelsPacket() noexcept
{
	return Packet(MessageHeader::GET_ALL_PIXELS);
}

static Packet createSetPixel(const uint32_t index, const Pixel pixel) noexcept
{
	std::string msg = u32ToString(index) + colorToString(pixel);
	return Packet(MessageHeader::SET_PIXEL, msg);
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
	return Packet(MessageHeader::SET_ALL_PIXELS_UNIFORM, colorToString(col));
}

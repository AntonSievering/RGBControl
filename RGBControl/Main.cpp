#include "Logger.h"
#include <iostream>
#include "Socket/TCP/SocketConnection.h"
#include "common.h"
#include <cmath>

static const Socket::IPAddress g_IP = Socket::IPAddress(192, 168, 0, 200);
static const std::string g_sFilepath = "/home/pi/RGBControl/log.txt";

Socket::TCP::SocketConnection connectToESP8266() noexcept
{
	return Socket::TCP::SocketConnection(g_IP, 5001, Socket::TCP::BlockingMode::Blocking);
}

float clamp(float x, float a, float b)
{
	return std::min(std::max(x, a), b);
}

void hue2rgb(float fHue, size_t index, std::vector<Pixel> &vPixels)
{
	float p1 = 6.0f * std::abs(std::fmod(fHue,               1.0f) - 0.5f);
	float p2 = 4.5f * std::abs(std::fmod(fHue + 1.0f / 3.0f, 1.0f) - 0.5f);
	float p3 = 4.5f * std::abs(std::fmod(fHue + 2.0f / 3.0f, 1.0f) - 0.5f);

	vPixels.at(index).r = (uint8_t)(255.0f * clamp(p1 - 1.0f, 0.0f, 1.0f));
	vPixels.at(index).g = (uint8_t)(255.0f * clamp(p2 - 1.0f, 0.0f, 1.0f));
	vPixels.at(index).b = (uint8_t)(255.0f * clamp(p3 - 1.0f, 0.0f, 1.0f));
}

void main_randomize()
{
	Logger logger(g_sFilepath, true, true);
	auto connection = connectToESP8266();
	if (connection.isConnected())
	{
		logger.log("RANDOMIZE", "Connection established");

		auto current_time = std::chrono::system_clock::now();
		auto duration_in_seconds = std::chrono::duration<double>(current_time.time_since_epoch());
		uint32_t num_seconds = duration_in_seconds.count();

		std::vector<Pixel> vPixels(300);

		for (size_t i = 0; i < 300; i++)
		{
			const float hue = 5.0f / 300.0f * (float)i;
			hue2rgb(hue, i, vPixels);
		}

		Packet packet = createSetAllPixelsPacket(vPixels);
		connection.Send(packet.asString());
		logger.log("RANDOMIZE", "packet sent");

		Parser pReturnMsg = Parser(connection.Recv());
		if (pReturnMsg.size() > 0)
		{
			MessageHeader responseHeader = pReturnMsg.parse<MessageHeader>();
			logger.log("RANDOMIZE", "response header: " + std::to_string((int)responseHeader));
		}
	}
	else
	{
		logger.log("RANDOMIZE", "connection failed");
	}
}

void main_get_led_count()
{
	auto connection = connectToESP8266();
	if (connection.isConnected())
	{
		Packet packet = createGetLedCountPacket();
		connection.Send(packet.asString());
		Parser pReturnMsg = Parser(connection.Recv());

		MessageHeader responseHeader = pReturnMsg.parse<MessageHeader>();
		uint32_t messageSize = pReturnMsg.parse<uint32_t>();
		std::cout << "Response Header: " << (uint8_t)responseHeader << std::endl;

		if (responseHeader == MessageHeader::RETURN_OK && pReturnMsg.size() >= 4)
		{
			uint32_t size = pReturnMsg.parse<uint32_t>();
			std::cout << "Led count: " << size << std::endl;
		}
	}
}

void main_get_pixel()
{
	auto connection = connectToESP8266();
	if (connection.isConnected())
	{
		Packet packet = createGetPixelPacket(0);
		connection.Send(packet.asString());
		Parser pReturnMsg = Parser(connection.Recv());

		MessageHeader responseHeader = pReturnMsg.parse<MessageHeader>();
		uint32_t messageSize = pReturnMsg.parse<uint32_t>();
		std::cout << "Response Header: " << (uint8_t)responseHeader << std::endl;

		if (responseHeader == MessageHeader::RETURN_OK)
		{
			Pixel pixel = pReturnMsg.parse<Pixel>();
			std::cout << "Pixel Color: " << (uint32_t)pixel.r << " " << (uint32_t)pixel.g << " " << (uint32_t)pixel.b << std::endl;
		}
	}
}

void main_set_uniform(Pixel pixel)
{
	Logger logger(g_sFilepath, true, true);
	auto connection = connectToESP8266();

	if (connection.isConnected())
	{
		logger.log("SET_UNIFORM", "connection established");
		
		Packet packet = createSetAllPixelsUniformPacket(pixel);
		connection.Send(packet.asString());
		logger.log("SET_UNIFORM", "packet sent");
		
		std::string sReturn = connection.Recv();
		logger.log("SET_UNIFORM", "return code: " + std::to_string((int)sReturn[0]));
	}
	else
	{
		logger.log("SET_UNIFORM", "connection failed");
	}
}

void main_get_all_pixels()
{
	auto connection = connectToESP8266();
	if (connection.isConnected())
	{
		Packet packet = createGetAllPixelsPacket();
		connection.Send(packet.asString());
		Parser pReturnMsg = Parser(connection.Recv());

		MessageHeader responseHeader = pReturnMsg.parse<MessageHeader>();
		uint32_t messageSize = pReturnMsg.parse<uint32_t>();
		std::cout << "Response Header: " << (uint8_t)responseHeader << std::endl;
		std::cout << "Message Size: " << messageSize << std::endl;

		if (messageSize != pReturnMsg.size())
			pReturnMsg.append(connection.Recv());

		if (responseHeader == MessageHeader::RETURN_OK)
		{
			std::vector<Pixel> vPixels;

			while (pReturnMsg.size() >= 3)
				vPixels.push_back(pReturnMsg.parse<Pixel>());

			for (size_t i = 0; i < vPixels.size(); i++)
				std::cout << "Pixel at " << i << ": " << (uint32_t)vPixels.at(i).r << " " << (uint32_t)vPixels.at(i).g << " " << (uint32_t)vPixels.at(i).b << std::endl;
		}
	}
}

void main_set_pixel(const size_t index, const Pixel pixel) noexcept
{
	auto connection = connectToESP8266();
	if (connection.isConnected())
	{
		Packet packet = createSetPixelPacket(index, pixel);
		connection.Send(packet.asString());
		Parser parser = Parser(connection.Recv());

		MessageHeader header = parser.parse<MessageHeader>();
		std::cout << "MessageHeader: " << (int)header << std::endl;
	}
}

int main()
{
	main_randomize();
	//main_set_uniform({ 0, 0, 0 });

	return EXIT_SUCCESS;
}

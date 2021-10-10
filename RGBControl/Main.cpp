#include <iostream>
#include "Socket/TCP/SocketConnection.h"
#include "common.h"
#include <time.h>

static const Socket::IPAddress g_IP = Socket::IPAddress(192, 168, 0, 103);

Socket::TCP::SocketConnection connectToESP8266() noexcept
{
	return Socket::TCP::SocketConnection(g_IP, 5001, Socket::TCP::BlockingMode::Blocking);
}

void main_randomize()
{
	auto connection = connectToESP8266();
	if (connection.isConnected())
	{
		auto current_time = std::chrono::system_clock::now();
		auto duration_in_seconds = std::chrono::duration<double>(current_time.time_since_epoch());
		uint32_t num_seconds = duration_in_seconds.count();

		uint32_t r1 = num_seconds * 7;
		uint32_t r2 = num_seconds * 11;
		uint32_t r3 = num_seconds * 13;

		std::vector<Pixel> vPixels(300);
		for (auto &pixel : vPixels)
		{
			pixel.r = rand() * r1;
			pixel.g = rand() * r2;
			pixel.b = rand() * r3;
		}

		Packet packet = createSetAllPixelsPacket(vPixels);
		std::cout << "Packet created\n";

		connection.Send(packet.asString());
		std::cout << "Packet sent\n";

		Parser pReturnMsg = Parser(connection.Recv());
		if (pReturnMsg.size() > 0)
		{
			std::cout << "Return Packet received\n";
			MessageHeader responseHeader = pReturnMsg.parse<MessageHeader>();
			std::cout << "Response Header: " << (int)responseHeader << std::endl;
		}
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
	auto connection = connectToESP8266();
	if (connection.isConnected())
	{
		Packet packet = createSetAllPixelsUniformPacket(pixel);
		connection.Send(packet.asString());
		std::string sReturn = connection.Recv();
		std::cout << "Returned code " << (int)sReturn[0] << std::endl;
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

int main()
{
	main_randomize();
	main_set_uniform({ 0, 0, 0 });

	return EXIT_SUCCESS;
}

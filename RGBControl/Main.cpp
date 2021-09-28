#include <iostream>
#include "Socket/TCP/SocketConnection.h"
#include "common.h"

int main_randomize()
{
	Socket::TCP::SocketConnection connection{};
	connection = Socket::TCP::SocketConnection(Socket::IPAddress(192, 168, 178, 21), 5001, Socket::TCP::BlockingMode::Blocking);

	std::vector<Pixel> vPixels(300);
	for (auto &pixel : vPixels)
	{
		pixel.r = rand();
		pixel.g = rand();
		pixel.b = rand();
	}

	Packet packet = createSetAllPixelsPacket(vPixels);
	connection.Send(packet.asString());
	std::string sReturn = connection.Recv();

	MessageHeader responseHeader = (MessageHeader)sReturn[0];
	uint32_t messageSize = readu32(sReturn, 1);
	std::cout << "Response Header: " << (uint8_t)responseHeader << std::endl;
}

int main_get_led_count()
{
	Socket::TCP::SocketConnection connection{};
	connection = Socket::TCP::SocketConnection(Socket::IPAddress(192, 168, 178, 21), 5001, Socket::TCP::BlockingMode::Blocking);

	Packet packet = createGetLedCountPacket();
	connection.Send(packet.asString());
	std::string sReturn = connection.Recv();

	MessageHeader responseHeader = (MessageHeader)sReturn[0];
	uint32_t messageSize = readu32(sReturn, 1);
	std::cout << "Response Header: " << (uint8_t)responseHeader << std::endl;

	if (responseHeader == MessageHeader::RETURN_OK && sReturn.size() >= 9)
	{
		uint32_t size = readu32(sReturn, 5);
		std::cout << "Led count: " << size << std::endl;
	}
}

int main_get_pixel()
{
	Socket::TCP::SocketConnection connection{};
	connection = Socket::TCP::SocketConnection(Socket::IPAddress(192, 168, 178, 21), 5001, Socket::TCP::BlockingMode::Blocking);

	Packet packet = createGetPixelPacket(0);
	connection.Send(packet.asString());
	std::string sReturn = connection.Recv();

	MessageHeader responseHeader = (MessageHeader)sReturn[0];
	uint32_t messageSize = readu32(sReturn, 1);
	std::cout << "Response Header: " << (uint8_t)responseHeader << std::endl;

	if (responseHeader == MessageHeader::RETURN_OK && sReturn.size() >= 8)
	{
		Pixel pixel = readPixel(sReturn, 5);
		std::cout << "Pixel Color: " << (uint32_t)pixel.r << " " << (uint32_t)pixel.g << " " << (uint32_t)pixel.b << std::endl;
	}
}

int main_get_all_pixels()
{
	Socket::TCP::SocketConnection connection{};
	connection = Socket::TCP::SocketConnection(Socket::IPAddress(192, 168, 178, 21), 5001, Socket::TCP::BlockingMode::Blocking);

	Packet packet = createGetAllPixelsPacket();
	connection.Send(packet.asString());
	std::string sReturn = connection.Recv();

	MessageHeader responseHeader = (MessageHeader)sReturn[0];
	uint32_t messageSize = readu32(sReturn, 1);
	std::cout << "Response Header: " << (uint8_t)responseHeader << std::endl;
	
	if (responseHeader == MessageHeader::RETURN_OK)
	{
		std::vector<Pixel> vPixels;
		
		size_t offset = 5;
		while (messageSize >= 3)
		{
			Pixel pixel = readPixel(sReturn, offset);
			vPixels.push_back(pixel);
			offset += 3;
			messageSize -= 3;
		}
		
		for (size_t i = 0; i < vPixels.size(); i++)
			std::cout << "Pixel at " << i << ": " << (uint32_t)vPixels.at(i).r << " " << (uint32_t)vPixels.at(i).g << " " << (uint32_t)vPixels.at(i).b << std::endl;
	}
}


int main()
{
	main_randomize();
	return main_get_all_pixels();
}

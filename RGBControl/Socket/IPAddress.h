#pragma once

#include "defines.h"


namespace Socket
{
	struct IPAddress
	{
		union
		{
			uint8_t addr[4];
			uint32_t chunk;
		};

	public:
		IPAddress() noexcept
		{
			// default IP address = 127.0.0.1
			addr[0] = 127;
			addr[1] = 0;
			addr[2] = 0;
			addr[3] = 1;
		}

		IPAddress(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3) noexcept
		{
			addr[0] = d0;
			addr[1] = d1;
			addr[2] = d2;
			addr[3] = d3;
		}

		IPAddress(uint8_t addr[4]) noexcept
		{
			this->addr[0] = addr[0];
			this->addr[1] = addr[1];
			this->addr[2] = addr[2];
			this->addr[3] = addr[3];
		}

		IPAddress(const sockaddr_in &sockaddr) noexcept
		{
			copySockaddr(sockaddr);
		}

		IPAddress(const std::string &sIP) noexcept
		{
			sockaddr_in server{};
			server.sin_family = AF_INET;
			server.sin_port = htons(1000);
			inet_pton(AF_INET, sIP.c_str(), &server.sin_addr);

			copySockaddr(server);
		}

	private:
		void copySockaddr(const sockaddr_in &sockaddr) noexcept
		{
			chunk = sockaddr.sin_addr.s_addr;
		}

	public:
		uint8_t &operator[](const uint8_t &i) noexcept
		{
			return addr[i];
		}

		operator uint32_t&() noexcept
		{
			return chunk;
		}

		operator uint32_t() const noexcept
		{
			return chunk;
		}

		std::string asString() const noexcept
		{
			return std::to_string(addr[0]) + "." + std::to_string(addr[1]) + "." + std::to_string(addr[2]) + "." + std::to_string(addr[3]);
		}

	public:
#ifdef _IOSFWD_
		friend std::ostream &operator << (std::ostream &stream, const IPAddress &addr) noexcept
		{
			return stream << addr.asString();
		}
#endif
	};
}

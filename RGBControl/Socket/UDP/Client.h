#pragma once

#include "../defines.h"
#include "../Socket.h"
#include "ClientInfo.h"


namespace Socket
{
	namespace UDP
	{
		class Client
		{
		private:
			Socket     m_socket;

		public:
			Client() noexcept = default;
			
			Client(int port = -1) noexcept
			{
				m_socket = socket(AF_INET, SOCK_DGRAM, 0);
				
				if (port > 0)
				{
					addrinfo hints{}, *result;
					hints.ai_family = AF_INET;
					hints.ai_socktype = SOCK_DGRAM;
					hints.ai_protocol = IPPROTO_UDP;
					hints.ai_flags = AI_PASSIVE;

					getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result);
					bind(m_socket, result->ai_addr, (int)result->ai_addrlen);
					freeaddrinfo(result);
				}
			}

		public:
			bool Send(const std::string &sContent, const ClientInfo &info) noexcept
			{
				sockaddr_in addr = info.getSockaddr();
				return sendto(m_socket, sContent.c_str(), sContent.size(), 0, (sockaddr *)&addr, sizeof(addr)) > 0;
			}
		};
	}
}

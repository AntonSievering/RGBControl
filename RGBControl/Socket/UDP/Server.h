#pragma once

#include "../defines.h"
#include "../IPAddress.h"
#include "ClientInfo.h"
#include "../Socket.h"


namespace Socket
{
	namespace UDP
	{
		struct Message
		{
		public:
			std::string sContent;
			ClientInfo addr;
		};

		class Server
		{
		private:
			constexpr static const int s_bufferLength = 1024;

		private:
			int         m_nCachedClientLength = sizeof(m_cachedClient);
			Socket      m_socket              = INVALID_SOCKET;
			sockaddr_in m_cachedClient{};
			char        m_buffer[s_bufferLength]{};

		public:
			Server() noexcept = default;
			
		public:
			bool Bind(const std::size_t &port) noexcept
			{
				m_socket = socket(AF_INET, SOCK_DGRAM, 0);

				sockaddr_in server{};
				server.sin_addr.S_un.S_addr = ADDR_ANY;
				server.sin_family = AF_INET;
				server.sin_port = htons(port);

				if (bind(m_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
					return false;

				return m_socket != SOCKET_ERROR;
			}

			Message Accept() noexcept
			{
				std::memset(&m_cachedClient, 0x00, m_nCachedClientLength);
				std::string sContent;

				int nReceivedBytes = s_bufferLength;
				while (nReceivedBytes == s_bufferLength)
				{
					nReceivedBytes = recvfrom(m_socket, m_buffer, s_bufferLength, 0, (sockaddr *)&m_cachedClient, &m_nCachedClientLength);

					if (nReceivedBytes > 0)
						sContent.append(m_buffer, nReceivedBytes);

					// reset buffer
					std::memset(m_buffer, 0x00, nReceivedBytes);
				}

				Message message{};
				message.addr = ClientInfo(m_cachedClient);
				message.sContent = sContent;

				return message;
			}

			bool Send(const std::string &sContent, const ClientInfo &info) noexcept
			{
				sockaddr_in addr = info.getSockaddr();
				return sendto(m_socket, sContent.c_str(), sContent.size(), 0, (sockaddr *)&addr, sizeof(addr)) > 0;
			}
		};
	}
}

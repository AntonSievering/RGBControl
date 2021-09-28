#pragma once

#include "../defines.h"
#include "../IPAddress.h"


namespace Socket
{
	namespace UDP
	{
		class ClientInfo
		{
		private:
			sockaddr_in m_serverConnectionInfo{};

		public:
			ClientInfo() noexcept = default;

			ClientInfo(const IPAddress &ip, const uint16_t &port) noexcept
			{
				m_serverConnectionInfo.sin_family = AF_INET;
				m_serverConnectionInfo.sin_port = htons(port);
				m_serverConnectionInfo.sin_addr.S_un.S_addr = ip.chunk;
			}

			ClientInfo(const sockaddr_in &clientInfo) noexcept
			{
				m_serverConnectionInfo = clientInfo;
			}

		public:
			sockaddr_in getSockaddr() const noexcept
			{
				return m_serverConnectionInfo;
			}
		};
	}
}

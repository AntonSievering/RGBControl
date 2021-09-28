#pragma once

#include "defines.h"
#include "UniformUtils.h"


namespace Socket
{
	class Socket
	{
		struct SharedSocket
		{
			SOCKET m_nSocket{};

		public:
			SharedSocket() noexcept = default;
			
			SharedSocket(const SOCKET &socket) noexcept
			{
				m_nSocket = socket;
			}
			
			virtual ~SharedSocket() noexcept
			{
				Uniform::closesocket(m_nSocket);
			}
		};

	private:
		std::shared_ptr<SharedSocket> m_shrSocket{};

	public:
		Socket() noexcept = default;

		Socket(const SOCKET &socket) noexcept
		{
			setSocket(socket);
		}

	public:
		void setSocket(const SOCKET &socket) noexcept
		{
			m_shrSocket = std::make_shared<SharedSocket>(socket);
		}

		SOCKET getSocket() const noexcept
		{
			return m_shrSocket.get()->m_nSocket;
		}

	public:
		operator SOCKET() const noexcept
		{
			return getSocket();
		}
	};
}

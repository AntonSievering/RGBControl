#pragma once

#ifdef _WIN32

#include <Winsock2.h>


namespace Socket
{
	class IOContext
	{
	private:
		WSAData m_wsaData;
		bool bSucceded = false;

	public:
		IOContext() noexcept
		{
			bSucceded = WSAStartup(MAKEWORD(2, 2), &m_wsaData) == 0;
		}

		~IOContext() noexcept
		{
			WSACleanup();
		}

	public:
		bool startupSucceeded() const noexcept
		{
			return bSucceded;
		}
	};
}

#endif

#ifdef _WIN32
#include <Winsock2.h>

class IOContext
{
public:
	IOContext() noexcept
	{
		WSAData wsaData{};
		(void)WSAStartup(MAKEWORD(2, 2), &wsaData);
	}

	~IOContext() noexcept
	{
		WSACleanup();
	}
} g_ioContext{};

#endif

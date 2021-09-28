#pragma once

#ifndef _SOCKET_DEFINES_
#define _SOCKET_DEFINES_

	#include <string>
	#include <functional>
	#include <thread>
	#include <mutex>
	#include <cstring>
	#include <queue>
	#include <memory>

	#ifdef _WIN32
		#include <WinSock2.h>
		#include <ws2tcpip.h>

		#pragma comment (lib, "Ws2_32.lib")
		#pragma comment (lib, "Mswsock.lib")
		#pragma comment (lib, "AdvApi32.lib")

	#elif __linux__
		#include <sys/socket.h>
		#include <sys/types.h>
		#include <sys/ioctl.h>		
		#include <netinet/in.h>
		#include <arpa/inet.h>
		#include <netdb.h>
		
		using SOCKET = int;
		#define INVALID_SOCKET 0

	#endif // _WIN32

#endif // _SOCKET_DEFINES_

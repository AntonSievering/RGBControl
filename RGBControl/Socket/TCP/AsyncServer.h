#pragma once

#include "../ThreadSafe.h"
#include "SocketConnection.h"
#include "Server.h"


namespace Socket
{
	namespace TCP
	{
		template <class ConnectionDataEntry>
		class AsyncServer
		{
			using Connection = std::pair<SocketConnection, ConnectionDataEntry>;

			struct ThreadInfo
			{
				std::thread                        thread{};
				ThreadSafe::Vector<Connection> vActiveConnections{};
			};

		private:
			Server                  m_acceptingServer{};
			std::vector<ThreadInfo> m_vThreadInfoPool{};

		public:
			AsyncServer() noexcept = default;

		public:
			/* \brief Returns the thread index with the lowest count of active connections, or 0 if the load is equal.
			*/
			size_t findLowestLoad() const noexcept
			{
				size_t nLowestIndex = 0;
				size_t nLowestValue = 1'000'000'000;

				// loop through every ThreadInfo trying to find a lower load
				for (size_t i = 0; i < m_vThreadInfoPool.size(); i++)
				{
					size_t nConnections = m_vThreadInfoPool.at(i).vActiveConnections.size();

					if (nConnections < nLowestValue)
					{
						nLowestIndex = i;
						nLowestValue = nConnections;
					}
				}

				return nLowestIndex;
			}

			/* \brief Starts the internal server and handles all incoming connections. Never returns.
			*/
			void start(const uint16_t nPort, const uint16_t nThreads) noexcept
			{
				m_acceptingServer.Bind(nPort);

				for (uint16_t i = 0; i < nThreads; i++)
					m_vThreadInfoPool.push_back({ std::thread(&AsyncServer::threadHandle, this, i), {} });

				while (true)
				{
					SocketConnection conn = m_acceptingServer.Accept();
					ConnectionDataEntry data{};

					if (acceptConnection(conn.getIPAddress(), data))
						m_vThreadInfoPool.at(findLowestLoad()).vActiveConnections.push_back(std::make_pair(conn, data));
				}
			}

			/* \brief Internal method called once by each thread to fulfill their task.
			* 
			*/
			void threadHandle(const uint16_t nId) noexcept
			{
				// pointer to element of a std::vector is okay in this case
				// because the size of m_vThreadInfoPool is not going to change
				ThreadInfo *pOwnThreadInfo = &m_vThreadInfoPool.at(nId);

				while (true)
				{
					size_t i = 0;
					size_t nLoad = 0;
					while (i < pOwnThreadInfo->vActiveConnections.size())
					{
						SocketConnection *pConnection = &pOwnThreadInfo->vActiveConnections.at(i).first;
						ConnectionDataEntry *data = &pOwnThreadInfo->vActiveConnections.at(i).second;

						// close dead connections
						if (!pConnection->isConnected())
						{
							closeConnection(*data);
							pOwnThreadInfo->vActiveConnections.remove(i);
							i--;
						}

						// if query is available
						std::string sQuery = pConnection->Recv();
						if (sQuery.size() > 0)
						{
							// handle the query by answering
							std::string sAnswer = handleQuery(sQuery, *data);
							pConnection->Send(sAnswer);
							nLoad++;
						}

						i++;
					}

					// wait 1ms if nothing had to be done
					// to reduce the idle processor usage
					if (nLoad == 0)
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}

			/* \brief Method that accepts or declines incoming connections.
			*  May be used to block banned IPs.
			*  \param ip   - the connecting ip address
			*  \param data - the data block
			*  \returns bool as wether to accept the connection
			*/
			virtual bool acceptConnection(const IPAddress &ip, ConnectionDataEntry &data) noexcept
			{
				return true;
			}

			/* \brief You need to override this method.
			*  \param sQuery - a std::string containing the received data block
			*  \param data   - the data block
			*  \returns std::string as the reply to the query
			*/
			virtual std::string handleQuery(const std::string &sQuery, ConnectionDataEntry &data) noexcept = 0;

			/* \brief Method called when a connection closes.
			   \param data - the data block
			*/
			virtual void closeConnection(ConnectionDataEntry &data) noexcept
			{}
		};
	}
}

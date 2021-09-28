#pragma once

#include "defines.h"


namespace Socket
{
	namespace ThreadSafe
	{
		template <class T>
		class Vector
		{
		private:
			std::vector<T>              m_vector{};
			std::shared_ptr<std::mutex> m_shrMutex = std::make_shared<std::mutex>();

		public:
			Vector() noexcept {}

		public:
			void push_back(T &&i) noexcept
			{
				std::lock_guard<std::mutex> lock(*m_shrMutex.get());
				m_vector.push_back(i);
			}

			void push_back(const T &i) noexcept
			{
				std::lock_guard<std::mutex> lock(*m_shrMutex.get());
				m_vector.push_back(i);
			}

			T &at(const size_t i) noexcept
			{
				std::lock_guard<std::mutex> lock(*m_shrMutex.get());
				return m_vector.at(i);
			}

			size_t size() const noexcept
			{
				return m_vector.size();
			}

			void remove(const size_t i) noexcept
			{
				std::lock_guard<std::mutex> lock(*m_shrMutex.get());
				m_vector.erase(m_vector.begin() + i);
			}
		};
	}
}

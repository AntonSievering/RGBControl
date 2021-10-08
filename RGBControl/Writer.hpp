#pragma once

#include <cstdint>
#include <string>

class Writer
{
private:
	std::string m_sWriteBuffer;

public:
	Writer() noexcept = default;

public:
	std::string getContent() const noexcept
	{
		return m_sWriteBuffer;
	}

	void writeString(const std::string &s) noexcept
	{
		m_sWriteBuffer += s;
	}

	template <class T>
	void write(const T &t) noexcept
	{
		write(&t, sizeof(T));
	}

	void write(const void *ptr, const size_t size) noexcept
	{
		for (size_t i = 0; i < size; i++)
			m_sWriteBuffer.push_back(((char*)ptr)[i]);
	}
};

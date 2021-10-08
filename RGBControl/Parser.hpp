#pragma once

#include <string>


class Parser
{
private:
	std::string m_sContent{};
	size_t m_nOffset = 0;

public:
	Parser() noexcept = default;

	explicit Parser(const std::string &sContent) noexcept
	{
		m_sContent = sContent;
	}

public:
	size_t size() const noexcept
	{
		return m_sContent.size() - m_nOffset;
	}

	void append(const std::string &str) noexcept
	{
		m_sContent += str;
	}

public:
	void parse(void *ptr, const size_t bytes) noexcept(false)
	{
		if (bytes > size()) throw std::out_of_range({});

		for (size_t i = 0; i < std::min(bytes, size()); i++)
			((char *)ptr)[i] = m_sContent.at(i);
		m_nOffset += bytes;
	}

	template <class T>
	T parse() noexcept
	{
		T t{};
		parse(&t, sizeof(T));
		return t;
	}
};

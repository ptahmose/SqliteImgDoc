#include "utils.h"
#include <sstream>
#include <locale>
#include <codecvt>
#include <cstdlib>

using namespace std;

/*static*/std::wstring Utils::convertUtf8ToWchart(const std::string& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
	std::wstring conv = utf8conv.from_bytes(str);
	return conv;
}

/*static*/std::string Utils::convertToUtf8(const std::wstring& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
	std::string conv = utf8_conv.to_bytes(str);
	return conv;
}

/*static*/std::vector<std::wstring> Utils::wrap(const wchar_t* text, size_t line_length/* = 72*/)
{
	wistringstream iss(text);
	std::wstring word, line;
	std::vector<std::wstring> vec;

	for (; ;)
	{
		iss >> word;
		if (!iss)
		{
			break;
		}

		if (word.size() > 2 && word[0] == L'\\' && word[1] == L'n')
		{
			line.erase(line.size() - 1);	// remove trailing space
			vec.push_back(line);
			line.clear();
			word.erase(0, 2);
		}

		if (line.length() + word.length() > line_length)
		{
			line.erase(line.size() - 1);
			vec.push_back(line);
			line.clear();
		}

		line += word + L' ';
	}

	if (!line.empty())
	{
		line.erase(line.size() - 1);
		vec.push_back(line);
	}

	return vec;
}

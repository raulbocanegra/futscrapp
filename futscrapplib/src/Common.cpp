#include "futscrapplib/Common.h"
#include <string>
#include "libxml/encoding.h"

const xmlChar* futscrapp::common::toXmlCharPtr(const std::string & str)
{
	return reinterpret_cast<const xmlChar*>(str.c_str());
}

std::string futscrapp::common::utf8ToIsoLat1(const xmlChar * message)
{
	int message_len = xmlStrlen(message);
	auto xmlCharPtr = common::make_unique_buffer<xmlChar>(message_len);
	int iso_message_length = message_len;
	auto res = UTF8Toisolat1(xmlCharPtr.get(), &iso_message_length, message, &message_len);
	return std::string(reinterpret_cast<const char*>(xmlCharPtr.get()), iso_message_length);
}

std::vector<std::string> futscrapp::common::split(const std::string & in_str, const std::string & delim)
{
	std::vector<std::string> result;
	size_t token_pos = 0;
	auto delim_pos = in_str.find(delim);
	while (delim_pos != std::string::npos)
	{
		result.emplace_back(in_str.substr(token_pos, delim_pos - token_pos));
		token_pos = (delim_pos + delim.size());
		delim_pos = in_str.find(delim, token_pos);
	}
	if (token_pos < in_str.size())
	{
		result.emplace_back(in_str.substr(token_pos));
	}
	return result;
}

int futscrapp::common::convertPriceToInt(const std::string & price_paid_str)
{
	std::size_t i = 1;
	auto tokens = split(price_paid_str, ".");
	auto exp = tokens.size();
	int price = 0;
	for (const auto& token : tokens)
	{
		price += (std::stoi(token) * static_cast<int>(std::pow(10, (3 * (exp - i++)))));
	}
	return price;
}
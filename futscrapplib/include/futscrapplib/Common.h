#pragma once
#ifndef _FUTSCRAPPLIB_COMMON_H_
#define _FUTSCRAPPLIB_COMMON_H_

#include <vector>
#include <memory>
#include <array>
#include "libxml/xmlstring.h"

namespace futscrapp { namespace common
{
	// -----------------------------------------------------------------------------------------------------------------
	// Aliases
	// -----------------------------------------------------------------------------------------------------------------
	using TablePrice = std::pair<unsigned, int>;

	// -----------------------------------------------------------------------------------------------------------------
	// Constants
	// -----------------------------------------------------------------------------------------------------------------
	constexpr int point_prize = 100000;
	constexpr int best_player_prize = 1000000;
	constexpr int best_eleven_prize = 1000000;
	constexpr int week_prize = 15000000;
	// Prize to be 
	constexpr std::array<int,3> ranking_prize =
	{
		static_cast<int>(week_prize * 0.2),
		static_cast<int>(week_prize * 0.3),
		static_cast<int>(week_prize * 0.5)
	};

	// -----------------------------------------------------------------------------------------------------------------
	// Common helper functions
	// -----------------------------------------------------------------------------------------------------------------
	const xmlChar* toXmlCharPtr(const std::string& str);

	std::string utf8ToIsoLat1(const xmlChar* message);

	// Helper to split a string with a delimiter.
	std::vector<std::string> split(const std::string& in_str, const std::string& delim);
	
	// Convert price of a player in euro from string to int.
	int convertPriceToInt(const std::string& price_paid_str);	

	// template to create and initialize a buffer wrapped in a unique_ptr.
	template <typename T>
	std::unique_ptr<T[]> make_unique_buffer(size_t N)
	{
		auto ptr = std::unique_ptr<T[]>(new T[N]);
		std::memset(ptr.get(), 0, N * sizeof(T));
		return ptr;
	}

}} // futscrapp::common

#endif // !_FUTSCRAPPLIB_COMMON_H_
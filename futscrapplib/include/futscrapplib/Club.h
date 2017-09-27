#pragma once
#ifndef _FUTSCRAPPLIB_CLUB_H_
#define _FUTSCRAPPLIB_CLUB_H_

#include <ostream>
#include <set>
#include <map>
#include <vector>

namespace futscrapp 
{ 	
	// -----------------------------------------------------------------------------------------------------------------
	// Aliases
	// -----------------------------------------------------------------------------------------------------------------
	// A Player is pair consisting of name and isMVP of week.
	using Player = std::pair<std::string, bool>;
	
	// -----------------------------------------------------------------------------------------------------------------
	// Transaction
	// -----------------------------------------------------------------------------------------------------------------
	// struct to store transactions read from html
	// -----------------------------------------------------------------------------------------------------------------
	struct Transaction
	{
		int price;
		std::string player_name;
		std::string origin_club;
		std::string destiny_club;
	};
	
	// -----------------------------------------------------------------------------------------------------------------
	// Club
	// -----------------------------------------------------------------------------------------------------------------
	// Class to encapsulate a Club.
	// -----------------------------------------------------------------------------------------------------------------
	class Club
	{
	public:
		explicit Club(const std::string& name);
				
		Club() = default;
		~Club() = default;
		
		Club(const Club& other) = default;
		Club(Club&& other) = default;
		
		Club& operator=(const Club& other) = default;
		Club& operator=(Club&& other) = default;

		void sellPlayer(const std::string& player, int price);
		void buyPlayer(const std::string& player, int price);

		std::string getName() const;
		
		std::ostream& print(std::ostream& out) const;

	private:

		//int _money;
		std::string _name;
		std::set<std::string> _players;
		friend class League;
	};

	// -----------------------------------------------------------------------------------------------------------------
	// Operators
	// -----------------------------------------------------------------------------------------------------------------	
	std::ostream& operator << (std::ostream& out, const Club& c);

} // futscrapp

#endif // !_FUTSCRAPPLIB_CLUB_H_

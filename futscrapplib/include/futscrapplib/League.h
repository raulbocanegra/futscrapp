#pragma once
#ifndef _FUTSCRAPPLIB_LEAGUE_H_
#define _FUTSCRAPPLIB_LEAGUE_H_

#include <iosfwd>
#include <vector>
#include "futscrapplib/Club.h"
#include "futscrapplib/Table.h"

namespace futscrapp
{	
	// MarketMap. A Map to store the transactions in the market on each day.
	using MarketOfMonth = std::map<std::string, std::vector<Transaction>>;
	using MarketMap = std::map<unsigned, MarketOfMonth>;
	// A structure to map player names and isMVP value;
	using BestTeamOfTheWeek = std::map<std::string, bool>;

	// Class to encapsulate a League.
	class League
	{
	public:
			
		explicit League(const MarketMap& market, const std::vector<Club>& clubs, const std::vector<Table>& tables);

		std::vector<Club> clubs() const;
		size_t getNumClubs() const;

		void fineClub(const std::string& club_name, int amount);
		//void updateMoneyFromMarket();
		void updateMoneyFromPoints();
		void updateMoneyFromPlayers(const std::vector<BestTeamOfTheWeek>& tow);
		void updateMoneyFromPosition();

		std::ostream& print(std::ostream& out) const;

	private:
		
		Table _current_table;
		MarketMap _market;
		std::vector<Club> _clubs;
		std::vector<Table> _fixture_tables;
		
	};

	// Operators
	std::ostream& operator << (std::ostream& out, const League& l);

} // futscrapp

#endif // !_FUTSCRAPPLIB_LEAGUE_H_

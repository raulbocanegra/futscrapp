#pragma once
#ifndef _FUTSCRAPPLIB_TABLE_H_
#define _FUTSCRAPPLIB_TABLE_H_

#include <iosfwd>
#include <vector>
#include "futscrapplib/Club.h"

namespace futscrapp
{	
	// -----------------------------------------------------------------------------------------------------------------
	// Aliases
	// -----------------------------------------------------------------------------------------------------------------
				
	// PositionInfo. A tuple that contains: team name, points, an team players for a fixture.
	using PositionInfo = std::tuple<std::string, double, std::vector<std::string>>;
	// This is a pair whose first value is the position and the second value is a PositionInfo.
	using Position = std::pair<unsigned, PositionInfo>;
	
	// Class to encapsulate a League.
	class Table
	{
	public:

		// Adds a position to the current table.
		bool addPosition(Position&& pos);
		// Get team name from the specified position.
		std::string getClubName(unsigned pos) const;
		// Get points from the specified position.
		double getPoints(unsigned pos) const;
		// Get team  from the specified position.
		std::vector<std::string> getTeamOfWeek(unsigned pos) const;

	private:
		
		mutable std::map<unsigned, PositionInfo> _fixture_team_map;
	};
		
} // futscrapp

#endif // !_FUTSCRAPPLIB_TABLE_H_

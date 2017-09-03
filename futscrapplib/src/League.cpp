#include "futscrapplib/League.h"
#include "futscrapplib/Common.h"
#include <iterator>
#include <iostream>
#include <string>

using namespace futscrapp;

League::League(const MarketMap & market, const std::vector<Club>& clubs, const std::vector<Table>& tables)
	: _market(market)
	, _clubs(clubs)
	, _fixture_tables(tables)
	, _current_table()
{
	// Nothing to do here.
}

std::vector<Club> League::clubs() const
{
	return _clubs;
}

size_t futscrapp::League::getNumClubs() const
{
	return _clubs.size();
}

void League::updateMoneyFromPoints()
{
	std::map<std::string, double> club_points;
	
	for (auto& fixture : _fixture_tables)
	{
		for (size_t i = 0; i < _clubs.size(); ++i)
		{
			club_points[fixture.getClubName(i+1)] += fixture.getPoints(i+1);
		}		
	}
	for (auto& c : _clubs)
	{
		c._money += club_points[c.getName()] * common::point_prize;
	}
}

void League::updateMoneyFromPlayers(const std::vector<BestTeamOfTheWeek>& tow)
{
	size_t week = 0;
	for (auto& fixture : _fixture_tables)
	{
		auto best_tow = tow[week];
		auto best_pow = std::find_if(best_tow.begin(), best_tow.end(),
			[](const BestTeamOfTheWeek::value_type& btow)
		{
			return btow.second;
		});
		std::cout << "Best player of week " << (week + 1) << " is: " << best_pow->first << std::endl;

		for (size_t i = 0; i < _clubs.size(); ++i)
		{
			auto club_name = fixture.getClubName(i + 1);
			auto team_of_week = fixture.getTeamOfWeek(i + 1);
			
			std::vector<std::string> best_tow_v;
			std::transform(
				best_tow.begin(), best_tow.end(), 
				std::back_inserter(best_tow_v),
				[](const BestTeamOfTheWeek::value_type& btow)
			{
				return btow.first;
			});
			std::sort(team_of_week.begin(), team_of_week.end());
			std::sort(best_tow_v.begin(), best_tow_v.end());
 			std::vector<std::string> intersection;
			std::set_intersection(
				team_of_week.begin(), team_of_week.end(),
				best_tow_v.begin(), best_tow_v.end(),
				std::back_inserter(intersection));
			auto num_players = intersection.size();
			std::cout << "Club " << club_name << " has " << num_players << " in the team of the week " << (week + 1)
					  << std::endl;
			if (num_players)
			{
				std::cout << "Players:";
				for (const auto& p : intersection)
				{
					std::cout << " " << p;
				}
				std::cout << std::endl;
			}
			auto has_bpow = std::find(intersection.begin(), intersection.end(), best_pow->first) != intersection.end();
			
			auto it = std::find_if(
				_clubs.begin(), _clubs.end(), 
				[&club_name](const Club& c)
			{
				return (c.getName() == club_name);
			});
			if (it == _clubs.end())
			{
				throw std::runtime_error("Club not found!");
			}
			it->_money += (num_players * common::best_eleven_prize) + ((has_bpow) ? common::best_player_prize : 0);

		}
		++week;
	}
}

void futscrapp::League::updateMoneyFromPosition()
{
	auto num_clubs = _clubs.size();
	size_t week = 1;
	for (const auto& fixture : _fixture_tables)
	{
		// Get the name of the 3 last clubs of the fixture and update their money
		for (size_t i = 0; i < 3; ++i)
		{
			std::string club_name = fixture.getClubName(num_clubs - i);
			auto it = std::find_if(
				_clubs.begin(), _clubs.end(),
				[&club_name](const Club& c)
			{
				return (c.getName() == club_name);
			});
			if (it == _clubs.end())
			{
				throw std::runtime_error("Club not found!");
			}
			std::cout << club_name << " finished in position " << (num_clubs - i) << " on week " << week << std::endl;
			it->_money += common::ranking_prize[i];
		}
		++week;
	}
}

std::ostream & futscrapp::League::print(std::ostream & out) const
{
	std::copy(_clubs.begin(), _clubs.end(), std::ostream_iterator<Club>(out));
	return out;
}

std::ostream & futscrapp::operator<<(std::ostream & out, const League & l)
{
	return l.print(out);
}



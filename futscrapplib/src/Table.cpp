#include "futscrapplib/League.h"
#include <iterator>

using namespace futscrapp;

bool Table::addPosition(Position&& pos)
{
	return _fixture_team_map.emplace(std::move(pos)).second;
}

std::string Table::getClubName(unsigned pos) const
{
	std::string result;
	auto it = _fixture_team_map.find(pos);
	if (it != _fixture_team_map.end())
	{
		result = std::get<0>(it->second);
	}
	return result;
}

double Table::getPoints(unsigned pos) const
{
	double result;
	auto it = _fixture_team_map.find(pos);
	if (it != _fixture_team_map.end())
	{
		result = std::get<1>(it->second);
	}
	return result;
}

std::vector<std::string> Table::getTeamOfWeek(unsigned pos) const
{
	std::vector<std::string> result;
	auto it = _fixture_team_map.find(pos);
	if (it != _fixture_team_map.end())
	{
		result = std::get<2>(it->second);
	}
	return result;
}

#include "futscrapplib/Club.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace futscrapp;

Club::Club(const std::string & name)
	: _money(initial_money)	
	, _name(name)
	, _players()
{
	// Nothing to do here
}

void Club::sellPlayer(const std::string& player, int price)
{
	_money += price;
	_players.erase(player);	
}

void Club::buyPlayer(const std::string& player, int price)
{
	_money -= price;
	_players.insert(player);
}

std::string futscrapp::Club::getName() const
{
	return _name;
}

std::ostream & Club::print(std::ostream & out) const
{
	return out << std::setw(30) << std::left << _name << " has " << std::setw(10) << std::put_money(_money) << " and "
		<< _players.size() << " players\n";
}

std::ostream & futscrapp::operator<<(std::ostream & out, const Club & c)
{
	return c.print(out);
}

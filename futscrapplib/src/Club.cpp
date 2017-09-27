#include "futscrapplib/Club.h"
#include "futscrapplib/Bank.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace futscrapp;

Club::Club(const std::string & name)
	: _name(name)
	, _players()
{
	// Nothing to do here
}

void Club::sellPlayer(const std::string& player, int price)
{
	auto& bank = Bank::getInstance();
	if (!bank.has_account(_name))
	{
		bank.open_account(_name);
	}
	bank.deposit(_name, price);
	_players.erase(player);	
}

void Club::buyPlayer(const std::string& player, int price)
{
	auto& bank = Bank::getInstance();
	if (!bank.has_account(_name))
	{
		bank.open_account(_name);
	}
	bank.withdraw(_name, price);
	_players.insert(player);
}

std::string futscrapp::Club::getName() const
{
	return _name;
}

std::ostream & Club::print(std::ostream & out) const
{
	auto& b = Bank::getInstance();
	return out << std::setw(30) << std::left << _name << " has " << std::setw(10) << std::put_money(b.getMoney(_name)) 
		       << " and " << _players.size() << " players" << std::endl;
}

std::ostream & futscrapp::operator<<(std::ostream & out, const Club & c)
{
	return c.print(out);
}

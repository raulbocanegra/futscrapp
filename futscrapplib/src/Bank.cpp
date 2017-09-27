#include "futscrapplib/Bank.h"
#include "futscrapplib/Common.h"
#include <iostream>
#include <iomanip>
#include <string>


using namespace futscrapp;

Bank & futscrapp::Bank::getInstance()
{
	static Bank b;
	return b;
}

bool Bank::open_account(const std::string & client)
{
	return _accounts.insert({ client, common::initial_money }).second;
}

bool futscrapp::Bank::has_account(const std::string & client) const
{
	return (_accounts.find(client) != _accounts.end());
}

void Bank::deposit(const std::string & account, int money)
{
	auto account_it = _accounts.find(account);
	if (account_it == _accounts.end())
	{
		throw std::runtime_error("Bank account not found!");
	}
	account_it->second += money;
}

void Bank::withdraw(const std::string & account, int money)
{
	auto account_it = _accounts.find(account);
	if (account_it == _accounts.end())
	{
		throw std::runtime_error("Bank account not found!");
	}
	account_it->second -= money;
}

int Bank::getMoney(const std::string & account)
{
	auto account_it = _accounts.find(account);
	if (account_it == _accounts.end())
	{
		throw std::runtime_error("Bank account not found!");
	}
	return account_it->second;
}

std::ostream & Bank::print(std::ostream & out) const
{
	for (const auto& a : _accounts)
	{
		out << std::setw(30) << std::left << a.first << " has " << std::setw(10) << std::put_money(a.second) << std::endl;
	}
	return out;
}

std::ostream & futscrapp::operator<<(std::ostream & out, const Bank & c)
{
	return c.print(out);
}

#pragma once
#ifndef _FUTSCRAPPLIB_BANK_H_
#define _FUTSCRAPPLIB_BANK_H_

#include <iosfwd>
#include <map>

namespace futscrapp 
{ 		
	// Class to encapsulate a Bank where the clubs store their money.
	class Bank
	{
	public:				
		
		~Bank() = default;

		static Bank& getInstance();

		Bank(const Bank& other) = delete;
		Bank(Bank&& other) = delete;
		
		Bank& operator=(const Bank& other) = delete;
		Bank& operator=(Bank&& other) = delete;
		
		// Function to open an account fro the specified client
		bool open_account(const std::string& client);
		// This function checks if the client has an account.
		bool has_account(const std::string& client) const;
		// This function deposits money in the client account.
		void deposit(const std::string& account, int money);
		// This function withdraw money from the client account.
		void withdraw(const std::string& account, int money);
		// This function gets the money from the client account.
		int getMoney(const std::string& account);

		std::ostream& print(std::ostream& out) const;

	private:
		
		Bank() = default;
		std::map<std::string, int> _accounts;
		
	};

	// Operators
	std::ostream& operator << (std::ostream& out, const Bank& c);

} // futscrapp

#endif // !_FUTSCRAPPLIB_BANK_H_

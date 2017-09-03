#include <iostream>
#include "libxml/HTMLparser.h"
#include "libxml/HTMLtree.h"
#include "futscrapplib/Club.h"
#include "futscrapplib/Common.h"
#include "futscrapplib/League.h"
#include "futscrapplib/Table.h"
#include <fstream>
#include <map>
#include <string>

using namespace std;
using namespace futscrapp;
using namespace futscrapp::common;

namespace
{
	// Deleter for unique_ptr<xmlChar> 
	auto xmlCharPtrDel = [](xmlChar* c) { xmlFree(c); };

	std::unique_ptr<xmlChar, decltype(xmlCharPtrDel)> getContentFromNode(htmlNodePtr node)
	{
		return std::unique_ptr<xmlChar, decltype(xmlCharPtrDel)>(xmlNodeGetContent(node), xmlCharPtrDel);
	}

	htmlNodePtr htmlGetNode(htmlNodePtr root, const std::string& node_name, 
		const std::string& attr_name, const std::string& attr_value)
	{
		htmlNodePtr result = nullptr;
		for (htmlNodePtr start = root; start; start = start->next)
		{
			xmlChar* attr = nullptr;
			if ((start->type == XML_ELEMENT_NODE) &&
				(xmlStrEqual(start->name, toXmlCharPtr(node_name))) &&
				(attr = xmlGetProp(start, toXmlCharPtr(attr_name))) &&
				(xmlStrEqual(attr, toXmlCharPtr(attr_value))))
			{
				return start;
			}
			if (result = htmlGetNode(start->children, node_name, attr_name, attr_value))
			{
				return result;
			}
		}
		return result;
	}
	
	std::vector<Transaction> getTransactionsFromXmlNode(htmlNodePtr node)
	{
		std::vector<Transaction> result;
		while (node)
		{
			// article is the last child of the li element
			htmlNodePtr article = xmlLastElementChild(node);
			htmlNodePtr player = xmlFirstElementChild(article);
			htmlNodePtr from = player->next->next;
			htmlNodePtr to = from->next->next;
			htmlNodePtr price = to->next->next;

			auto name = utf8ToIsoLat1(getContentFromNode(player).get());
			auto from_team = utf8ToIsoLat1(getContentFromNode(from).get());
			auto to_team = utf8ToIsoLat1(getContentFromNode(to).get());
			auto price_paid_str = utf8ToIsoLat1(getContentFromNode(price).get());

			int price_paid = common::convertPriceToInt(price_paid_str);
			result.emplace_back(Transaction{ price_paid , name , from_team, to_team });
			node = xmlNextElementSibling(node);
		}
		return result;
	}
	
	MarketOfMonth getTransactionsFromFile(const std::string filename)
	{
		MarketOfMonth market;
		htmlDocPtr html_doc = htmlReadFile(filename.c_str(), "utf8", HTML_PARSE_NOERROR | HTML_PARSE_RECOVER | HTML_PARSE_NOWARNING);
		xmlNode *root_element = xmlDocGetRootElement(html_doc);
		htmlNodePtr all_transfers = htmlGetNode(root_element, "ul", "id", "pressReleases");

		// iterate over pressRelease, extracting the transactions per day, and date.
		for (htmlNodePtr day = xmlFirstElementChild(all_transfers); day; day = xmlNextElementSibling(day))
		{
			xmlChar* class_name = nullptr;
			if ((day->type == XML_ELEMENT_NODE) &&
				(xmlStrEqual(day->name, reinterpret_cast<xmlChar*>("section"))) &&
				(class_name = xmlGetProp(day, reinterpret_cast<xmlChar*>("class"))) &&
				(xmlStrEqual(class_name, reinterpret_cast<xmlChar*>("pressRelease"))) &&
				!xmlIsBlankNode(day->children))
			{
				const xmlChar* date = day->children->next->children->content;
				htmlNodePtr day_transfers = day->children->next->next;
				htmlNodePtr list_item = day_transfers->children;
				std::vector<Transaction> transfers_of_day = getTransactionsFromXmlNode(list_item);
				market.emplace(std::make_pair(reinterpret_cast<const char*>(date), std::move(transfers_of_day)));
			}
		}
		xmlFreeDoc(html_doc);
		return market;
	}
	
	Position getTableFromFile(const std::string& table_file)
	{
		Position p;
		htmlDocPtr table_doc = htmlReadFile(table_file.c_str(), "utf8", HTML_PARSE_NOERROR | HTML_PARSE_RECOVER | HTML_PARSE_NOWARNING);
		htmlNodePtr table_root = xmlDocGetRootElement(table_doc);
		
		// Extract the Club name.
		htmlNodePtr team_name_node = htmlGetNode(table_root, "h2", "id", "selectedUserteam");
		auto node_content = getContentFromNode(team_name_node);
		auto club_name_in_field = utf8ToIsoLat1(node_content.get());
		
		// Extract the team used in the current fixture.
		htmlNodePtr eleven_node = htmlGetNode(table_root, "div", "id", "chosen");
		htmlNodePtr team_node = xmlFirstElementChild(eleven_node);

		// Extract Players.
		std::vector<std::string> players;
		for (htmlNodePtr line = xmlFirstElementChild(team_node); line; line = xmlNextElementSibling(line))
		{
			for (htmlNodePtr player = xmlFirstElementChild(line); player; player = xmlNextElementSibling(player))
			{
				htmlNodePtr player_data = xmlFirstElementChild(player);
				htmlNodePtr name_node = htmlGetNode(player_data, "article", "class", "name pointer");
				auto name_ptr = getContentFromNode(xmlFirstElementChild(name_node));
				auto name = utf8ToIsoLat1(name_ptr.get());
				players.emplace_back(std::move(name));
			}
		}

		// Iterate over the table until the team is found.
		htmlNodePtr table_parent_node = htmlGetNode(table_root, "div", "class", "rankingComponent");
		htmlNodePtr table_node = htmlGetNode(table_parent_node, "ul", "class", "ulRanking");
		for (htmlNodePtr pos = xmlFirstElementChild(table_node); pos; pos = xmlNextElementSibling(pos))
		{
			htmlNodePtr ranking_node = xmlFirstElementChild(pos);
			htmlNodePtr team_name_node = xmlNextElementSibling(ranking_node);
			htmlNodePtr points_node = xmlNextElementSibling(team_name_node);

			auto club_name_table = utf8ToIsoLat1(getContentFromNode(team_name_node).get());
			if (club_name_in_field == club_name_table)
			{
				auto ranking_str = utf8ToIsoLat1(getContentFromNode(ranking_node).get());
				auto ranking = std::stoul(ranking_str);

				auto points_str = utf8ToIsoLat1(getContentFromNode(points_node).get());
				auto points = std::stod(points_str);
				//int points_price = points * common::point_price;
				p = std::make_pair(ranking, std::make_tuple(club_name_table, points, players));				
				break;
			}
		}
		
		xmlFreeDoc(table_doc);
		return p;
	}
	
	BestTeamOfTheWeek getTeamOfTheWeekFromFile(const std::string& clasification_file)
	{
		BestTeamOfTheWeek t;
		htmlDocPtr clasification_doc = htmlReadFile(clasification_file.c_str(), "utf8", HTML_PARSE_NOERROR | HTML_PARSE_RECOVER | HTML_PARSE_NOWARNING);
		htmlNodePtr clasification_root = xmlDocGetRootElement(clasification_doc);
		htmlNodePtr eleven_node = htmlGetNode(clasification_root, "div", "id", "chosen");
		htmlNodePtr team_node = xmlFirstElementChild(eleven_node);

		// Extract Players
		for (htmlNodePtr line = xmlFirstElementChild(team_node); line; line = xmlNextElementSibling(line))
		{
			for (htmlNodePtr player = xmlFirstElementChild(line); player; player = xmlNextElementSibling(player))
			{
				htmlNodePtr player_data = xmlFirstElementChild(player);
				htmlNodePtr points_node = xmlFirstElementChild(player_data);
				xmlChar* mvp = xmlGetProp(xmlFirstElementChild(points_node), toXmlCharPtr("title"));
				auto isMVP = ((mvp != NULL) && xmlStrEqual(mvp, toXmlCharPtr("MVP")));
				htmlNodePtr name_node = xmlLastElementChild(player_data);

				htmlNodePtr name_node_str = xmlFirstElementChild(name_node);
				auto name = utf8ToIsoLat1(getContentFromNode(name_node_str).get());
				t.emplace(name, isMVP);
			}
		}
		xmlFreeDoc(clasification_doc);
		return t;
	}

}

int main()
{
	std::string pressroom_file = "C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\futmondo_pressroom_august.html";
	std::vector<std::string> market_months
	{
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\futmondo_pressroom_august.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\futmondo_pressroom_september.html"
	};

	std::string table_file
		= "C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W1\\TeamOfTheWeek.html";
	std::string out_filename = "C:\\Users\\Raul\\OneDrive\\Programming\\FutScrap.txt";
	std::ofstream fout(out_filename, std::ios::binary);
	std::vector<std::string> week1Teams
	{
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W1\\TeamPos1.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W1\\TeamPos2.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W1\\TeamPos3.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W1\\TeamPos4.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W1\\TeamPos5.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W1\\TeamPos6.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W1\\TeamPos7.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W1\\TeamPos8.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W1\\TeamPos9.html"
	};
	std::vector<std::string> week2Teams
	{
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W2\\TeamPos1.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W2\\TeamPos2.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W2\\TeamPos3.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W2\\TeamPos4.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W2\\TeamPos5.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W2\\TeamPos6.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W2\\TeamPos7.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W2\\TeamPos8.html",
		"C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W2\\TeamPos9.html"
	};
	std::map<unsigned, std::vector<std::string>> weeks
	{
		{1, week1Teams},
		{2, week2Teams}
	};

	std::map<unsigned, std::string> teamsOfTheWeek
	{
		{ 1, "C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W1\\TeamOfTheWeek.html" },
		{ 2, "C:\\Users\\Raul\\OneDrive\\Programming\\C++\\futscrapp\\res\\W2\\TeamOfTheWeek.html" }
	};

	// Read the market form file
	
	MarketMap market;
	for (size_t i = 0; i < market_months.size(); ++i)
	{
		MarketOfMonth m = getTransactionsFromFile(market_months[i]);
		market.emplace(std::make_pair(i, std::move(m)));
	}

	// Read the market and create the clubs.
	std::vector<Club> clubs;
	for (const auto& month_market : market)
	{
		const auto& month = month_market.first;
		const auto& market_of_month = month_market.second;
		for (const auto& day_transfers : market_of_month)
		{
			const auto& day = day_transfers.first;
			const auto& transfers = day_transfers.second;
			for (const auto& t : transfers)
			{
				std::string club_name = (t.destiny_club != "futmondo") ? t.destiny_club : t.origin_club;
				auto it = std::find_if(clubs.begin(), clubs.end(),
					[&club_name](const Club& c)
				{
					return (c.getName() == club_name);
				});

				if (t.destiny_club != "futmondo") // club is buying
				{						
					if (it == clubs.end())
					{
						Club c(t.destiny_club);
						c.buyPlayer(t.player_name, t.price);
						clubs.emplace_back(std::move(c));
					}
					else
					{
						it->buyPlayer(t.player_name, t.price);
					}
					if (t.origin_club != "futmondo") // another club is selling
					{
						auto selling_it = std::find_if(clubs.begin(), clubs.end(),
							[&t](const Club& c)
						{
							return (c.getName() == t.origin_club);
						});
						selling_it->sellPlayer(t.player_name, t.price);
					}
				}
				else // club is selling
				{	
					it->sellPlayer(t.player_name, t.price);					
				}
			}
		}
	}

	// Eleven of the week
	std::vector<BestTeamOfTheWeek> teams_of_the_week;
	for (const auto& team_file : teamsOfTheWeek)
	{
		BestTeamOfTheWeek team_of_the_week = getTeamOfTheWeekFromFile(team_file.second);
		teams_of_the_week.emplace_back(std::move(team_of_the_week));
	}
	
	// Read the tables for file.
	std::vector<Table> tables;	
	for (const auto& week : weeks)
	{
		Table table;
		for (const auto& file : week.second)
		{			
			Position p = getTableFromFile(file);
			if (!table.addPosition(std::move(p)))
			{
				throw std::runtime_error("Error inserting Position in Table!!");
			}			
		}
		tables.emplace_back(std::move(table));
    }
	
	League league(market, clubs, tables);

	league.updateMoneyFromPoints();
	league.updateMoneyFromPlayers(teams_of_the_week);
	league.updateMoneyFromPosition();

	// TODO: Add Fines.
	// output
	for (const auto& c : league.clubs())
	{
		fout << c;
		std::cout << c;
	}
	return 0;
}

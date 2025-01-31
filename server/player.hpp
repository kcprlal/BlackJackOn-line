#pragma once
#include "card.hpp"
#include <iostream>
#include <bits/stdc++.h>
class Player
{
private:
    std::vector<Card> hand{};
    int wallet{};
    std::string login{};
    std::string password{};
    bool win{};
    int wins{};
    int score{};
    int aces{};
    public:
    Player(std::string log, std::string pass, int wal, int wi) : login(log), password(pass), wallet(wal), wins(wi){};
    ~Player() {};
    const std::string getLogin() { return login; };
    const std::string getPassword() { return password; };
    const bool getWin()  { return win; };
    const int getWallet() { return wallet; };
    const int getWins() { return wins; };
    const int getScore() { return score; };
    void setWin(bool newwin) { win = newwin; };
    void setScore(int newscore) { score = newscore; };
    void setAces(int newaces) { aces = newaces; };
    void clearhand() { hand.clear(); };
    void deposit(int value) { wallet += value; };
    void withdraw(int value)
    {
        if (wallet > value)
            wallet -= value;
    };
    void printHand()
    {
	for (const auto &element : hand)
	{
            std::cout << element << " | ";
        }
        std::cout << std::endl;
    }
    void printWallet()
    {
	std::cout << "Your balance: " << wallet << std::endl;
    }

    void handClear()
    {
	hand.clear();
    }

    std::string drawCard()
    {
	int cardvalue[13]{11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};
        std::string cardsymb[13]{"A", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "J", "Q", "K"};
        int randomnumber = rand() % 13;
	int randomsymbol = rand() % 4;
	if (randomnumber == 0)
	     aces++;
	score += cardvalue[randomnumber];
	if (score > 21 && aces > 0)
	{
	     aces--;
	     score -= 10;
	}
	std::string figure[4]{"diamonds", "hearts", "clubs", "spades"};
	Card card{cardvalue[randomnumber], figure[randomsymbol], cardsymb[randomnumber]};
	hand.push_back(card);
	return std::to_string(randomnumber) + "x" + std::to_string(randomsymbol);
    }

};

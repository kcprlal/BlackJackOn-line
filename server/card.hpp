#pragma once
#include<iostream>
#include<string>
class Card
{
    private:
	bool faceup{};
	int cardvalue{};
	std::string figure{};
	std::string cardsymb{};
    public:
	Card(int cv, std::string fig, std::string symb) : cardvalue(cv), figure(fig), cardsymb(symb) {};
	~Card() {};
    friend std::ostream &operator<<(std::ostream &os, const Card &object)
    {
	os << object.cardsymb << " of " << object.figure;
	return os;
    }
    std::string getSymb() { return cardsymb; };
};

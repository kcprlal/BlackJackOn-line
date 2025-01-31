#pragma once
#include <iostream>
#include <string>
class Card
{
private:
    std::string figure{};
    std::string cardsymb{};

public:
    Card(std::string fig, std::string symb) : figure(fig), cardsymb(symb) {};
    ~Card() {};
    friend std::ostream &operator<<(std::ostream &os, const Card &object)
    {
        os << object.cardsymb << " of " << object.figure;
        return os;
    }
    std::string getSymb() { return cardsymb; };
};

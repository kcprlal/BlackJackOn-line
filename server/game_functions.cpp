#include "game_functions.hpp"
#include<filesystem>
#include<fstream>
#include <vector>
#include <string>
#include <iostream>

std::string playerRegister(const std::string& login, const std::string& password)
{
    std::ofstream baza("bazakont.txt", std::ios::app);
    std::cout<<"Player "<<login<<" has been registered"<<std::endl;
    baza << login << " "
         << password << " "
         << 0 << " "
         << 0 << std::endl;
    baza.close();
    return login;
}

bool playerLogin(std::vector<Player> &regdata,const std::string& login, const std::string& password)
    {
	std::ifstream baza("bazakont.txt");
	std::string fname, fpass, tempread;
	int wins, wallet;

	if (baza.is_open())
	{
            while (std::getline(baza, tempread))
            {
                std::istringstream linestream(tempread);
                linestream >> fname >> fpass >> wallet >> wins;
                if (fname == login)
                    break;
            }

            if (fpass != password)
            {
                std::cout << "Wrong_password :(" << std::endl;
		baza.close();
		return false;
            }
            std::cout << "Logged in to: " << login << std::endl;
        }
        Player tempplayer(fname, fpass, wallet, wins);
	regdata.emplace_back(tempplayer);
	baza.close();
	return true;
}

int ranking(const std::string& login)
{
    std::ifstream baza("bazakont.txt");
    std::string password, fname, fpass, tempread;
    int wins, wallet;

    if (baza.is_open())
    {
        while (std::getline(baza, tempread))
        {
            std::istringstream linestream(tempread);
            linestream >> fname >> fpass >> wallet >> wins;
            if (fname == login)
                break;
        }
    }
    baza.close();
    return wins;
}

void playgame(std::vector<Player> &vecdata, char action, std::string& cardinfo)
{
        if (action == 'h')
        {
            cardinfo = vecdata[1].drawCard();
            if (vecdata[1].getScore() > 21)
            {
                vecdata[0].clearhand();
                vecdata[1].clearhand();
                vecdata[0].setScore(-1);
		vecdata[1].setScore(-1);
                vecdata[0].setAces(0);
                vecdata[1].setAces(0);
            }
        }
        else if (action = 's')
        {
            cardinfo = vecdata[0].drawCard();
	    if(vecdata[0].getScore() > 21)
	    {
		editFile(vecdata[1].getLogin(), 1, 2);
		vecdata[1].setWin(true);
		vecdata[0].clearhand();
                vecdata[1].clearhand();
		vecdata[0].setScore(-1);
                vecdata[1].setScore(-1);
		vecdata[0].setAces(0);
                vecdata[1].setAces(0);
            }
            else if (vecdata[0].getScore() >= 17 && vecdata[0].getScore() < vecdata[1].getScore())
            {
		vecdata[1].setWin(true);
                editFile(vecdata[1].getLogin(), 1, 2);
		vecdata[0].clearhand();
                vecdata[1].clearhand();
		vecdata[0].setScore(-1);
                vecdata[1].setScore(-1);
                vecdata[0].setAces(0);
                vecdata[1].setAces(0);
            }
	    else if(vecdata[0].getScore() >= 17 && vecdata[0].getScore() >= vecdata[1].getScore())
	    {
		vecdata[0].clearhand();
                vecdata[1].clearhand();
                vecdata[0].setScore(-1);
                vecdata[1].setScore(-1);
                vecdata[0].setAces(0);
		vecdata[1].setAces(0);
	    }

        }
}

void editFile(const std::string &targetName, int amountToAdd, int option)
{
    std::ifstream inputFile("bazakont.txt");
    std::vector<Player> users;

    if (!inputFile.is_open())
    {
        std::cerr << "Can't open file!\n";
        return;
    }

    std::string line;
    while (std::getline(inputFile, line))
    {
        std::istringstream linestream(line);
        std::string name, password;
        int balance, wins;
        if (linestream >> name >> password >> balance >> wins)
        {
            if (name == targetName)
            {
                switch (option)
                {
                case 0:
                    balance += amountToAdd;
                    break;
                case 1:
                    balance -= amountToAdd;
                    break;
		 case 2:
                    wins += amountToAdd;
                    break;
                default:
                    std::cout << ":c" << std::endl;
                }
            }
            users.push_back({name, password, balance, wins});
        }
    }
    inputFile.close();

    std::ofstream outputFile("bazakont.txt");
    if (!outputFile.is_open())
    {
        std::cerr << "Can't open file for write!\n";
        return;
    }

    for (auto &user : users)
    {
        outputFile << user.getLogin() << " " << user.getPassword() << " " << user.getWallet() << " " << user.getWins() << std::endl;
    }
    outputFile.close();
}

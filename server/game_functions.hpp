#pragma once

#include <vector>
#include <string>
#include<iostream>

#include "player.hpp"

std::string playerRegister(const std::string&, const std::string&);
bool playerLogin(std::vector<Player>&, const std::string&, const std::string&);
int ranking(const std::string&);
void printInGame(std::vector<Player>&);
void playgame(std::vector<Player>&, char, std::string&);
void editFile(const std::string &targetName, int amountToAdd, int option);

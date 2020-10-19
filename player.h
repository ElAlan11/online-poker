#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <ctime>
using namespace std;

class Player
{
public:
    Player();
    Player(int num, string name);

private:
    short int playerNum;
    string nickname;
    int stack;
    int bet;
    char holeCards[2][2];
};

#endif // PLAYER_H

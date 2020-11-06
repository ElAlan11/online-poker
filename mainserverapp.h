#ifndef MAINSERVERAPP_H
#define MAINSERVERAPP_H

#include "player.h"
#include <iostream>
#include <algorithm>

class mainServerApp
{
private:
    static const int INITIAL_BET = 5;

    int turn;
    int pot;
    bool subPots;

    int contPlayers;
    vector<Player> players;

    char commCards[5][2] = {{'X'}};
    vector<char> suits;
    vector<char> values;
    map<char, vector<char>> deck;


public:
    typedef map<char,int> valToNumMap;
    static valToNumMap valToNum;
    bool static compareCards(const char* card1, const char* card2);

    mainServerApp();
    void newGame();
    char* dealRandomCard(int s);
    short int whichHand(Player p);

};

#endif // MAINSERVERAPP_H

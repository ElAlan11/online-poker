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
    friend class mainServerApp;
    void setCards(char** cards);
    int higherCardVal();
    int lowerCardVal();
    friend ostream& operator<<(ostream& os, const Player& p);

private:
    bool isAllin;
    bool isOut;
    short int playerNum;
    string nickname;
    int stack;
    int bet;
    int secPot;
    char holeCards[2][2];
    int hand[2];
    //hand[0] = Hand
    //hand[1] = MaxValue

    /* HANDS RANK
     * 10 = Royal flush
     * 9 = Straight Flush
     * 8 = Four of a kind
     * 7 = Full house
     * 6 = Flush
     * 5 = Straight
     * 4 = Three of a kind
     * 3 = Two pair
     * 2 = Pair
     * 1 = High card
     */
};

#endif // PLAYER_H

#include "player.h"
#include "pokergame.h"
using namespace std;

Player::Player()
{
    playerNum = 0;
    nickname = "Default";
    stack = 0;
    bet = 0;
}


Player::Player(int number, string name)
{
    playerNum = number;
    nickname = name;

    stack = 9999;
}

void Player::setCards(char** cards){
    holeCards[0][0] = cards[0][0];
    holeCards[0][1] = cards[0][1];
    holeCards[1][0] = cards[1][0];
    holeCards[1][1] = cards[1][1];

    hand[0] = 0;
    hand[1] = 0;

    bet = 0;
}

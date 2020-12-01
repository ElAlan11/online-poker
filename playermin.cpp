#include "playermin.h"

PlayerMin::PlayerMin()
{
    stack = 1000;
    isOut = 0;
    inTheMatch = false;
}

PlayerMin::PlayerMin(int num, string name){
    playerNum = num;
    nickname = name;
    stack = 1000;
    inTheMatch = true;
    isOut = 0;
}

ostream& operator<<(ostream& os, const PlayerMin& p)
{
    os << endl << "Player: " << p.playerNum << endl
         << "Nickname: " << p.nickname << endl
         << "Stack: " << p.stack << endl
         << "Is in the match?: " << p.inTheMatch << endl
         << "Hole Cards: " << p.holeCards[0][0] << p.holeCards[0][1] << " "
                           << p.holeCards[1][0] << p.holeCards[1][1] << endl
         << "Is out?: " << p.isOut << endl
         << "Bet: " << p.bet << endl
         << "Hand: " << p.hand[0] << " " << p.hand[1] << endl;
    return os;
}

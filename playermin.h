#ifndef PLAYERMIN_H
#define PLAYERMIN_H

#include <string>
using namespace std;

class PlayerMin
{
public:
    PlayerMin();
    PlayerMin(int num, string name);
    friend class MainWindow;

private:
    short int playerNum;
    string nickname;
    int stack;
    bool inTheMatch;

    int bet;
    bool isOut;
    char holeCards[2][2];
    int hand[2];
};

#endif // PLAYERMIN_H

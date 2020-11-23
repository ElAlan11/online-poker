#ifndef POKERGAME_H
#define POKERGAME_H

#include "playermin.h"
#include <string>
#include <vector>

using namespace std;
class PokerGame
{
private:
    int turn;
    int pot;
    vector<PlayerMin> competitors;
    PlayerMin me;
    char commCards[5][2] = {{'X'}};

    int ammount;
    int action;
    int lastBet;


    /* Cada que se reciba -Jugada turno-
     * Si jugada = 2 || 3
     *      Si ammount > lastBet
     *          lastBet = ammount
     *
     */

public:
    PokerGame(string nickname);
    bool gameLoop();
};

#endif // POKERGAME_H

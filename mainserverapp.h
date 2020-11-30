#ifndef MAINSERVERAPP_H
#define MAINSERVERAPP_H

#include "player.h"
#include <iostream>
#include <algorithm>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>

class mainServerApp : public QObject
{
    Q_OBJECT
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

    QTcpServer *tcpServer = nullptr;

private slots:
    void playerConnected();


public:
    typedef map<char,int> valToNumMap;
    static valToNumMap valToNum;
    bool static compareCards(const char* card1, const char* card2);
    bool static compareBets(const Player p1, const Player p2);

    mainServerApp();
    bool newGame();
    bool betsRound();
    char* dealRandomCard(int s);
    void whichHand(Player& p);
};

#endif // MAINSERVERAPP_H

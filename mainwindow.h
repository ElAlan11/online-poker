#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QMainWindow>
#include <QMessageBox>
#include <QStyle>

#include "playermin.h"
#include "lobby.h"

#include <string>
#include <vector>
#include <iostream>

#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDataStream>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initializeUI();
    void defaultButtons();
    bool joinMatch(string nickname);
    void updateGameState();
    void showdown(string buffer);
    bool gameLoop();
    void playTurn();
    bool betRound();
    void deletePlayer(int num);
    void closeGame();

private slots:
    void on_foldBtn_clicked();
    void on_checkBtn_clicked();
    void on_raiseBtn_clicked();
    void on_allinBtn_clicked();

public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void displayError(QAbstractSocket::SocketError socketError);
    void initializeNewGame();

signals:
    void matchStarted();
    void beginPreFlop();

private:
    Ui::MainWindow *ui;
    Lobby *lobby;

    QTcpSocket *socket;
    QLabel* playersInfo[4];
    QLabel* playersIcons[4];
    QLabel* playersCards[4][2];
    QLabel* bets[4];
    QLabel* commCardIcons[5];
    QLabel* chipsIcons[4];

    int turn;
    int myNum;
    string nickname;
    int pot;
    PlayerMin players[4];
    char commCards[5][2] = {{'X'}};

    int state;

    int ammount;
    int action;
    int lastBet;
};
#endif // MAINWINDOW_H

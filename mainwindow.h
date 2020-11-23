#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QStyle>
#include "playermin.h"
#include "mainserverapp.h"
#include <string>
#include <vector>

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
    void updateGameState(string buffer);
    void showdown(string buffer);
    bool gameLoop();
    void playTurn();
    bool betRound();

private slots:

    void on_foldBtn_clicked();

    void on_checkBtn_clicked();

    void on_raiseBtn_clicked();

    void on_allinBtn_clicked();

private:
    Ui::MainWindow *ui;
    QLabel* playersInfo[4];
    QLabel* playersIcons[4];
    QLabel* playersCards[4][2];
    QLabel* bets[4];
    QLabel* commCardIcons[5];
    QLabel* chipsIcons[4];

    int turn;
    int myNum;
    int pot;
    PlayerMin players[4];
    char commCards[5][2] = {{'X'}};

    int ammount;
    int action;
    int lastBet;
};
#endif // MAINWINDOW_H

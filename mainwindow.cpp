#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "player.h"
#include "pokergame.h"
#include "mainserverapp.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1500,700);

    mainServerApp game;

//    mainServerApp::valToNum['2'] = 2;
//    mainServerApp::valToNum['3'] = 3;
//    mainServerApp::valToNum['4'] = 4;
//    mainServerApp::valToNum['5'] = 5;
//    mainServerApp::valToNum['6'] = 6;
//    mainServerApp::valToNum['7'] = 7;
//    mainServerApp::valToNum['8'] = 8;
//    mainServerApp::valToNum['9'] = 9;
//    mainServerApp::valToNum['0'] = 10;
//    mainServerApp::valToNum['J'] = 11;
//    mainServerApp::valToNum['Q'] = 12;
//    mainServerApp::valToNum['K'] = 13;
//    mainServerApp::valToNum['A'] = 14;


    //QMessageBox::warning(this,"Title", "Content"); // It can be QMessageBox::information
}

MainWindow::~MainWindow()
{
    delete ui;
}


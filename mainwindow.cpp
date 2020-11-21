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

    //QMessageBox::warning(this,"Title", "Content"); // It can be QMessageBox::information
}

MainWindow::~MainWindow()
{
    delete ui;
}


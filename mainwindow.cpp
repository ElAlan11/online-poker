#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "player.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1500,700);

    Player p1(1,"Alan");
    //QMessageBox::warning(this,"Title", "Content"); // It can be QMessageBox::information
}

MainWindow::~MainWindow()
{
    delete ui;
}


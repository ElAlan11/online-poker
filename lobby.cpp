#include "lobby.h"
#include "ui_lobby.h"
#include "mainwindow.h"

Lobby::Lobby(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Lobby)
{
    ui->setupUi(this);
    this->setFixedSize(370, 620);
    ui->loadingAnimation->show();
    ui->loadinLabel->hide();
}

Lobby::~Lobby()
{
    delete ui;
}

void Lobby::reInitialize(){
    ui->loadinLabel->hide();
    ui->nicknameLbl->show();
    ui->nicknameLinedit->setEnabled(true);
    ui->ipLinedit->setEnabled(true);
    ui->portLinedit->setEnabled(true);
    ui->joinBtn->setEnabled(true);
    ui->loadingAnimation->hide();
}

void Lobby::on_joinBtn_clicked()
{
    if(ui->nicknameLinedit->text().isEmpty() || ui->ipLinedit->text().isEmpty() || ui->portLinedit->text().isEmpty()){
        QMessageBox::warning(this,"Poker Online", "Please fill all the fields to start playing.");
        return;
    }

    QMovie *movie = new QMovie(":/stage/resources/stage/waitAnimation.gif");
    movie->setSpeed(210);
    ui->loadingAnimation->setMovie(movie);
    movie->start();

    ui->loadingAnimation->show();
    ui->loadinLabel->show();
    ui->nicknameLbl->hide();
    ui->nicknameLinedit->setEnabled(false);
    ui->ipLinedit->setEnabled(false);
    ui->portLinedit->setEnabled(false);
    ui->joinBtn->setEnabled(false);

    ((MainWindow*)parentWidget())->joinMatch(ui->nicknameLinedit->text().toStdString(), ui->ipLinedit->text(), ui->portLinedit->text());
}


#include "lobby.h"
#include "ui_lobby.h"


Lobby::Lobby(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Lobby)
{
    ui->setupUi(this);
    ui->loadingAnimation->hide();
    ui->loadinLabel->hide();
}

Lobby::~Lobby()
{
    delete ui;
}

void Lobby::on_joinBtn_clicked()
{
    QMovie *movie = new QMovie(":/stage/resources/stage/waitAnimation.gif");
    movie->setSpeed(200);
    ui->loadingAnimation->setMovie(movie);

    movie->start();
    ui->loadingAnimation->show();
    ui->loadinLabel->show();
    ui->nicknameLbl->hide();
    ui->nicknameLinedit->setEnabled(false);
    ui->joinBtn->setEnabled(false);

    if(true){
        close();
        m.show();
    }
}

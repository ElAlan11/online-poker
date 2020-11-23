#include "lobby.h"
#include "ui_lobby.h"


Lobby::Lobby(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Lobby)
{
    ui->setupUi(this);
    this->setFixedSize(370, 500);
    ui->loadingAnimation->show();
    ui->loadinLabel->hide();
}

Lobby::~Lobby()
{
    delete ui;
}

void Lobby::on_joinBtn_clicked()
{
    if(ui->nicknameLinedit->text().isEmpty()){
        QMessageBox::warning(this,"Poker Online", "Please introduce your nickname to start playing.");
        return;
    }

    QMovie *movie = new QMovie(":/stage/resources/stage/waitAnimation.gif");
    movie->setSpeed(200);
    ui->loadingAnimation->setMovie(movie);
    movie->start();

    ui->loadingAnimation->show();
    ui->loadinLabel->show();
    ui->nicknameLbl->hide();
    ui->nicknameLinedit->setEnabled(false);
    ui->joinBtn->setEnabled(false);

    int res = m.joinMatch(ui->nicknameLinedit->text().toStdString());

    if(res){
        close();
        m.show();
        m.gameLoop();
//        while(m.gameLoop());
//        m.close();
    }
}

#ifndef LOBBY_H
#define LOBBY_H

#include <QDialog>
#include "qmovie.h"
#include <QDialog>

namespace Ui {
class Lobby;
}

class Lobby : public QDialog
{
    Q_OBJECT

public:
    explicit Lobby(QWidget *parent = nullptr);
    ~Lobby();

private slots:
    void on_joinBtn_clicked();

private:
    Ui::Lobby *ui;
};

#endif // LOBBY_H

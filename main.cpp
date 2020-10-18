#include "mainwindow.h"
#include <QApplication>
#include "lobby.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Lobby l;

    l.show();
    return a.exec();
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    lobby = new Lobby(this);
    lobby->show();

    state = 0;
    initializeUI();
}

void MainWindow::initializeUI(){
    ui->setupUi(this);
    this->setFixedSize(1500,700);

    playersInfo[0] = ui->playerProfile1;
    playersInfo[1] = ui->playerProfile2;
    playersInfo[2] = ui->playerProfile3;
    playersInfo[3] = ui->playerProfile4;

    playersIcons[0] = ui->playerIcon1;
    playersIcons[1] = ui->playerIcon2;
    playersIcons[2] = ui->playerIcon3;
    playersIcons[3] = ui->playerIcon4;

    playersCards[0][0] = ui->playerCard1_1;
    playersCards[0][1] = ui->playerCard1_2;
    playersCards[1][0] = ui->playerCard2_1;
    playersCards[1][1] = ui->playerCard2_2;
    playersCards[2][0] = ui->playerCard3_1;
    playersCards[2][1] = ui->playerCard3_2;
    playersCards[3][0] = ui->playerCard4_1;
    playersCards[3][1] = ui->playerCard4_2;

    bets[0] = ui->betP1;
    bets[1] = ui->betP2;
    bets[2] = ui->betP3;
    bets[3] = ui->betP4;

    commCardIcons[0] = ui->comCard1;
    commCardIcons[1] = ui->comCard2;
    commCardIcons[2] = ui->comCard3;
    commCardIcons[3] = ui->comCard4;
    commCardIcons[4] = ui->comCard5;

    chipsIcons[0] = ui->chipsIconP1;
    chipsIcons[1] = ui->chipsIconP2;
    chipsIcons[2] = ui->chipsIconP3;
    chipsIcons[3] = ui->chipsIconP4;
}

void MainWindow::joinMatch(string nickname, QString ip, QString port){

    this->nickname = nickname;
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &MainWindow::connected);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::disconnected);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &MainWindow::displayError);
    connect(this, &MainWindow::matchStarted, this, &MainWindow::initializeNewGame);

    cout << "Connecting..." << endl;


//    QNetworkConfiguration::setConnectTimeout(1000);
    // AGREGAR CAJA DE TEXTO
    socket->connectToHost(ip, port.toInt());
}

void MainWindow::connected()
{
    cout << "Connected." << endl;
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (qint8)1;
    out << QString::fromStdString(nickname);

    socket->write(block);
    socket->flush();
    state = 1;
}
void MainWindow::disconnected()
{
    cout << "Disconnected." << endl;
    socket->close();
    delete(socket);
    socket = nullptr;
    state = 0;
}
void MainWindow::bytesWritten(qint64 bytes)
{
    cout << "Bytes written: " << bytes << endl;
}
void MainWindow::readyRead()
{
    cout << "Bytes recibidos: "<< socket->bytesAvailable() << endl;

    if(socket->bytesAvailable() == 0)
        return;

    QByteArray bytearray = socket->readAll();
    QDataStream in(bytearray);
    in.setVersion(QDataStream::Qt_4_0);

    bool endReached = false;

    while(!endReached){
        qint8 pkgCode;
        in >> pkgCode;

        cout << "Package code: " << (int)pkgCode << endl;

        switch(pkgCode){

        case 2: // Partida encontrada
        {
            if(state != 1)
                break;

            qint8 mynum;
            in >> mynum;
            qint8 playersCont;
            in >> playersCont;

            QString names[4];
            for(int i=0; i<playersCont; i++)
                in >> names[i];

            myNum = mynum;

            for(int i=0; i<playersCont; i++){
                players[i] = PlayerMin(i+1, names[i].toStdString());
                playersInfo[i]->setText(QString::fromStdString(players[i].nickname+"\n"+to_string(players[i].stack)+"$"));
            }

            ui->chatBox->setPlainText("");
            ui->chatLineEdit->setText("");

            playersInfo[myNum-1]->setProperty("myPlayer", true);
            playersInfo[myNum-1]->style()->unpolish(playersInfo[myNum-1]);
            playersInfo[myNum-1]->style()->polish(playersInfo[myNum-1]);
            playersInfo[myNum-1]->update();

            state = 2;
            break;
        }

        case 3: // Nueva partida
        {
            if(state != 2)
                break;

            qint8 cards[2][2];
            in >> cards[0][0];
            in >> cards[0][1];
            in >> cards[1][0];
            in >> cards[1][1];

            for(int i=0; i<2; i++)
                for(int j=0; j<2; j++)
                    players[myNum-1].holeCards[i][j] = cards[i][j];

            emit matchStarted();
            break;
        }

        case 4: // Estado global
        {
            if(state > 2){
                qint8 countP;
                in >> countP;
                qint16 rPot;
                in >> rPot;

                qint8 numPlayer;
                qint16 nBet;
                qint16 nStack;

                for(int i=0; i<countP; i++){
                        in >> numPlayer;
                        in >> nBet;
                        in >> nStack;

                        players[numPlayer-1].bet = nBet;
                        players[numPlayer-1].stack = nStack;
                }

                pot = rPot;
                updateGameState();
            }
            break;
        }

        case 5: // Turno de
        {
            if(state > 2){
                qint8 numPlayer;
                in >> numPlayer;

                turn = numPlayer;
                playersInfo[turn-1]->setEnabled(true);

                if(turn == myNum)
                    playTurn();
            }
            break;
        }

        case 6: // Turno jugado
        {
            if(state > 2){
                qint8 numPlayer;
                in >> numPlayer;
                qint8 move;
                in >> move;
                qint16 money;
                in >> money;

                ammount = money;
                action = move;

                QPixmap skull(":/stage/resources/stage/skull.png");

                switch (action) {
                case 0: // Out
                    players[numPlayer-1].isOut = true;
                    playersIcons[numPlayer-1]->setPixmap(skull);
                    break;
                case 1:
                    break;
                case 2: // Raise
                case 3: // All-in
                    if (ammount > lastBet)
                        lastBet = ammount;
                    break;
                }

                playersInfo[numPlayer-1]->setEnabled(false);
            }
            break;
        }

        case 7: // Cartas comunitarias
        {
            if(state != 3)
                break;

            qint8 cards[3][2];
            in >> cards[0][0];
            in >> cards[0][1];
            in >> cards[1][0];
            in >> cards[1][1];
            in >> cards[2][0];
            in >> cards[2][1];

            for(int i=0; i<3; i++){
                commCards[i][0] = cards[i][0];
                commCards[i][1] = cards[i][1];

                string cardStr = "";
                cardStr += commCards[i][1];
                cardStr += commCards[i][0];
                QPixmap cardImg(":/cards/resources/cards/"+QString::fromStdString(cardStr)+".png");
                commCardIcons[i]->setPixmap(cardImg);
            }

            state = 4;
            break;
        }

        case 8: // Abrir carta
        {
            if(state != 4 && state != 5)
                break;

            qint8 card[2];
            in >> card[0];
            in >> card[1];

            commCards[state-1][0] = card[0];
            commCards[state-1][1] = card[1];

            string cardStr = "";
            cardStr += commCards[state-1][1];
            cardStr += commCards[state-1][0];
            QPixmap cardImg(":/cards/resources/cards/"+QString::fromStdString(cardStr)+".png");
            commCardIcons[state-1]->setPixmap(cardImg);

            state++;
            break;
        }

        case 9: // Showdown
        {
            if(state != 6)
                break;

            qint8 countPlayers;
            in >> countPlayers;
            qint8 qWinner;
            in >> qWinner;
            qint16 qMoney;
            in >> qMoney;

            int winner = qWinner;
            int money = qMoney;

            qint8 num;
            qint8 cards[2][2];
            qint16 hand[2];

            for(int i=0; i<countPlayers; i++){
                in >> num;
                in >> cards[0][0];
                in >> cards[0][1];
                in >> cards[1][0];
                in >> cards[1][1];
                in >> hand[0];
                in >> hand[1];

                players[num-1].holeCards[0][0] = cards[0][0];
                players[num-1].holeCards[0][1] = cards[0][1];
                players[num-1].holeCards[1][0] = cards[1][0];
                players[num-1].holeCards[1][1] = cards[1][1];
                players[num-1].hand[0] = hand[0];
                players[num-1].hand[1] = hand[1];
            }

            for(int i=0; i<4; i++){
                if(players[i].inTheMatch){
                    string cardStr = "";
                    cardStr += players[i].holeCards[0][1];
                    cardStr += players[i].holeCards[0][0];
                    QPixmap cardImg1(":/cards/resources/cards/"+QString::fromStdString(cardStr)+".png");
                    cardStr = "";
                    cardStr += players[i].holeCards[1][1];
                    cardStr += players[i].holeCards[1][0];
                    QPixmap cardImg2(":/cards/resources/cards/"+QString::fromStdString(cardStr)+".png");

                    playersCards[i][0]->setPixmap(cardImg1);
                    playersCards[i][1]->setPixmap(cardImg2);
                }
            }

            QString txtHand;

            switch(players[qWinner-1].hand[0]){
            case 1: txtHand = "Higher Card"; break;
            case 2: txtHand = "Pair"; break;
            case 3: txtHand = "Two Pairs"; break;
            case 4: txtHand = "Three of a kind"; break;
            case 5: txtHand = "Straight"; break;
            case 6: txtHand = "Flush"; break;
            case 7: txtHand = "Full house"; break;
            case 8: txtHand = "Four of a kind"; break;
            case 9: txtHand = "Straight Flush"; break;
            case 10: txtHand = "Royal Flush"; break;
            }

            QMessageBox::StandardButton reply;

            if(winner == myNum)
                reply = QMessageBox::question(this, "Poker Online", "Congratulations! You won "+QString::number(money)
                                              +"$\nYour hand: " + txtHand + "\n\nDo you want to play another round?",
                                              QMessageBox::Yes|QMessageBox::No);

            else
                reply = QMessageBox::question(this, "Poker Online", QString::fromStdString(players[winner-1].nickname)
                                            + " won "+QString::number(money) + "$\nHand: " + txtHand
                                            + "\n\nDo you want to play another round?",
                                            QMessageBox::Yes|QMessageBox::No);

            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_0);
            out << (qint8)15;

            if (reply == QMessageBox::Yes) {
                cout << "\n--- New game started ---\n";

                QMovie *movie = new QMovie(":/stage/resources/stage/waiting.gif");
                movie->setSpeed(200);
                ui->loadingAnimation->setMovie(movie);
                movie->start();

                ui->loadingAnimation->show();
                ui->waitingLbl->show();

                state = 2;
                out << (qint8)1;
                socket->write(block);
                socket->flush();
                emit readyRead();
            } else {
                out << (qint8)0;
                socket->write(block);
                socket->flush();

                closeGame();
            }

            break;
        }

        case 10: // Jugador eliminado
        {
            if(state != 2 && state != 3)
                break;

            qint8 nPlayer;
            in >> nPlayer;

            if(nPlayer == myNum){
                QMessageBox::critical(this,"Game over", "You ran out of money.");
                closeGame();
            }
            deletePlayer(nPlayer);
            break;
        }

        case 11: // Partida terminada
        {
            qint8 reason;
            in >> reason;

            if(reason == 1)
                QMessageBox::critical(this,"Game over", "Detected hacker suspicious behavior");
            else
                QMessageBox::critical(this,"Game over", "All the other players left the game.");

            closeGame();
            break;
        }

        case 12: // Sala llena
        {
            cout << "Entre e case 12" << endl;
            if(state > 1)
                break;

            QMessageBox::information(lobby, tr("Poker online"),
                                     tr("The table you're trying to join is full. Please try later."));
            lobby->reInitialize();
            break;
        }

        case 13: // Jugador dejo la partida
        {
            qint8 nPlayer;
            in >> nPlayer;

            deletePlayer(nPlayer);
            break;
        }

        case 14: // Mensaje del chat
        {
            QString str;
            in >> str;

            ui->chatBox->appendPlainText(str);
            break;
        }

        }

        if(in.atEnd())
            endReached = true;
    }

}

void MainWindow::initializeNewGame(){
    lobby->close();
    show();

    QPixmap grayCard(":/cards/resources/cards/gray_back.png");
    QPixmap redCard(":/cards/resources/cards/red_back.png");
    pot = 0;
    lastBet = 0;
    string buffer = "";

    for(int i=0; i<4; i++){
        if(!players[i].inTheMatch){
            playersIcons[i]->setVisible(false);
            playersInfo[i]->setVisible(false);
            playersCards[i][0]->setVisible(false);
            playersCards[i][1]->setVisible(false);
            bets[i]->setVisible(false);
            chipsIcons[i]->setVisible(false);
        }

        players[i].isOut = false;
        players[i].bet = 0;
        bets[i]->setText(QString::fromStdString(to_string(players[i].bet)+"$"));
        playersCards[i][0]->setPixmap(grayCard);
        playersCards[i][1]->setPixmap(grayCard);
        players[i].hand[0] = 0;
        players[i].hand[0] = 0;
        playersInfo[i]->setEnabled(false);

        QPixmap pIcon(":/stage/resources/stage/player"+QString::number(i+1)+".png");
        playersIcons[i]->setPixmap(pIcon);
    }

    for(int i=0; i<5; i++)
        commCardIcons[i]->setPixmap(redCard);

    ui->waitingLbl->hide();
    ui->loadingAnimation->hide();

    ui->totalPot->setText(QString::number(pot)+"$");

    ui->holeCard1->setPixmap(grayCard);
    ui->holeCard2->setPixmap(grayCard);

    defaultButtons();

    string cardStr = "";
    cardStr += players[myNum-1].holeCards[0][1];
    cardStr += players[myNum-1].holeCards[0][0];
    QPixmap cardImg1(":/cards/resources/cards/"+QString::fromStdString(cardStr)+".png");
    cardStr = "";
    cardStr += players[myNum-1].holeCards[1][1];
    cardStr += players[myNum-1].holeCards[1][0];
    QPixmap cardImg2(":/cards/resources/cards/"+QString::fromStdString(cardStr)+".png");

    ui->holeCard1->setPixmap(cardImg1);
    ui->holeCard2->setPixmap(cardImg2);

    state = 3;

    QApplication::processEvents();
    emit readyRead();
}

void MainWindow::closeGame(){
    close();
    QApplication::quit();
}

void MainWindow::deletePlayer(int nPlayer){
    players[nPlayer-1].inTheMatch = false;

    playersIcons[nPlayer-1]->setVisible(false);
    playersInfo[nPlayer-1]->setVisible(false);
    playersCards[nPlayer-1][0]->setVisible(false);
    playersCards[nPlayer-1][1]->setVisible(false);
    bets[nPlayer-1]->setVisible(false);
    chipsIcons[nPlayer-1]->setVisible(false);
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(lobby, tr("Poker online"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(lobby, tr("Poker online"),
                                 tr("The connection was refused by the server. "
                                    "Make sure the server app is running, "
                                    "and check that the host IP and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(lobby, tr("Poker online"),
                                 tr("The following error occurred: %1.")
                                 .arg(socket->errorString()));
    }

    lobby->reInitialize();
}

bool MainWindow::gameLoop(){

    // --------------------------------------------

/*
    //Wait for a -Jugador sin dinero- or -Cartas personales package-

    if(pkgCode == 0){
        players[myNum-1].holeCards[0][0] = 'C';
        players[myNum-1].holeCards[0][1] = '0';
        players[myNum-1].holeCards[1][0] = 'D';
        players[myNum-1].holeCards[1][1] = 'J';

        string cardStr = "";
        cardStr += players[myNum-1].holeCards[0][1];
        cardStr += players[myNum-1].holeCards[0][0];
        QPixmap cardImg1(":/cards/resources/cards/"+QString::fromStdString(cardStr)+".png");
        cardStr = "";
        cardStr += players[myNum-1].holeCards[1][1];
        cardStr += players[myNum-1].holeCards[1][0];
        QPixmap cardImg2(":/cards/resources/cards/"+QString::fromStdString(cardStr)+".png");

        ui->holeCard1->setPixmap(cardImg1);
        ui->holeCard2->setPixmap(cardImg2);

        //pkgCode = 1; // TEST
    }
    if(pkgCode == 1){ // Player out
        int playerOut = 3;
        //pkgCode = 2; // TEST

        if(pkgCode == 2) // Partida terminada
        {
            QMessageBox::critical(this,"Game over", "All the other players left the game.");
            this->close();
            return false;
        }

        if(playerOut == myNum){
            QMessageBox::critical(this,"Game over", "You ran out of money.");
            this->close();
            return false;
        }
        else{
            players[playerOut-1].inTheMatch = false;

            playersIcons[playerOut-1]->setVisible(false);
            playersInfo[playerOut-1]->setVisible(false);
            playersCards[playerOut-1][0]->setVisible(false);
            playersCards[playerOut-1][1]->setVisible(false);
            bets[playerOut-1]->setVisible(false);
            chipsIcons[playerOut-1]->setVisible(false);
        }
    }
*/



//    // PRE FLOP
//    if(!betRound())
//        return false;

//    // TEST BLOCK
////    turn = 1;
////    while(betRound())
////        turn++;

//    //-------------

////    // THE FLOP
////    // Wait for -Cartas comunitarias- PACKAGE

//    for(int i=0; i<3; i++){
//        commCards[i][0] = 'S';
//        commCards[i][1] = '3';

//        string cardStr = "";
//        cardStr += commCards[i][1];
//        cardStr += commCards[i][0];
//        QPixmap cardImg(":/cards/resources/cards/"+QString::fromStdString(cardStr)+".png");
//        commCardIcons[i]->setPixmap(cardImg);
//    }

////    if(!betRound())
////        return false;

////    // THE TURN
////    // Wait for -Abrir carta- PACKAGE
//    commCards[3][0] = 'C';
//    commCards[3][1] = 'A';

//    string cardStr = "";
//    cardStr += commCards[3][1];
//    cardStr += commCards[3][0];
//    QPixmap cardImg(":/cards/resources/cards/"+QString::fromStdString(cardStr)+".png");
//    commCardIcons[3]->setPixmap(cardImg);

////    if(!betRound())
////        return false;

////    // THE RIVER
////    // Wait for -Abrir carta- PACKAGE
//    commCards[4][0] = 'D';
//    commCards[4][1] = '2';

//    string cardStr2 = "";
//    cardStr2 += commCards[4][1];
//    cardStr2 += commCards[4][0];
//    QPixmap cardImg2(":/cards/resources/cards/"+QString::fromStdString(cardStr2)+".png");
//    commCardIcons[4]->setPixmap(cardImg2);

////    if(!betRound())
////        return false;

////    // Wait for -Showdown- PACKAGE
//    showdown(buffer);

    return true;
}

bool MainWindow::betRound(){

    /*

    // Repeat everything while  pkg != -Cartas comunitarias- -Abrir carta- -Showdown-
    // Wait for -Turno de- PKG
    turn = 3;
    playersInfo[turn-1]->setEnabled(true);

    if(turn == myNum){
        playTurn();
    }
    */

    // Wait and read -Jugada turno- PACKAGE

    /*
    int action = 1;
    //ammount = 0;
    QPixmap skull(":/stage/resources/stage/skull.png");

    switch (action) {
    case 0: // Out
        players[turn-1].isOut = true;
        playersIcons[turn-1]->setPixmap(skull);
        break;
    case 1:
        break;
    case 2: // Raise
    case 3: // All-in
        if (ammount > lastBet)
            lastBet = ammount;
        break;
    }

    */

    //QMessageBox::information(this,"Testing", QString::number(lastBet));

    // Wait for -Estatus global- PACKAGE
//    updateGameState();



    return true;
}

void MainWindow::playTurn()
{
    playersInfo[myNum-1]->setEnabled(true);
    ui->checkBtn->setEnabled(true);
    ui->foldBtn->setEnabled(true);

    int toCall = lastBet - players[myNum-1].bet;

    if(toCall <= 0){ // No raises done for other players
        ui->checkBtn->setEnabled(true);
        ui->checkBtn->setText("Check");
        ammount = 0;

        if(players[myNum-1].stack > 0){ // Enough money to raise
            ui->allinBtn->setEnabled(true);
            ui->raiseBtn->setEnabled(true);
            ui->betSpinBox->setEnabled(true);
            ui->betSpinBox->setMaximum(players[myNum-1].stack);
            ui->betSpinBox->setMinimum(1);
        }
    }
    else{ // Bet raised for other player
        if(toCall < players[myNum-1].stack){ // Enough money to raise
            ui->checkBtn->setText("Call "+QString::number(toCall)+"$");

            ui->allinBtn->setEnabled(true);
            ui->raiseBtn->setEnabled(true);
            ui->betSpinBox->setEnabled(true);
            ui->betSpinBox->setMaximum(players[myNum-1].stack);
            ui->betSpinBox->setMinimum(toCall+1);

            ammount = toCall;
        }
        else{ // Not enough money: All-in or out
            ui->checkBtn->setText("All-in ("+QString::number(players[myNum-1].stack)+"$)");
            ammount = players[myNum-1].stack;
        }
    }
}

void MainWindow::defaultButtons(){
    playersInfo[myNum-1]->setEnabled(false);
    ui->foldBtn->setEnabled(false);
    ui->allinBtn->setEnabled(false);
    ui->checkBtn->setEnabled(false);
    ui->checkBtn->setText("Check/Call");
    ui->raiseBtn->setEnabled(false);
    ui->betSpinBox->setEnabled(false);
    ui->betSpinBox->setValue(1);
}

void MainWindow::updateGameState(){
    ui->totalPot->setText(QString::number(pot)+"$");

    for(int i=0; i<4; i++){
        bets[i]->setText(QString::number(players[i].bet)+"$");
        playersInfo[i]->setText(QString::fromStdString(players[i].nickname+"\n"+to_string(players[i].stack)+"$"));
    }
}

void MainWindow::showdown(){
//(NumJugador, Carta1, Carta2, Mano) x (numJugadores), NumJGanador, Cantidad;
    int nJugadores = 4;

    int num = 1; // !!!!!!!!!! CHANGE
    for(int j=0; j<nJugadores; j++){    
        // int num = 1;
        int i;
        for(i=0; i<4; i++){
            if(players[i].playerNum == num)
                break;
        }
        players[i].holeCards[0][0] = 'S';
        players[i].holeCards[0][1] = 'A';
        players[i].holeCards[1][0] = 'H';
        players[i].holeCards[1][1] = 'K';
        players[i].hand[0] = 1;
        players[i].hand[1] = 9;

        num++;// !!!!!!!!!! DELETE
    }

    for(int i=0; i<4; i++){
        if(players[i].inTheMatch){
            string cardStr = "";
            cardStr += players[i].holeCards[0][1];
            cardStr += players[i].holeCards[0][0];
            QPixmap cardImg1(":/cards/resources/cards/"+QString::fromStdString(cardStr)+".png");
            cardStr = "";
            cardStr += players[i].holeCards[1][1];
            cardStr += players[i].holeCards[1][0];
            QPixmap cardImg2(":/cards/resources/cards/"+QString::fromStdString(cardStr)+".png");

            playersCards[i][0]->setPixmap(cardImg1);
            playersCards[i][1]->setPixmap(cardImg2);
        }
    }

    int nWinner = 3;
    int money = 2000;

    if(nWinner == myNum)
        QMessageBox::information(this,"Poker Online", "Congratulations! You won "+QString::number(money)+"$");
    else
        QMessageBox::information(this,"Poker Online", QString::fromStdString(players[nWinner-1].nickname)+" won "+QString::number(money)+"$");
}

void MainWindow::on_foldBtn_clicked()
{
    if(turn != myNum)
        return;

    players[myNum-1].isOut = true;
    ammount = 0;

    QPixmap skull(":/stage/resources/stage/skull.png");
    playersIcons[myNum-1]->setPixmap(skull);

    // -JugadaTurno- PACKAGE with code 0(Out)
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (qint8)6;
    out << (qint8)myNum;
    out << (qint8)0;
    out << (qint16)ammount;

    socket->write(block);
    socket->flush();

//    QMessageBox::information(this,"PKG To Send", QString::number(0)+" - "+QString::number(ammount));

    defaultButtons();
}

void MainWindow::on_checkBtn_clicked()
{
    if(turn != myNum)
        return;
    if(ammount > players[myNum-1].stack){
        return;
    }

    if(ammount == players[myNum-1].stack){
        // -JugadaTurno- PACKAGE with code 3(All in)
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (qint8)6;
        out << (qint8)myNum;
        out << (qint8)3;
        out << (qint16)ammount;

        socket->write(block);
        socket->flush();
//        QMessageBox::information(this,"PKG To Send", QString::number(3)+" - "+QString::number(ammount));
    }
    else{
        // -JugadaTurno- PACKAGE with code 1(Check/Call)
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (qint8)6;
        out << (qint8)myNum;
        out << (qint8)1;
        out << (qint16)ammount;

        socket->write(block);
        socket->flush();
//        QMessageBox::information(this,"PKG To Send", QString::number(1)+" - "+QString::number(ammount));
    }

    defaultButtons();
}

void MainWindow::on_raiseBtn_clicked()
{
    if(turn != myNum)
        return;
    if(ui->betSpinBox->value() < 1)
        return;

    ammount = ui->betSpinBox->value();
    if(ammount > players[myNum-1].stack)
        return;
    else if(ammount == players[myNum-1].stack){
        // -JugadaTurno- PACKAGE with code 3(All in)
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (qint8)6;
        out << (qint8)myNum;
        out << (qint8)3;
        out << (qint16)ammount;

        socket->write(block);
        socket->flush();
//        QMessageBox::information(this,"PKG To Send", QString::number(3)+" - "+QString::number(ammount));
    }
    else{
        // -JugadaTurno- PACKAGE with code 2(Raise)
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (qint8)6;
        out << (qint8)myNum;
        out << (qint8)2;
        out << (qint16)ammount;

        socket->write(block);
        socket->flush();
//        QMessageBox::information(this,"PKG To Send", QString::number(2)+" - "+QString::number(ammount));
    }
    defaultButtons();
}

void MainWindow::on_allinBtn_clicked()
{
    if(turn != myNum)
        return;

    ui->betSpinBox->setValue(players[myNum-1].stack);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_sendMsgBtn_clicked()
{
    if(ui->chatLineEdit->text().isEmpty())
        return;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    QString msg = ui->chatLineEdit->text();
    out << (qint8)14;
    out << msg;

    socket->write(block);
    socket->flush();

    ui->chatLineEdit->setText("");
}

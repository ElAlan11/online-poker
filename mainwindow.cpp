#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
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

bool MainWindow::joinMatch(string nickname){

    this->nickname = nickname;

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &MainWindow::connected);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::disconnected);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readyRead);
    connect(socket, &QTcpSocket::bytesWritten, this, &MainWindow::bytesWritten);
    connect(socket, &QAbstractSocket::errorOccurred, this, &MainWindow::displayError);

    cout << "Connecting..." << endl;
    socket->connectToHost("192.168.56.1",49300);

    QEventLoop loop;
    loop.connect(this, SIGNAL(matchFound()), SLOT(quit()));
    loop.exec();

    return true;
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
}
void MainWindow::disconnected()
{
    cout << "Disconnected." << endl;
    socket->close();
}
void MainWindow::bytesWritten(qint64 bytes)
{
    cout << "Bytes written: " << bytes << endl;
}
void MainWindow::readyRead()
{
    QDataStream in;
    in.setDevice(socket);
    in.setVersion(QDataStream::Qt_4_0);

    in.startTransaction();
    qint8 pkgCode;
    in >> pkgCode;

    if (!in.commitTransaction())
        return;
    cout << "Package code: " << pkgCode << endl;

    switch(pkgCode){

    case 2: // Partida encontrada
    {
        in.startTransaction();
        qint8 mynum;
        in >> mynum;
        qint8 playersCont;
        in >> playersCont;

        QString names[4];
        for(int i=0; i<playersCont; i++)
            in >> names[i];

        if(!in.commitTransaction())
            return;

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

        emit matchFound();
        break;
    }

    case 3: // Nueva partida
    {
        in.startTransaction();
        qint8 cards[2][2];
        in >> cards[0][0];
        in >> cards[0][1];
        in >> cards[1][0];
        in >> cards[1][1];

        if(!in.commitTransaction())
            return;

        for(int i=0; i<2; i++)
            for(int j=0; j<2; j++)
                players[myNum-1].holeCards[i][j] = cards[i][j];
        cout << players[myNum-1];
        break;
    }

    }
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Poker online"),
                                 tr("Host no encontrado. Por favor, verifica que la dirección "
                                    "IP y el puerto sean correctos"));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Poker online"),
                                 tr("La conexión fue rechazada por el host. "
                                    "La aplicación del servidor no esta activa "
                                    "en este momento."));
        break;
    default:
        QMessageBox::information(this, tr("Poker online"),
                                 tr("Ocurrio el siguiente error: %1.")
                                 .arg(socket->errorString()));
    }

    // Behaviour in case of error
}

bool MainWindow::gameLoop(){
    // ---------- INITIALIZATION BLOCK ----------
    QPixmap grayCard(":/cards/resources/cards/gray_back.png");
    QPixmap redCard(":/cards/resources/cards/red_back.png");
    pot = 0;
    lastBet = 0;
    int pkgCode = 0;
    string buffer = "";

    for(int i=0; i<4; i++){
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

    ui->totalPot->setText(QString::number(pot)+"$");

    ui->holeCard1->setPixmap(grayCard);
    ui->holeCard2->setPixmap(grayCard);

    defaultButtons();
    // --------------------------------------------

    for(PlayerMin p : players)
        if(p.inTheMatch)
            cout << p;

    QEventLoop loop;
    loop.connect(this, SIGNAL(beginPreFlop()), SLOT(quit()));
    loop.exec();

//    //Wait for a -Jugador sin dinero- or -Cartas personales package-

//    if(pkgCode == 0){
//        players[myNum-1].holeCards[0][0] = 'C';
//        players[myNum-1].holeCards[0][1] = '0';
//        players[myNum-1].holeCards[1][0] = 'D';
//        players[myNum-1].holeCards[1][1] = 'J';

//        string cardStr = "";
//        cardStr += players[myNum-1].holeCards[0][1];
//        cardStr += players[myNum-1].holeCards[0][0];
//        QPixmap cardImg1(":/cards/resources/cards/"+QString::fromStdString(cardStr)+".png");
//        cardStr = "";
//        cardStr += players[myNum-1].holeCards[1][1];
//        cardStr += players[myNum-1].holeCards[1][0];
//        QPixmap cardImg2(":/cards/resources/cards/"+QString::fromStdString(cardStr)+".png");

//        ui->holeCard1->setPixmap(cardImg1);
//        ui->holeCard2->setPixmap(cardImg2);

//        //pkgCode = 1; // TEST
//    }
//    if(pkgCode == 1){ // Player out
//        int playerOut = 3;
//        //pkgCode = 2; // TEST

//        if(pkgCode == 2) // Partida terminada
//        {
//            QMessageBox::critical(this,"Game over", "All the other players left the game.");
//            this->close();
//            return false;
//        }

//        if(playerOut == myNum){
//            QMessageBox::critical(this,"Game over", "You ran out of money.");
//            this->close();
//            return false;
//        }
//        else{
//            players[playerOut-1].inTheMatch = false;

//            playersIcons[playerOut-1]->setVisible(false);
//            playersInfo[playerOut-1]->setVisible(false);
//            playersCards[playerOut-1][0]->setVisible(false);
//            playersCards[playerOut-1][1]->setVisible(false);
//            bets[playerOut-1]->setVisible(false);
//            chipsIcons[playerOut-1]->setVisible(false);
//        }
//    }

//    pkgCode = 3; // TEST
//    // Waits for a -Estado global- PKG
//    if(pkgCode == 3){ // Estado global
//        buffer = "";
//        updateGameState(buffer);
//    }

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
    string buffer;
    int pkgCode = 0;

    // Repeat everything while  pkg != -Cartas comunitarias- -Abrir carta- -Showdown-
    // Wait for -Turno de- PKG
    turn = 3;
    playersInfo[turn-1]->setEnabled(true);

    if(turn == myNum){
        playTurn();
    }

    // Wait and read -Jugada turno- PACKAGE

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

    //QMessageBox::information(this,"Testing", QString::number(lastBet));

    // Wait for -Estatus global- PACKAGE
    updateGameState(buffer);

    if(pkgCode == 3){
        QMessageBox::critical(this,"Game over", "Detected hacker suspicious behavior");
        return false;
        this->close();
    }
    playersInfo[turn-1]->setEnabled(false);

    return true;
}

void MainWindow::playTurn()
{
    // TEST BLOCK
    lastBet = 620;
    // -------------

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

void MainWindow::updateGameState(string buffer){
// Bote, ApuestaJ1, ApuestaJ2, ApuestaJ3, ApuestaJ4, DineroJ1, DineroJ2, DineroJ3, DineroJ4.
    pot = 100;
    ui->totalPot->setText(QString::number(pot)+"$");

    for(int i=0; i<4; i++){
        players[i].bet = 200;
        bets[i]->setText(QString::number(players[i].bet)+"$");
    }
    for(int i=0; i<4; i++){
        players[i].stack = 400;
        playersInfo[i]->setText(QString::fromStdString(players[i].nickname+"\n"+to_string(players[i].stack)+"$"));
    }
}

void MainWindow::showdown(string buffer){
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
    QMessageBox::information(this,"PKG To Send", QString::number(0)+" - "+QString::number(ammount));

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
        QMessageBox::information(this,"PKG To Send", QString::number(3)+" - "+QString::number(ammount));
    }
    else{
        // -JugadaTurno- PACKAGE with code 1(Check/Call)
        QMessageBox::information(this,"PKG To Send", QString::number(1)+" - "+QString::number(ammount));
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
        QMessageBox::information(this,"PKG To Send", QString::number(3)+" - "+QString::number(ammount));
    }
    else{
        // -JugadaTurno- PACKAGE with code 2(Raise)
        QMessageBox::information(this,"PKG To Send", QString::number(2)+" - "+QString::number(ammount));
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

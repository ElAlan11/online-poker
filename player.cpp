#include "player.h"

using namespace std;

Player::Player()
{
    playerNum = 0;
    nickname = "Default";
    stack = 0;
    bet = 0;
}


Player::Player(int number, string name)
{
    playerNum = number;
    nickname = name;
    bet = 0;
    stack = 0;

    vector<char> suits = {'S', 'C', 'H', 'D'};
    vector<char> values = {'2', '3', '4', '5', '6', '7', '8', '9', '0', 'J', 'Q', 'K', 'A'};
    map<char, vector<char>> deck;

    for(auto s:suits)
        deck[s] = values;

    /* 4 TESTING

    int emptySuits = 0;
    int cardsDealed = 0;
    int i=0;

    while(emptySuits!=4){
    */
    for(int i=0; i<2; i++){ // .. To give the 2 holecards to the player

        bool emptySuit = false;
        char card[2] = {'X', 'X'};

        while(card[1] == 'X'){
            srand((unsigned) time(0) + i*3); // Random seed

            int suitIndex = rand() % suits.size(); // Selects a random non empty suit
            card[0] = suits[suitIndex];

            int cardsRemaining = deck[card[0]].size(); // ... Cards remaining for a given suit

            if(!cardsRemaining) // If there're no more cards for that suit
            {
                suits.erase(suits.begin() + suitIndex); // Deletes the empty suit
                emptySuit = true;
                //emptySuits++; 4 TEST PURPOSES
                break;
            }

            int cardIndex = rand() % cardsRemaining;
            card[1] = deck[card[0]][cardIndex]; // Selects a random card available in the picked random suit
            deck[card[0]].erase(deck[card[0]].begin() + cardIndex); // Delete the just taken card
        }

        if(!emptySuit){
            holeCards[i][0] = card[0];
            holeCards[i][1] = card[1];

            /* 4 TEST PURPOSES
            cardsDealed++;
            cout << endl << card[0] << card[1] << " " << cardsDealed ;
            i++;
            */
        }
    }

    cout << endl << "HOLE CARDS" << endl << holeCards[0][0] << holeCards[0][1]
                                 << endl << holeCards[1][0] << holeCards[1][1] <<endl;


    /*
    for(auto c:cards)
        for(auto i:c.second)
            cout << c.first << i << endl;
            */
}

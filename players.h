#ifndef PLAYERS_H
#define PLAYERS_H

#include <iostream>
#include <fstream>
#include "deck.h"
#include "logger.h"
using namespace std;

Card target_card;

class Player
{
private:
    static const int FULL_HAND = 2;
    int id;
    int hand_size;
    Card card_one;
    Card card_two;  
    bool dealer;  
    bool winner;

public:
    // constructor
    // called when round starts, initialize with thread_id and first card
    Player(int n, Card initial) : id(n), hand_size(1), card_one(initial), dealer(false), winner(false) {}
    // set dealer
    void setDealer() { dealer = true; }
    // draw card from deck
    // only draw if there is less than 2 cards in hand
    void draw(Card card)
    {
        if (hand_size == FULL_HAND)
            return;
        else if (hand_size == 0)
            card_one = card;
        else if (hand_size == 1)
            card_two = card;

        log_file("PLAYER " + to_string(id) + " draws " + card.print_card());
        cout << "PLAYER " + to_string(id) + " draws " + card.print_card() << endl;
        hand_size++;
    }
    // player draws and returns card
    Card deal(Deck& deck) { return deck.top(); }
    // return card to the bottom of the deck
    // player recieves the deck and places the discarded card at the bottom of the deck
    // if r is 0 then card 1 is discarded, else card two is
    void discard(Deck& deck, uniform_real_distribution<double>& unif, mt19937_64& rng)
    {

        double r = unif(rng); 
        if (r >= .5)
        {
            log_file("PLAYER " + to_string(id) + " discards " + card_one.print_card());
            deck.push(card_one);
            card_one = card_two;
        }
        else
        {
            log_file("PLAYER " + to_string(id) + " discards " + card_two.print_card());
            deck.push(card_two);
        }
            
        
        hand_size--;
    }

    void return_hand(Deck& deck)
    {
        if (hand_size == 1)
        {
            deck.push(card_one);
            log_file("PLAYER " + to_string(id) + " has returned " + card_one.print_card());
            cout << "PLAYER " + to_string(id) + " has returned " + card_one.print_card() << endl;
        }  
        else if(hand_size == 2)
        {
            deck.push(card_one);
            deck.push(card_two);
            log_file("PLAYER " + to_string(id) + " has returned " + card_one.print_card() + " and " + card_two.print_card());
            cout << "PLAYER " + to_string(id) + " has returned " + card_one.print_card() + " and " + card_two.print_card() << endl;
        }
        hand_size = 0; 
    }
    // player checks if their cards match the target
    // if yes, then the player is the winner
    // else false
    bool isMatch()
    {
        if (card_one == target_card || card_two == target_card)
        {
            winner = true;
            if (hand_size == 1)
                log_file("PLAYER " + to_string(id) + " has " + card_one.print_card() + "\nTarget " + target_card.print_card());
            else if (hand_size == 2)
                log_file("PLAYER " + to_string(id) + " has " + card_one.print_card() + " and " + card_two.print_card() + "\nTarget " + target_card.print_card());
            return true;
        }
        return false;
    }
    // functions to check win/deal status
    // and to check current hand size
    bool isWinner() { return winner; }
    bool isDealer() { return dealer; }
    int getHandSize() { return hand_size; }

};

#endif
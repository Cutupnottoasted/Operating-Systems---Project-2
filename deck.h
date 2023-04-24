#ifndef DECK_H
#define DECK_H

#include "cards.h"
#include "logger.h"
using namespace std;

// the deck can shuffle, pop, push, and top
// top is needed incase a player trys to draw a card when they have too many
// this prevents the deck from being altered
class Deck 
{
private:
    vector<Card> cards; 
    static const int DECK_SIZE = 52;
public:
    // constructor, creates a new deck
    Deck()
    {
        Card card;
        Suit s;
        for (int i = 0; i < 4; ++i)
        {
            switch(i)
            {
                case 0: s = SPADES; break;
                case 1: s = CLUBS; break;
                case 2: s = HEARTS; break;
                case 3: s = DIAMONDS; break;
            }
            for (int n = 1; n < 14; ++n)
            {
                 card.num = n;
                 card.suit = s;
                 cards.push_back(card);
            }
        }
    }
    // randomly shuffles deck
    void shuffle()
    {
        Card temp; 
        for (int i = 0; i < DECK_SIZE; i++)
        {
            int r = i + (rand() % (DECK_SIZE - i));
            temp = cards[i];
            cards[i] = cards[r];
            cards[r] = temp;  
        }
    }
    // returns the top card
    Card top() { return cards.back(); }
    // remove card from top of deck (index 51)
    void pop() { cards.pop_back(); }
    // return card to bottom of deck (index 0)
    void push(Card card) { cards.insert(cards.begin(), card); }

    int getSize() { return cards.size(); }

    string print_deck()
    {
        string temp;
        temp += "Deck contents: \n"; 
        for(int i = cards.size() - 1; i > 0; i--)
        {
            switch(cards[i].num)
            {
                case 11:
                    temp += "Jack";
                    break;
                case 12:
                    temp += "Queen";
                    break;
                case 13:
                    temp += "King";
                    break;
                case 0:
                    temp += "Ace";
                    break;
                default:
                    temp += to_string(cards[i].num);
                    break;
            }
            switch(cards[i].suit)
            {
                case 0: 
                    temp += " of Spades ";
                    break; 
                case 1:
                    temp += " of Clubs "; 
                    break;
                case 2: 
                    temp += " of Hearts "; 
                    break;
                case 3:
                    temp += " of Diamonds "; 
                    break;
            }
            
            if (i != 0 && i % 13 == 0)
                temp += "\n";  
        }
        return temp; 
    }
};

#endif 
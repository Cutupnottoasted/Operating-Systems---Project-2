#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdint>
#include <iostream>
#include <random>
#include <chrono>
#include <ctime>
#include <cmath>
using namespace std;

static uint64_t timeSeed; 
// card struct number and suit
enum Suit {SPADES = 0, CLUBS = 1, HEARTS = 2, DIAMONDS = 3};

struct Card 
{
    int num;
    enum Suit suit; 
};

// deck composed of cards 
// deck is global, avaliable to all players
// deck can pop the first, push a card to the bottom, and shuffle
class Deck 
{
private:
    static const int DECK_SIZE = 52;
    int current_size;
    Card *cards = new Card[DECK_SIZE];

public:
    Deck() : current_size(DECK_SIZE)
    {
        int size = 0;
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
            for (int j = 1; j < 14; ++j)
            {
                cards[size].num = j;
                cards[size].suit = s;
                size++; 
            }

        }
    }

    void shuffle(uniform_int_distribution<int>& shuffle, mt19937_64& rng)
    {
        Card *new_deck = new Card[DECK_SIZE];
        int new_size = 0;
        bool shuffled = false;
        int j = 0;
        while (!shuffled)
        {
            int n = 0;
            while (n < 1 || n > DECK_SIZE)
                n = rand() % DECK_SIZE + 1;  
            Card c = cards[n];
            bool contains = false;
            for (int i = 0; i < new_size; ++i)
            {
                if (new_deck[i].num == c.num && new_deck[i].suit == c.suit)
                {
                    contains = true;
                    break;
                }
            }
            if (!contains)
            {
                new_deck[j] = c;
                j++;
                new_size++;
                if (j == DECK_SIZE - 1)
                    shuffled = true;
            }
        }
        delete[] cards;
        cards = new_deck; 
    }
    void pop();
    void push(Card);
    // temp
    void print_deck()
    {
        for(int i = 0; i < 52; i++)
        {
            cout << "Card: " << cards[i].num << " " << cards[i].suit << endl; 
        }
    }
};

// player class
// can draw, recieve, deal, and discard a card
// can check if match, winner, their turn, or is dealer
// can get current hand, declare next round, and state their status (win or loss)
class Player
{
private:
    int id;
    static const int FULL_HAND = 2;
    int hand_size;
    Card* hand;
    bool dealer;  
    bool winner;
public:
    Player(int capacity = FULL_HAND);
    void draw(Card);
    void recieve(Card);
    Card discard();
    Card deal(); 
    bool isMatch();
    bool isWinner();
    bool isTurn();
    bool isDealer();
    string getHand();
    void next_round();
    void declare_status(); 
};



// create defines for rounds
// round 1
// 1. dealer shuffles
// 2. draws target card
// 3. deals 1 card to each player
// 4. place card onto table
// 5. signal round 2



int main(int argc, char* argv[], uint64_t timeSeed)
{
    mt19937_64 rng;
    timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    seed_seq ss { uint32_t (timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
    rng.seed(ss);
    uniform_int_distribution<int> shuffle(1, 13);
    uniform_int_distribution<int> discard(0, 1); 
    // make for discard and shuffling 
    Deck current_deck; 
    current_deck.print_deck();
    current_deck.shuffle(shuffle, rng);

    cout << endl << "Shuffled Deck: " << endl;
    current_deck.print_deck();    
}
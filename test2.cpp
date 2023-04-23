#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdint>
#include <iostream>
#include <random>
#include <chrono>
#include <ctime>
#include <cmath>
#include <vector>
using namespace std;

static uint64_t timeSeed; 
Card target; 
Deck deck;
bool winner;
int delt_cards = 0;
pthread_mutex_t player_mutex;
pthread_cond_t winnerExists;
pthread_t player1, player2, player3, player4, player5, player6;
// card struct number and suit
enum Suit {SPADES = 0, CLUBS = 1, HEARTS = 2, DIAMONDS = 3};

struct Card 
{
    int num;
    enum Suit suit;

    bool operator==(const Card& rhs) const
    {
        if (rhs.num == num && rhs.suit == suit)
            return true;
        return false;
    }
    void print_card()
    {
        cout << "rank: " << num << endl;
        cout << "suit: " << suit << endl; 
    } 
};

// deck composed of cards 
// deck is global, avaliable to all players
// deck can pop the first, push a card to the bottom, and shuffle
class Deck 
{
private:
    vector<Card> cards; 
    static const int DECK_SIZE = 52;
public:
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

    // TOP OF DECK @ INDEX 52 - - - - - - - - INDEX @ 1 BOTTOM OF DECK
    // BASICALLY PULL FROM BOTTOM, PLACE CARD ON TOP BUT REALLY ITS NORMAL

    // draw from top of deck (index 51)
    Card pop()
    {
        Card temp;
        temp = cards.back();
        cards.pop_back();
        return temp;
    }
    // discard card at bottom of deck (index 0)
    void push(Card card)
    {
        cards.insert(cards.begin(), card); 
    }
    
    void swap(int i, int r)
    {
        Card temp;
        temp = cards[i];
        cards[i] = cards[r];
        cards[r] = temp; 
    }
    // temp
    void print_deck()
    {
        cout << endl << "Deck size: " << cards.size() << endl;
        for(int i = 0; i < cards.size(); i++)
        {
            cout << "Card: " << cards[i].num << " " << cards[i].suit << endl; 
        }
    }

    void check_complete(Deck shuffled_deck)
    {
        int count = 0;
        for (Card s : shuffled_deck.cards)
        {
            for (Card c : cards)
            {
                if (s == c)
                {
                    count++;
                    break;
                }
            }
        }
        if (count != 52)
        {
            cout << "Not complete" << endl;
        }
    }

};

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
    Player(int n) : id(n), hand_size(0), winner(false) {}
    void setDealer()
    {
        dealer = true;
    }
    void setPlayer()
    {
        dealer = false; 
    }
    void add_card(Card card)
    {
        if (hand_size == 0)
        {
            card_one = card; 
        }
        else if (hand_size == 1)
        {
            card_two = card; 
        }
        else if (hand_size == 2)
            return;
        hand_size++;
    }
    void discard(Deck& deck)
    {
        int r = rand() % FULL_HAND;
        if (hand_size == 1)
        {
            cout << endl << "Card 1 discarded" << endl;
            deck.push(card_one);
            hand_size--;
            return;
        }
        else if (r == 0)
        {
            cout << endl << "Card 1 discarded" << endl;
            deck.push(card_one);
            card_one = card_two;

        }
        else if (r == 1)
        {
            cout << endl << "Card 2 discarded" << endl;
            deck.push(card_two);
        }
        hand_size--; 
    }
    Card deal()
    {
        if (hand_size == 1)
        {
            hand_size--;
            return card_one;
        }
        else if (hand_size == 2)
        {
            hand_size--;
            return card_two;
        }
    } 
    bool isMatch()
    {
        if (card_one == target || card_two == target)
        {
            cout << "Match is found! " << endl;
            winner = true;
            return true;
        }
    }
    bool isWinner()
    {
        return winner; 
    }
    bool isDealer()
    {
        return dealer; 
    }
    int getHandSize()
    {
        return hand_size; 
    }
    // string getHand();
    // void next_round();
    // void declare_status(); 

    // temp
    void print_hand()
    {
        if (hand_size == 1)
            cout << endl << "Card 1: " << card_one.num << " " << card_one.suit << endl;
        if (hand_size == 2)
        {
            cout << endl << "Card 1: " << card_one.num << " " << card_one.suit << endl;
            cout << endl << "Card 2: " << card_two.num << " " << card_two.suit << endl;
        }
        if (hand_size == 0)
            cout << endl << "No cards." << endl; 
    }
};

struct thread_data
{
    int thread_id;
    Card card;
};

void init()
{
    timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    srand(timeSeed);
    deck = Deck();
    winner = false;  
}

void *round_start(void *param)
{
    auto *arg = (thread_data *) param;
    Player player(arg -> thread_id);
    player.add_card(arg -> card); 
    
    
    while(!winner)
    {
        pthread_mutex_lock(&player_mutex);

        player.add_card(deck.pop());

        if (player.isMatch())
        {
            winner = true;
        }
    }
}


int main(int argc, char* argv[], uint64_t timeSeed)
{
    for (int j = 0; j < 6; ++j)
    {
        
        init();
        cout << "player " << j + 1 << " starts as dealer \n"; 
        thread_data td[6];
        for (int i = 0; i < 6; ++i)
        {
            td[i].thread_id = i + 1;
            td[i].card = deck.pop();
        }

        pthread_create(&player1, nullptr, round_start, (void *) &td[0]);
        pthread_create(&player1, nullptr, round_start, (void *) &td[1]);
        pthread_create(&player1, nullptr, round_start, (void *) &td[2]);
        pthread_create(&player1, nullptr, round_start, (void *) &td[3]);
        pthread_create(&player1, nullptr, round_start, (void *) &td[4]);
        pthread_create(&player1, nullptr, round_start, (void *) &td[5]);
        

    }
}
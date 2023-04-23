


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

#define NUM_THREADS 6 // number of players/rounds
pthread_t p1, p2, p3, p4, p5, p6; // initialize player threads
pthread_mutex_t p_mutex; // player wait/signal
pthread_cond_t next_round; // signal to initiate next round

enum Suit {SPADES = 0, CLUBS = 1, HEARTS = 2, DIAMONDS = 3};
// card structure
// has a number and suit
// operator== checks for matching number
struct Card 
{
    int num;
    enum Suit suit;

    bool operator==(const Card& rhs) const
    {
        return rhs.num == num; 
    }

    void print_card()
    {
        cout << endl << "card: ";
        switch(num)
        {
            case 11:
                cout << "jack ";
                break;
            case 12:
                cout << "queen ";
                break;
            case 13:
                cout << "king ";
                break;
            case 0:
                cout << "ace ";
                break;
            default:
                cout << num << " ";
                break;
        }
        switch(suit)
        {
            case 0: 
                cout << "SPADES" << endl;
                break; 
            case 1:
                cout << "CLUBS" << endl; 
                break;
            case 2: 
                cout << "HEARTS" << endl; 
                break;
            case 3:
                cout << "DIAMONDS" << endl; 
                break;
        }
    }
};


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

    void print_deck()
    {
        cout << endl << "Deck size: " << cards.size() << endl;
        for(int i = cards.size() - 1; i > 0; i--)
        {
            cout << endl << "Card: ";
            switch(cards[i].num)
            {
                case 11:
                    cout << "jack ";
                    break;
                case 12:
                    cout << "queen ";
                    break;
                case 13:
                    cout << "king ";
                    break;
                case 0:
                    cout << "ace ";
                    break;
                default:
                    cout << cards[i].num << " ";
                    break;
            }
            switch(cards[i].suit)
            {
                case 0: 
                    cout << "SPADES" << endl;
                    break; 
                case 1:
                    cout << "CLUBS" << endl; 
                    break;
                case 2: 
                    cout << "HEARTS" << endl; 
                    break;
                case 3:
                    cout << "DIAMONDS" << endl; 
                    break;
            }
             
        }
    }
};

// globals
Card target_card; // target card draw by dealer
Deck global_deck; // deck accessable by all players
static uint64_t timeSeed; // time seed to set random number generator
int dealer_index; // global incremental flag to designate current dealer
bool winner; // global winner flag

// each player class has a id, two cards, their current hand size and their dealer/winner status
// each player can draw, discard, deal and check for a match
// they can set their dealer status & check for hand size and dealer/winner status
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
        else if (hand_size = 0)
            card_one = card;
        else if (hand_size = 1)
            card_two = card;

        hand_size++;
    }
    // player draws and returns card
    Card deal(Deck& deck) { return deck.top(); }
    // return card to the bottom of the deck
    // player recieves the deck and places the discarded card at the bottom of the deck
    // if r is 0 then card 1 is discarded, else card two is
    void discard(Deck& deck)
    {
        int r = rand() % FULL_HAND;
        if (r == 0)
        {
            deck.push(card_one);
            card_one.print_card();
            card_one = card_two;
        }
        else
        {
            deck.push(card_two);
            card_two.print_card();
        }
            
        
        hand_size--;
    }

    void return_hand(Deck& deck)
    {
        if (hand_size == 1)
            deck.push(card_one);
        else if(hand_size == 2)
        {
            deck.push(card_one);
            deck.push(card_two);
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
            cout << endl << "target card";
            target_card.print_card();
            cout << endl << "card_one";
            card_one.print_card();
            cout << endl << "card_two";
            card_two.print_card();
            winner = true;
            return true;
        }
        return false;
    }
    // functions to check win/deal status
    // and to check current hand size
    bool isWinner() { return winner; }
    bool isDealer() { return dealer; }
    int getHandSize() { return hand_size; }

    void print_hand()
    {
        if (hand_size == 1)
        {
            cout << endl << "card 1 ";
            switch(card_one.num)
            {
                case 11:
                    cout << "jack ";
                    break;
                case 12:
                    cout << "queen ";
                    break;
                case 13:
                    cout << "king ";
                    break;
                case 0:
                    cout << "ace ";
                    break;
                default:
                    cout << card_one.num << " ";
                    break;
            }
            switch(card_one.suit)
            {
                case 0: 
                    cout << "SPADES" << endl;
                    break; 
                case 1:
                    cout << "CLUBS" << endl; 
                    break;
                case 2: 
                    cout << "HEARTS" << endl; 
                    break;
                case 3:
                    cout << "DIAMONDS" << endl; 
                    break;
            }
        }
        else if (hand_size == 2)
        {
            cout << endl << "card 1 ";
            switch(card_one.num)
            {
                case 11:
                    cout << "jack ";
                    break;
                case 12:
                    cout << "queen ";
                    break;
                case 13:
                    cout << "king ";
                    break;
                case 0:
                    cout << "ace ";
                    break;
                default:
                    cout << card_one.num << " ";
                    break;
            }
            switch(card_one.suit)
            {
                case 0: 
                    cout << "SPADES" << endl;
                    break; 
                case 1:
                    cout << "CLUBS" << endl; 
                    break;
                case 2: 
                    cout << "HEARTS" << endl; 
                    break;
                case 3:
                    cout << "DIAMONDS" << endl; 
                    break;
            }
            cout << endl << "card 2 ";
            switch(card_two.num)
            {
                case 11:
                    cout << "jack ";
                    break;
                case 12:
                    cout << "queen ";
                    break;
                case 13:
                    cout << "king ";
                    break;
                case 0:
                    cout << "ace ";
                    break;
                default:
                    cout << card_two.num << " ";
                    break;
            }
            switch(card_two.suit)
            {
                case 0: 
                    cout << "SPADES" << endl;
                    break; 
                case 1:
                    cout << "CLUBS" << endl; 
                    break;
                case 2: 
                    cout << "HEARTS" << endl; 
                    break;
                case 3:
                    cout << "DIAMONDS" << endl; 
                    break;
            }
        } 
    }
};
// thread data
// has initial card before round start
struct thread_data
{
    int thread_id;
    Card card;
};

// function
// set timeSeed from epoch to now in ticks
// initializes the number generator
// sets global winner flag to false
void set_rng()
{
    timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    srand(timeSeed);
    winner = false;  
}

void *round_start(void *p_data)
{
    auto *arg = (thread_data *) p_data;
    Player player(arg -> thread_id, arg -> card);
    if (winner && !player.isWinner())
    {
        cout << endl << "player " << arg -> thread_id << " returned his hand" << endl;
        player.return_hand(global_deck);
        pthread_exit(NULL);
    }
    cout << endl << "player " << arg -> thread_id << " has entered round start" << endl;
    while (!winner)
    {
        pthread_mutex_lock(&p_mutex);

        cout << endl << "player " << arg -> thread_id << " drawing a card" << endl;
        player.draw(global_deck.top()); 
        global_deck.pop();

        if (player.isMatch())
        {
            winner = true;
            cout << endl << "player " << arg -> thread_id << " has won this round" << endl;
            cout << endl << "player " << arg -> thread_id << " returned his hand" << endl;
            player.return_hand(global_deck);
            pthread_cond_signal(&next_round);
        }
        else
        {
            cout << endl << "player " << arg -> thread_id << " has discarded a card" << endl;
            player.discard(global_deck);
        }
            
        
        pthread_mutex_unlock(&p_mutex);

        if (!player.isWinner())
        {
            cout << endl << "player " << arg -> thread_id << " is sleeping..." << endl;
            sleep(5); 
        } 
        
    }
    pthread_mutex_unlock(&p_mutex); 

    if (winner && !player.isWinner())
    {
        cout << endl << "player " << arg -> thread_id << " returned his hand" << endl;
        player.return_hand(global_deck);
    }
    pthread_exit(NULL);
}

// dealer enters function and waits for winning player
void *signal_next(void *p_data)
{
    // set p_data to thread_data structure
    auto *arg = (thread_data *) p_data;
    auto id = arg -> thread_id;
    cout << endl << "player " << arg -> thread_id << " has entered signal next" << endl;
    pthread_mutex_lock(&p_mutex); 
    cout << endl << "player " << arg -> thread_id << " is now waiting for a winner" << endl;
    while (!winner)
        pthread_cond_wait(&next_round, &p_mutex);
    
    pthread_mutex_unlock(&p_mutex);
    pthread_exit(NULL);
}

// main
// passed the number of strings (argc)
// the pointer to the strings (argv)
// the seed used to randomize
int main(int argc, char* argv[], uint64_t timeSeed)
{
    // initialize the deck
    global_deck = Deck();
    cout << endl << "deck created" << endl; 
    for (int round = 0; round < NUM_THREADS; ++round)
    {
        cout << endl << "round " << round + 1 << endl;
        set_rng(); // set/reset rng and winner flag
        cout << endl << "setting rng" << endl;
        dealer_index = round; // dealer set to player index
        cout << endl << "dealer " << dealer_index + 1 << endl;
        // create player thread data for current round
        thread_data pd[NUM_THREADS];
        // set all ids
        cout << endl << "creating threads" << endl;
        for (int i = 0; i < NUM_THREADS; ++i)
            pd[i].thread_id = i + 1;
        // shuffle the deck
        cout << endl << "shuffle deck" << endl;
        global_deck.shuffle();
        // global_deck.print_deck();
        // set target card
        cout << endl << "setting target card" << endl;
        target_card = global_deck.top();
        global_deck.pop(); 
        target_card.print_card();

        // deal the first round of cards
        for (int i = 0; i < NUM_THREADS; ++i)
        {
            if (dealer_index == i) // skip dealer
            {
                cout << endl << "player " << i + 1 << " is dealer...skipping" << endl;
                continue;
            }

            else // dealer deals card to player
            {
                // dealer draws and gives to player
                cout << endl << "player " << i + 1 << " dealt a card" << endl;
                pd[dealer_index].card = global_deck.top();
                global_deck.pop();
                pd[i].card = pd[dealer_index].card; 
            }
        }

        // initialize player mutex and next round condition signal
        cout << endl << "mutex and condition initialized" << endl;
        pthread_mutex_init(&p_mutex, nullptr);
        pthread_cond_init(&next_round, nullptr);
        
        // based on round determines who is dealer/round signaler
        cout << endl << "creating threads" << endl;
        switch(round) 
        {
            case 0:
                pthread_create(&p1, nullptr, signal_next, (void *) &pd[0]);
                pthread_create(&p2, nullptr, round_start, (void *) &pd[1]);
                sleep(1);
                pthread_create(&p3, nullptr, round_start, (void *) &pd[2]);
                sleep(1);
                pthread_create(&p4, nullptr, round_start, (void *) &pd[3]);
                sleep(1);
                pthread_create(&p5, nullptr, round_start, (void *) &pd[4]);
                sleep(1);
                pthread_create(&p6, nullptr, round_start, (void *) &pd[5]);
                break;
            case 1:
                pthread_create(&p2, nullptr, signal_next, (void *) &pd[1]);
                pthread_create(&p3, nullptr, round_start, (void *) &pd[2]);
                sleep(1);
                pthread_create(&p4, nullptr, round_start, (void *) &pd[3]);
                sleep(1);
                pthread_create(&p5, nullptr, round_start, (void *) &pd[4]);
                sleep(1);
                pthread_create(&p6, nullptr, round_start, (void *) &pd[5]);
                sleep(1);
                pthread_create(&p1, nullptr, round_start, (void *) &pd[0]);
                break;
            case 2:
                pthread_create(&p3, nullptr, signal_next, (void *) &pd[2]);
                pthread_create(&p4, nullptr, round_start, (void *) &pd[3]);
                sleep(1);
                pthread_create(&p5, nullptr, round_start, (void *) &pd[4]);
                sleep(1);
                pthread_create(&p6, nullptr, round_start, (void *) &pd[5]);
                sleep(1);
                pthread_create(&p1, nullptr, round_start, (void *) &pd[0]);
                sleep(1);
                pthread_create(&p2, nullptr, round_start, (void *) &pd[1]);
                break;
            case 3:
                pthread_create(&p4, nullptr, signal_next, (void *) &pd[3]);
                pthread_create(&p5, nullptr, round_start, (void *) &pd[4]);
                sleep(1);
                pthread_create(&p6, nullptr, round_start, (void *) &pd[5]);
                sleep(1);
                pthread_create(&p1, nullptr, round_start, (void *) &pd[0]);
                sleep(1);
                pthread_create(&p2, nullptr, round_start, (void *) &pd[1]);
                sleep(1);
                pthread_create(&p3, nullptr, round_start, (void *) &pd[2]);
                break;
            case 4:
                pthread_create(&p5, nullptr, signal_next, (void *) &pd[4]);
                pthread_create(&p6, nullptr, round_start, (void *) &pd[5]);
                sleep(1);
                pthread_create(&p1, nullptr, round_start, (void *) &pd[0]);
                sleep(1);
                pthread_create(&p2, nullptr, round_start, (void *) &pd[1]);
                sleep(1);
                pthread_create(&p3, nullptr, round_start, (void *) &pd[2]);
                sleep(1);
                pthread_create(&p4, nullptr, round_start, (void *) &pd[3]);
                sleep(1);
                break;
            case 5:
                pthread_create(&p6, nullptr, signal_next, (void *) &pd[5]);
                pthread_create(&p1, nullptr, round_start, (void *) &pd[0]);
                sleep(1);
                pthread_create(&p2, nullptr, round_start, (void *) &pd[1]);
                sleep(1);
                pthread_create(&p3, nullptr, round_start, (void *) &pd[2]);
                sleep(1);
                pthread_create(&p4, nullptr, round_start, (void *) &pd[3]);
                sleep(1);
                pthread_create(&p5, nullptr, round_start, (void *) &pd[4]);
                break;
        }
        // sleep(5);
        // cout << endl << "threads created. waiting to join threads" << endl;
        // rejoin all threads as they complete
        pthread_join(p1, nullptr);
        pthread_join(p2, nullptr);
        pthread_join(p3, nullptr);
        pthread_join(p4, nullptr);
        pthread_join(p5, nullptr);
        pthread_join(p6, nullptr);

        cout << endl << "target card has returned to the deck & all threads have been joined" << endl;
        global_deck.push(target_card); 
        cout << endl << "The size of the deck is " << global_deck.getSize() << endl; 
    }
    cout << endl << "cleaning up" << endl;
    pthread_mutex_destroy(&p_mutex);
    pthread_exit(nullptr);
}
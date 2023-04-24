#include <pthread.h>
#include <cstdlib>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <vector>
#include <iostream>
#include <functional>
#include <random>
#include <time.h>
#include "deck.h"
#include "players.h"
#include "logger.h"
#include "cards.h"

using namespace std;


#define NUM_THREADS 6 // number of players/rounds
pthread_t p1, p2, p3, p4, p5, p6; // initialize player threads
pthread_mutex_t p_mutex; // player wait/signal
pthread_cond_t next_round; // signal to initiate next round

Deck global_deck; // deck accessable by all players
static uint64_t timeSeed; // time seed to set random number generator
mt19937_64 rng;
uniform_real_distribution<double> unif(0.0, 1.0);
int dealer_index; // global incremental flag to designate current dealer
bool winner; // global winner flag
// ofstream log_file; 


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

    if (player.isMatch())
    {
        winner = true;
        log_file("PLAYER " + to_string(arg -> thread_id) + " has won!");
        cout << "PLAYER " + to_string(arg -> thread_id) + " has won!" << endl;
        cout << "Target " + target_card.print_card() << endl; 
        player.return_hand(global_deck);
        pthread_cond_signal(&next_round);
    }

    if (winner && !player.isWinner())
    {
        log_file("PLAYER " + to_string(arg -> thread_id) + " has lost");
        cout << "PLAYER " + to_string(arg -> thread_id) + " has lost" << endl;
        player.return_hand(global_deck);
        pthread_exit(NULL);
    }
    while (!winner)
    {
        pthread_mutex_lock(&p_mutex);
        player.draw(global_deck.top()); 
        global_deck.pop();

        if (player.isMatch())
        {
            winner = true;
            log_file("PLAYER " + to_string(arg -> thread_id) + " has won!");
            cout << "PLAYER " + to_string(arg -> thread_id) + " has won!" << endl;
            cout << "Target " + target_card.print_card() << endl; 
            player.return_hand(global_deck);
            pthread_cond_signal(&next_round);
        }
        else
        {
            player.discard(global_deck, unif, rng);
        }
            
        
        pthread_mutex_unlock(&p_mutex);

        if (!player.isWinner()) { sleep(5); } 
        
    }
    pthread_mutex_unlock(&p_mutex); 

    if (winner && !player.isWinner())
    {
        log_file("PLAYER " + to_string(arg -> thread_id) + " has lost");
        cout << "PLAYER " + to_string(arg -> thread_id) + " has lost" << endl;
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
    pthread_mutex_lock(&p_mutex); 
    while (!winner)
        pthread_cond_wait(&next_round, &p_mutex);
    
    pthread_mutex_unlock(&p_mutex);
    log_file("PLAYER " + to_string(arg -> thread_id) + ": round end");
    pthread_exit(NULL);
}

// main
// passed the number of strings (argc)
// the pointer to the strings (argv)
// the seed used to randomize
int main(int argc, char* argv[], uint64_t timeSeed)
{
    // initialize the deck
    
    reset(); 
    global_deck = Deck();
     
    seed_seq ss { uint32_t (timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
    rng.seed(ss);

    
    for (int round = 0; round < NUM_THREADS; ++round)
    {
        log_file("************************************************************************** Round " + to_string(round + 1) + 
                                " has started ************************************************************************** "); 
        set_rng(); 
        log_file("RNG HAS BEEN SET TO CURRENT TIME");
         
        dealer_index = round; // dealer set to player index
        // create player thread data for current round
        thread_data pd[NUM_THREADS];
        // set all ids;
        for (int i = 0; i < NUM_THREADS; ++i)
            pd[i].thread_id = i + 1;
        // shuffle the deck
        global_deck.shuffle();
        log_file(global_deck.print_deck());
        cout << endl << global_deck.print_deck() << endl << endl;
        // global_deck.print_deck();
        // set target card
        target_card = global_deck.top();
        global_deck.pop();
        log_file("PLAYER " + to_string(round + 1) + " is the dealer. Target " + target_card.print_card());
        cout <<  "PLAYER " + to_string(round + 1) + " is the dealer. Target " + target_card.print_card() << endl;
        // deal the first round of cards
        for (int i = 0; i < NUM_THREADS; ++i)
        {
            if (dealer_index == i) // skip dealer
                continue;
            else // dealer deals card to player
            {
                // dealer draws and gives to player
                log_file("PLAYER " + to_string(i + 1) + " has " + global_deck.top().print_card());
                cout << "PLAYER " + to_string(i + 1) + " has " + global_deck.top().print_card() << endl; 
                pd[dealer_index].card = global_deck.top();
                global_deck.pop();
                pd[i].card = pd[dealer_index].card;  
            }
        }

        // initialize player mutex and next round condition signal
        pthread_mutex_init(&p_mutex, nullptr);
        pthread_cond_init(&next_round, nullptr);
        
        // based on round determines who is dealer/round signaler
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

        global_deck.push(target_card); 
    }
    pthread_mutex_destroy(&p_mutex);
    pthread_exit(nullptr);
}
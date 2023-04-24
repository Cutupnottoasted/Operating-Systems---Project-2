#ifndef CARDS_H
#define CARDS_H
using namespace std;
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

    string print_card()
    {
        string temp;
        temp += "Card: ";
        switch(num)
        {
            case 11:
                temp += "Jack ";
                break;
            case 12:
                temp += "Queen ";
                break;
            case 13:
                temp += "King ";
                break;
            case 0:
                temp += "Ace ";
                break;
            default:
                temp += to_string(num) + " ";
                break;
        }
        switch(suit)
        {
            case 0: 
                temp += "of Spades";
                break; 
            case 1:
                temp += "of Clubs"; 
                break;
            case 2: 
                temp += "of Hearts"; 
                break;
            case 3:
                temp += "of Diamonds"; 
                break;
        }
        return temp;
    }

};

#endif
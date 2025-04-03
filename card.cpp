#include "card.hpp"

Card::Card(Suit suit, int rank) : suit(suit), rank(rank) {}

Suit Card::getSuit() const {
    return suit;
}

int Card::getRank() const {
    return rank;
}

QString Card::getCardName() const {
    QString suitStr;
    switch(suit) {
        case Hearts:   suitStr = "hearts"; break;
        case Diamonds: suitStr = "diamonds"; break;
        case Clubs:    suitStr = "clubs"; break;
        case Spades:   suitStr = "spades"; break;
    }
    QString rankStr;
    if(rank == 1) rankStr = "ace";
    else if(rank == 11) rankStr = "jack";
    else if(rank == 12) rankStr = "queen";
    else if(rank == 13) rankStr = "king";
    else rankStr = QString::number(rank);
    return rankStr + "_of_" + suitStr;
}

QPixmap Card::getCardImage() const {
    return image;
}

void Card::setCardImage(const QPixmap &pixmap) {
    image = pixmap;
}

#ifndef CARD_H
#define CARD_H

#include <QString>
#include <QPixmap>

enum Suit {
    Hearts, Diamonds, Clubs, Spades
};

class Card {
public:
    Card(Suit suit, int rank);
    Suit getSuit() const;
    int getRank() const;
    QString getCardName() const;
    QPixmap getCardImage() const;
    void setCardImage(const QPixmap &pixmap);
private:
    Suit suit;
    int rank; // 1 (Ace) to 13 (King)
    QPixmap image;
};

#endif // CARD_H

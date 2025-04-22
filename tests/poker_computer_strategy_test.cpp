#include <gtest/gtest.h>
#include "../player.hpp"
#include "../card.hpp"
#include "../computer_strategy.hpp"


//class ComputerStrategyTest : public ::testing::Test{};

TEST(ComputerStrategyTest, flush_straight){
    
    // All the cards and Round to determine the next move {call,fold, raise}
    ComputerPlayer p(Difficulty::Easy);
    
    const Card *c1 = Card::getCard(Suit::Diamonds, Rank::Ace).get();
    const Card *c2 = Card::getCard(Suit::Diamonds, Rank::King).get();
    const Card *c3 = Card::getCard(Suit::Diamonds, Rank::Queen).get();
    const Card *c4 = Card::getCard(Suit::Diamonds, Rank::Ten).get();
    const Card *c5 = Card::getCard(Suit::Diamonds, Rank::Jack).get();

    std::vector<const Card*> cards = {c1,c2,c3,c4,c5};
    
    std::map<Rank,int> rankCount;
    std::map<Suit,int> suitCount;
    std::vector<int> ranks;

    for (const auto& c : cards){
        rankCount[c->getRank()]++;
        suitCount[c->getSuit()]++;
        ranks.push_back(static_cast<int>(c->getRank()));
    }

    std::sort(ranks.begin(),ranks.end());

    bool isFlush = false;

    for (auto& p : suitCount){
        if (p.second >= 5){
            isFlush = true;
            break;
        }
    }

    bool isStraight = false;

    int consec = 1;
    for (size_t i =1; i < ranks.size() ; i++){
        if (ranks[i-1] + 1 == ranks[i]) consec++;
        else if(ranks[i] != ranks[i-1]) consec = 1;
        if (consec >= 5){
            isStraight = true;
            break;
        }

    }
    
    bool four = false, three = false,  pair = false, two_pair = false;

    int pairCount = 0;

    for (const auto& r : rankCount){
        if (r.second == 4) four = true;
        if (r.second == 3) three = true;
        if (r.second == 2){
            pairCount++;
            if (pairCount == 2){
                two_pair = true;
            }
            pair = true;
        } 
    }


    EXPECT_EQ(isStraight,true);
    EXPECT_EQ(isFlush,true);
    EXPECT_EQ(four,false);
    EXPECT_EQ(three,false);
    EXPECT_EQ(pair,false);
    EXPECT_EQ(two_pair,false);

}


TEST(ComputerStrategyTest, fourofkind){
    
    // All the cards and Round to determine the next move {call,fold, raise}
    ComputerPlayer p(Difficulty::Easy);
    
    const Card *c1 = Card::getCard(Suit::Diamonds, Rank::Ace).get();
    const Card *c2 = Card::getCard(Suit::Clubs, Rank::Ace).get();
    const Card *c3 = Card::getCard(Suit::Hearts, Rank::Ace).get();
    const Card *c4 = Card::getCard(Suit::Spades, Rank::Ace).get();
    const Card *c5 = Card::getCard(Suit::Diamonds, Rank::Jack).get();

    std::vector<const Card*> cards = {c1,c2,c3,c4,c5};
    
    std::map<Rank,int> rankCount;
    std::map<Suit,int> suitCount;
    std::vector<int> ranks;

    for (const auto& c : cards){
        rankCount[c->getRank()]++;
        suitCount[c->getSuit()]++;
        ranks.push_back(static_cast<int>(c->getRank()));
    }

    std::sort(ranks.begin(),ranks.end());

    bool isFlush = false;

    for (auto& p : suitCount){
        if (p.second >= 5){
            isFlush = true;
            break;
        }
    }

    bool isStraight = false;

    int consec = 1;
    for (size_t i =1; i < ranks.size() ; i++){
        if (ranks[i-1] + 1 == ranks[i]) consec++;
        else if(ranks[i] != ranks[i-1]) consec = 1;
        if (consec >= 5){
            isStraight = true;
            break;
        }

    }
    
    bool four = false, three = false,  pair = false, two_pair = false;

    int pairCount = 0;

    for (const auto& r : rankCount){
        if (r.second == 4) four = true;
        if (r.second == 3) three = true;
        if (r.second == 2){
            pairCount++;
            if (pairCount == 2){
                two_pair = true;
            }
            pair = true;
        } 
    }


    EXPECT_EQ(isStraight,false);
    EXPECT_EQ(isFlush,false);
    EXPECT_EQ(four,true);
    EXPECT_EQ(three,false);
    EXPECT_EQ(pair,false);
    EXPECT_EQ(two_pair,false);

}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

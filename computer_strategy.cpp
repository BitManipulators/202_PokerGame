#include "computer_strategy.hpp"
#include <random>


int getRandomInt(std::size_t min, std::size_t max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

Move EasyStrategy::getNextMove(GameState current_state){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0,1);
    
    int random_number = distr(gen);
    switch (random_number){
        case 0 :
            return Call{};
        default :
            std::size_t bet = current_state.current_bet;
            return Raise{2*bet};
    }
}

Move MediumStrategy::getNextMove(GameState current_state){
    
    int strength = evaluateHandStrength(current_state.hands, current_state.community_cards, current_state.stage);
    std::size_t bet = current_state.current_bet;
    if (bet == 0) {
        bet = 10;
    }
    
    if (strength >= 80) {
       
        int raiseChance = getRandomInt(0, 100);
        if (raiseChance < 90) { 
            return Raise{2 * bet};
        } else {
            return Call{}; 
        }
    }
    
    if (strength >= 40) {
        
        int callRaiseChance = getRandomInt(0, 100);
        if (callRaiseChance < 70) { 
            return Call{};
        } else { 
            return Raise{2 * bet};
        }
    }
    
    
    int foldChance = getRandomInt(0, 100);
    if (foldChance < 60) { 
        return Fold{};
    } else {
        return Call{}; 
    }
}

int MediumStrategy::evaluateHandStrength(const std::vector<const Card*>& hand,
                         const std::vector<const Card*>& community,
                         PokerEngineEnumState stage) {
    std::vector<const Card*> all = hand;
    all.insert(all.end(), community.begin(), community.end());

    std::map<Rank, int> rankCounts = getRankCounts(all);
    std::set<int> uniqueRanks;
    for (auto* card : all)
        uniqueRanks.insert((int)card->getRank());

    int score = 0;

    switch (stage) {
        case PokerEngineEnumState::PreFlop:
            
            //std::cout << "Preflop" << std::endl;
            return evaluatePreflop(hand);  // returns 0-100 scale

        case PokerEngineEnumState::Flop:
        case PokerEngineEnumState::Turn:
        case PokerEngineEnumState::River: {
            bool pair = false, twoPair = false, trips = false, straight = false,
                 flush = false, fullHouse = false, quads = false;

            int pairs = 0;
            for (auto& [rank, count] : rankCounts) {
                if (count == 2) pairs++;
                else if (count == 3) trips = true;
                else if (count == 4) quads = true;
            }

            if (pairs >= 2) twoPair = true;
            if (pairs == 1) pair = true;
            if (trips && pairs) fullHouse = true;
            flush = hasFlush(all);
            straight = hasStraight(uniqueRanks);

            // Score based on hand ranking
            if (quads) score = 90;
            else if (fullHouse) score = 80;
            else if (flush) score = 70;
            else if (straight) score = 65;
            else if (trips) score = 50;
            else if (twoPair) score = 40;
            else if (pair) score = 30;
            else score = 15; // high card

            // Bonus: high card contribution
            int highCard = std::max((int)hand[0]->getRank(), (int)hand[1]->getRank());
            score += highCard / 10;

            return std::min(score, 100); // cap at 100
        }
    }

    return 0;
}


// Helper: Check if suited
bool MediumStrategy::isSuited(const std::vector<const Card*>& hand) {
    return hand[0]->getSuit() == hand[1]->getSuit();
}

// Helper: Preflop strength (0 - 100)
int MediumStrategy::evaluatePreflop(const std::vector<const Card*>& hand) {
    
    int hand1 = hand[0]->getValue();
    int hand2 = hand[0]->getValue();
    int high = std::max(hand1, hand2);
    bool ispair = hand1 == hand2;
    
    if(ispair){
        if (high >= 10){
            return 90;
        }else if(high >= 7){
            return 70;
        }
    }

    bool is_suited = isSuited(hand);

    if(is_suited ){
        if(abs(hand1-hand2) == 1 && (hand1>=10 || hand2>=10)){
            return 90;
        }
        else if( hand1>=9 || hand2 >= 9 ){
            return 80;
        }
       
    }

    if(high > (int)Rank::Jack){
        return 40;
    }

    return 30;
    
}

// Helper: Count ranks
std::map<Rank, int> MediumStrategy::getRankCounts(const std::vector<const Card*>& cards) {
    std::map<Rank, int> counts;
    for (auto* card : cards)
        counts[card->getRank()]++;
    return counts;
}

// Helper: Check for flush
bool MediumStrategy::hasFlush(const std::vector<const Card*>& cards) {
    std::map<Suit, int> suitCount;
    for (auto* card : cards)
        suitCount[card->getSuit()]++;
    for (auto& [suit, count] : suitCount)
        if (count >= 5) return true;
    return false;
}

// Helper: Check for straight
bool MediumStrategy::hasStraight(const std::set<int>& ranksSet) {
    std::vector<int> ranks(ranksSet.begin(), ranksSet.end());

    // Ace can be low (1) or high (14)
    if (ranksSet.count((int)Rank::Ace)) ranks.insert(ranks.begin(), 1);

    for (size_t i = 0; i <= ranks.size() - 5; ++i) {
        bool straight = true;
        for (int j = 1; j < 5; ++j)
            if (ranks[i + j] != ranks[i] + j)
                straight = false;
        if (straight) return true;
    }
    return false;
}

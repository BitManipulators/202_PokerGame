#include "computer_strategy.hpp"
#include "poker_hand_evaluator.hpp"

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
    switch (stage) {
        case PokerEngineEnumState::PreFlop:
            
            //std::cout << "Preflop" << std::endl;
            return evaluatePreflop(hand);  // returns 0-100 scale

        case PokerEngineEnumState::Flop:
        case PokerEngineEnumState::Turn:
        case PokerEngineEnumState::River: {
            PokerHandEvaluation poker_hand_evaluation = PokerHandEvaluator::evaluate_hand(hand, community);

            int score = 0;
            switch(poker_hand_evaluation.category) {
            case HighCard:
                score = 15;
                break;
            case OnePair:
                score = 30;
                break;
            case TwoPair:
                score = 40;
                break;
            case ThreeOfAKind:
                score = 50;
                break;
            case Straight:
                score = 65;
                break;
            case Flush:
                score = 70;
                break;
            case FullHouse:
                score = 80;
                break;
            case FourOfAKind:
                score = 90;
                break;
            case StraightFlush:
                score = 95;
                break;
            case RoyalFlush:
                score = 100;
                break;
            case Unknown:
                score = 0;
                break;
            }

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

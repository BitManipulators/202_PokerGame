#include "computer_strategy.hpp"
#include "poker_hand_evaluator.hpp"

#include <random>


int get_random_int(std::size_t min, std::size_t max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

Move EasyStrategy::get_next_move(GameState current_state) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0,1);

    int random_number = distr(gen);
    switch (random_number){
        case 0 :
            return Call{};
        default :
            std::size_t bet = current_state.current_bet;
            std::size_t raiseAmount = std::max<std::size_t>(2 * bet, 10); // Minimum raise of $10
            return Raise{raiseAmount};
    }
}

Move MediumStrategy::get_next_move(GameState current_state) {

    int strength = evaluate_hand_strength(current_state.hands, current_state.community_cards, current_state.stage);
    std::size_t bet = current_state.current_bet;
    if (bet == 0) {
        bet = 10;
    }

    if (strength >= 80) {

        int raiseChance = get_random_int(0, 100);
        if (raiseChance < 90) {
            return Raise{2 * bet};
        } else {
            return Call{};
        }
    }

    if (strength >= 40) {

        int callRaiseChance = get_random_int(0, 100);
        if (callRaiseChance < 70) {
            return Call{};
        } else {
            return Raise{2 * bet};
        }
    }

    if (bet <= 10) {
        // Small blind or low bets don't fold immediately
        return Call{};
    }

    int foldChance = get_random_int(0, 100);
    if (foldChance < 30) {
        return Fold{};
    } else {
        return Call{};
    }
}

int MediumStrategy::evaluate_hand_strength(const std::vector<const Card*>& hand,
                         const std::vector<const Card*>& community,
                         PokerEngineEnumState stage) {
    switch (stage) {
        case PokerEngineEnumState::PreFlop:

            //std::cout << "Preflop" << std::endl;
            return evaluate_preflop(hand);  // returns 0-100 scale

        case PokerEngineEnumState::Flop:
        case PokerEngineEnumState::Turn:
        case PokerEngineEnumState::River: {
            auto [poker_hand, poker_hand_evaluation] = PokerHandEvaluator::evaluate_hand(hand, community);

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
            int highCard = std::max((int)hand[0]->get_rank(), (int)hand[1]->get_rank());
            score += highCard / 10;

            return std::min(score, 100); // cap at 100
        }

        case PokerEngineEnumState::Showdown:
        case PokerEngineEnumState::Folded:
            // During showdown or folded state hand strength evaluation may not matter
            return 0;
    }

    return 0;
}


// Helper: Check if suited
bool MediumStrategy::is_suited(const std::vector<const Card*>& hand) {
    return hand[0]->get_suit() == hand[1]->get_suit();
}

// Helper: Preflop strength (0 - 100)
int MediumStrategy::evaluate_preflop(const std::vector<const Card*>& hand) {

    int hand1 = hand[0]->get_value();
    int hand2 = hand[1]->get_value();
    int high = std::max(hand1, hand2);
    bool is_pair = hand1 == hand2;

    if (is_pair) {
        if (high >= 10){
            return 90;
        }else if(high >= 7){
            return 70;
        }
    }

    bool cards_suited = is_suited(hand);

    if (cards_suited) {
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

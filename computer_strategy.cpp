#include "computer_strategy.hpp"
#include "poker_hand_evaluator.hpp"

#include <random>
#include <algorithm>



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

Move MediumStrategy::getNextMove(GameState current_state) {
    int strength = evaluateHandStrength(current_state.hands, current_state.community_cards, current_state.stage);
    std::size_t bet = current_state.current_bet;
    std::size_t pot = current_state.pot_size;
    std::size_t computer_chips = current_state.computer_chips;

    if (bet == 0) {
        bet = 20;  // Small starting bet if no bet has been placed
    }
    
    std::size_t raise_amount = MINIMUM_BET_MULTIPLIER * bet;  // Minimum raise is always 2x the current bet
    std::size_t pot_portion = calculatePotPortion(strength, pot);

    // Add the pot portion to the raise amount
    raise_amount += pot_portion;

    if (computer_chips < MINIMUM_BET_MULTIPLIER * bet) {
        return handleLowChipCount(strength);
    }

    return handleNormalBetting(strength, raise_amount, current_state.current_bet);
}

std::size_t MediumStrategy::calculatePotPortion(int strength, std::size_t pot) {
    std::size_t pot_portion = 0;
    if (strength >= HIGH_HAND_THRESHOLD) {
        pot_portion = static_cast<std::size_t>(pot * 0.5);
    } else if (strength >= MEDIUM_HAND_THRESHOLD) {
        pot_portion = static_cast<std::size_t>(pot * 0.3);
    } else if (strength >= WEAK_HAND_THRESHOLD) {
        pot_portion = static_cast<std::size_t>(pot * 0.2);
    }
    // Round the pot portion to the nearest multiple of 10
    return (pot_portion / 10) * 10;
}

Move MediumStrategy::handleLowChipCount(int strength) {
    if (strength >= MEDIUM_HAND_THRESHOLD) {
        return Call{};  // If hand is strong, prefer calling
    } else {
        int fold_chance = getRandomInt(0, 100);
        if (fold_chance < FOLD_CHANCE_LOW_HAND_LOW_BET) {  // 50% chance to fold if hand strength is lower
            return Fold{};
        } else {
            return Call{};
        }
    }
}

Move MediumStrategy::handleNormalBetting(int strength, std::size_t raise_amount, std::size_t current_bet) {
    if (strength >= HIGH_HAND_THRESHOLD) {
        return Raise{raise_amount};
    } else if (strength >= MEDIUM_HAND_THRESHOLD) {
        return Raise{raise_amount};
    } else if (strength >= WEAK_HAND_THRESHOLD) {
        int call_raise_chance = getRandomInt(0, 100);
        if (call_raise_chance < CALL_CHANCE_WEAKER_HAND) {
            return Call{};  // Call more often with weaker hands
        } else {
            return Raise{raise_amount};
        }
    }

    // When hand strength is low, make decisions based on betting behavior
    if (current_bet <= 60) {
        return Call{};  // If the bet is small, do not fold immediately
    }

    int fold_chance = getRandomInt(0, 100);
    if (fold_chance < FOLD_CHANCE_LOW_HAND) {
        return Fold{};  // Low hand strength and large bet: Consider folding
    } else {
        return Call{};  // Otherwise, call the bet
    }
}

int MediumStrategy::evaluateHandStrength(const std::vector<const Card*>& hand,
                                         const std::vector<const Card*>& community,
                                         PokerEngineEnumState stage) {
    switch (stage) {
        case PokerEngineEnumState::PreFlop:
            return evaluatePreflop(hand);  // returns 0-100 scale

        case PokerEngineEnumState::Flop:
        case PokerEngineEnumState::Turn:
        case PokerEngineEnumState::River: {
            PokerHandEvaluation poker_hand_evaluation = PokerHandEvaluator::evaluate_hand(hand, community);
            int score = getHandCategoryScore(poker_hand_evaluation.category);
            int high_card = std::max((int)hand[0]->getRank(), (int)hand[1]->getRank());
            score += high_card / 10;
            return std::min(score, 100); // cap at 100
        }

        case PokerEngineEnumState::Showdown:
        case PokerEngineEnumState::Folded:
            return 0; // Hand strength evaluation doesn't matter during showdown or folded state
    }

    return 0;
}

int MediumStrategy::getHandCategoryScore(PokerHandEvaluationCategory category) {
    switch (category) {
        case HighCard: return 15;
        case OnePair: return 30;
        case TwoPair: return 40;
        case ThreeOfAKind: return 50;
        case Straight: return 65;
        case Flush: return 70;
        case FullHouse: return 80;
        case FourOfAKind: return 90;
        case StraightFlush: return 95;
        case RoyalFlush: return 100;
        case Unknown: return 0;
    }
    return 0;
}

bool MediumStrategy::isSuited(const std::vector<const Card*>& hand) {
    return hand[0]->getSuit() == hand[1]->getSuit();
}

int MediumStrategy::evaluatePreflop(const std::vector<const Card*>& hand) {
    int hand1 = hand[0]->getValue();
    int hand2 = hand[1]->getValue();
    int high = std::max(hand1, hand2);
    bool ispair = hand1 == hand2;

    if (ispair) {
        if (high >= STRONG_PAIR_THRESHOLD) {
            return 90;
        } else if (high >= 7) {
            return 70;
        }
    }

    bool is_suited = isSuited(hand);

    if (is_suited) {
        if (std::abs(hand1 - hand2) == 1 && (hand1 >= STRONG_SUITED_THRESHOLD || hand2 >= STRONG_SUITED_THRESHOLD)) {
            return 90;
        } else if (hand1 >= 9 || hand2 >= 9) {
            return 80;
        }
    }

    if (high > (int)Rank::Jack) {
        return 40;
    }

    return 30;
}
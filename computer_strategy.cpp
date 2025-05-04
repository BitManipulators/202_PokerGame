#include "computer_strategy.hpp"
#include "poker_hand_evaluator.hpp"

#include <iostream>
#include <random>
#include <algorithm>
#include <vector>

#include <boost/program_options.hpp>
#include <pokerstove/penum/ShowdownEnumerator.h>

//namespace po = boost::program_options;


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

// Convert hand (vector of Card pointers) into a vector of strings in the expected format
std::vector<std::string> convertHandToStringFormat(const std::vector<const Card*>& hand) {
    std::vector<std::string> handStrings;

    for (const auto& card : hand) {
        if (card == nullptr) {
            throw std::invalid_argument("Null card detected in hand");
        }

        std::ostringstream cardStr;

        // Convert the Rank to string
        std::string rankStr;
        switch (card->getRank()) {
            case Rank::Two:    rankStr = "2"; break;
            case Rank::Three:  rankStr = "3"; break;
            case Rank::Four:   rankStr = "4"; break;
            case Rank::Five:   rankStr = "5"; break;
            case Rank::Six:    rankStr = "6"; break;
            case Rank::Seven:  rankStr = "7"; break;
            case Rank::Eight:  rankStr = "8"; break;
            case Rank::Nine:   rankStr = "9"; break;
            case Rank::Ten:    rankStr = "T"; break;
            case Rank::Jack:   rankStr = "J"; break;
            case Rank::Queen:  rankStr = "Q"; break;
            case Rank::King:   rankStr = "K"; break;
            case Rank::Ace:    rankStr = "A"; break;
            default: throw std::invalid_argument("Invalid rank");
        }

        // Convert the Suit to string
        std::string suitStr;
        switch (card->getSuit()) {
            case Suit::Hearts:   suitStr = "h"; break;
            case Suit::Diamonds: suitStr = "d"; break;
            case Suit::Clubs:    suitStr = "c"; break;
            case Suit::Spades:   suitStr = "s"; break;
            default: throw std::invalid_argument("Invalid suit");
        }

        // Combine Rank and Suit
        cardStr << rankStr << suitStr;
        handStrings.push_back(cardStr.str());
    }

    return handStrings;
}

// HardStrategy's getNextMove function
Move HardStrategy::getNextMove(GameState current_state) {

    std::cout << "Hard Strategy !!!" << std::endl;
    
    // Hardcoded values (You might want to adjust these later)
    std::string game = "h";  // Example game type
    
    // Convert hand and board to the expected string format
    std::vector<std::string> hands = convertHandToStringFormat(current_state.hands);
    std::vector<std::string> board_card = convertHandToStringFormat(current_state.community_cards);

    if(board_card.size() == 0 ){
        return Call{};
    }

    std::cout << "Conversion Done" << std::endl;

    // Concatenate board cards into a single string
    std::string board;
    for (size_t i = 0; i < board_card.size(); ++i) {
        board += board_card[i];
    }

    std::cout << "Board is: " << board << std::endl;

   

    // Allocate evaluator and create card distributions
    std::shared_ptr<pokerstove::PokerHandEvaluator> evaluator = pokerstove::PokerHandEvaluator::alloc(game);
    std::vector<pokerstove::CardDistribution> handDists;

    // Check if the hand is valid and prepare card distributions
    if (hands.empty()) {
        throw std::invalid_argument("No hands provided for evaluation");
    }

    for (const std::string& hand : hands) {
        handDists.emplace_back();
        handDists.back().parse(hand);
    }

    // Fill with random if necessary (e.g., for multi-hand evaluations)
    if (handDists.size() == 1) {
        handDists.emplace_back();
        handDists.back().fill(evaluator->handSize());
    }

    // Calculate equity results
    pokerstove::ShowdownEnumerator showdown;
    std::vector<pokerstove::EquityResult> results =
        showdown.calculateEquity(handDists, pokerstove::CardSet(board), evaluator);

    double total = 0.0;
    for (const pokerstove::EquityResult& result : results) {
        total += result.winShares + result.tieShares;
    }

    // Calculate equity for the first hand
    double equity = 0.0;
    if (!results.empty()) {
        equity = (results[0].winShares + results[0].tieShares) / total;
    }

    
    std::cout << "The hand " << hands[0] << " has " << equity * 100.0 << " % equity." << std::endl;
    

    // Betting decision based on equity
    std::size_t bet = current_state.current_bet;
    std::size_t pot = current_state.pot_size;
    std::size_t computer_chips = current_state.computer_chips;

    std::size_t raise_amount = 2 * bet;  // Minimum raise is always 2x the current bet

    // Strong hand decision (high equity)
    if (equity >= 0.80) {
        
        return Raise{raise_amount};
    }
    // Moderate hand decision (medium equity)
    else if (equity >= 0.50) {
        
        if (computer_chips > 2 * bet) {
            return Raise{raise_amount};  
        } else {
            return Call{};  
        }
    }
    // Weak hand decision (low equity)
    else if (equity >= 0.30) {
       
        if (bet <= 60) {
            return Call{};  // If the bet is small, call
        } else {
            // Fold if bet is large and hand is weak
            return Fold{};
        }
    }
    // Very weak hand decision (low equity)
    else {
        
        // Very weak hand: fold or call with a small chance
        int foldChance = getRandomInt(0, 100);
        if (foldChance < 70) {
            return Fold{};
        } else {
            return Call{};
        }
    }
}
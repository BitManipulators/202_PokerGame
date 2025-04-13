#pragma once

#include "poker_hand_evaluation.hpp"

class PokerHand;

class PokerHandVisitor {
public:
    virtual ~PokerHandVisitor() = default;
    virtual const PokerHandEvaluation visit(const PokerHand* poker_hand) const = 0;
};

class StraightFlushVisitor : public PokerHandVisitor {
    const PokerHandEvaluation visit(const PokerHand* poker_hand) const override;
};

class FourOfAKindVisitor : public PokerHandVisitor {
public:
    FourOfAKindVisitor(const std::vector<std::pair<int,int>>& card_frequencies);
    const PokerHandEvaluation visit(const PokerHand* poker_hand) const override;

private:
    const std::vector<std::pair<int,int>>& card_frequencies_;
};

class FullHouseVisitor : public PokerHandVisitor {
public:
    FullHouseVisitor(const std::vector<std::pair<int,int>>& card_frequencies);
    const PokerHandEvaluation visit(const PokerHand* poker_hand) const override;

private:
    const std::vector<std::pair<int,int>>& card_frequencies_;
};

class FlushVisitor : public PokerHandVisitor {
    const PokerHandEvaluation visit(const PokerHand* poker_hand) const override;
};

class StraightVisitor : public PokerHandVisitor {
    const PokerHandEvaluation visit(const PokerHand* poker_hand) const override;
};

class ThreeOfAKindVisitor : public PokerHandVisitor {
public:
    ThreeOfAKindVisitor(const std::vector<std::pair<int,int>>& card_frequencies);
    const PokerHandEvaluation visit(const PokerHand* poker_hand) const override;

private:
    const std::vector<std::pair<int,int>>& card_frequencies_;
};

class TwoPairVisitor : public PokerHandVisitor {
public:
    TwoPairVisitor(const std::vector<std::pair<int,int>>& card_frequencies);
    const PokerHandEvaluation visit(const PokerHand* poker_hand) const override;

private:
    const std::vector<std::pair<int,int>>& card_frequencies_;
};

class OnePairVisitor : public PokerHandVisitor {
public:
    OnePairVisitor(const std::vector<std::pair<int,int>>& card_frequencies);
    const PokerHandEvaluation visit(const PokerHand* poker_hand) const override;

private:
    const std::vector<std::pair<int,int>>& card_frequencies_;
};

class HighCardVisitor : public PokerHandVisitor {
    const PokerHandEvaluation visit(const PokerHand* poker_hand) const override;
};

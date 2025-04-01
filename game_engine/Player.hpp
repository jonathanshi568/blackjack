#pragma once

#include "util.hpp"

namespace jshi_blackjack {

class IPlayer {
public:
    IPlayer(const std::string& name, double bankroll) : name_{name}, bankroll_{bankroll} {}
    virtual ~IPlayer() = default;

    virtual Action action(const Hand& playerHand, const Card& dealerCard) = 0;
    virtual void notifyHandFinal(const Hand& hand) {}

    virtual double getBetAmount() = 0;
    virtual inline void collect(double amount) { bankroll_ += amount; }
    virtual inline void bet(double amount) { bankroll_ -= amount; }

    virtual bool buyInsurance(double cost) { return false; }

    double bankroll() const { return bankroll_; }
    std::string name() const { return name_; }

protected:
    std::string name_;
    double bankroll_;

};

class InteractivePlayer : public IPlayer {
public:
    InteractivePlayer(const std::string& name, double bankroll) : IPlayer(name, bankroll) {}
    virtual ~InteractivePlayer() = default;

    Action action(const Hand& playerHand, const Card& dealerCard) final;
    double getBetAmount() final;
    void notifyHandFinal(const Hand& hand) final;
    bool buyInsurance(double cost) final;
private:
};

// class BasicStrategyPlayer : public IPlayer {
// public:
//     Action action(const Hand& playerHand, const Hand& dealerHand) final;

// private:
// };


} // namespace jshi_blackjack
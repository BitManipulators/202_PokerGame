# Poker GUI Project

This project is a poker game GUI built using Qt, with various components such as game logic, poker hand evaluation, and computer strategy.

## Requirements

- **CMake** >= 3.16
- **Qt** (version 5 or 6) for the GUI components
- **[Boost](https://github.com/boostorg/boost)** library for general utility
- **[PokerStove](https://github.com/andrewprock/pokerstove)** library for poker hand evaluation

## Features

- Poker game logic and mechanics
- Poker hand evaluation using PokerStove
- Computer AI to play poker hands

## Dependencies

### Boost

## Mac
- brew install boost

## Linux

- sudo apt update
- sudo apt install libboost-all-dev


### PokerStove

PokerStove is used for poker hand evaluation and is fetched directly from its **[GitHub repository](https://github.com/andrewprock/pokerstove)**. It is linked into the project for hand evaluation computations.

## Steps to Install

```bash
mkdir build && cd build

cmake ..

make
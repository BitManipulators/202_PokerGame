#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "pokergame.hpp"
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    // Initialize game variables
    pot = 0;
    currentBet = 0;
    player1Chips = 1000; // Starting chips
    player2Chips = 1000;
    smallBlind = 5;
    bigBlind = 10;
    player1CurrentBet = 0;
    player2CurrentBet = 0;
    bettingRoundComplete = false;
    isPlayer1Dealer = true;

    // Landing page: Connect start new game button.
    connect(ui->startNewGameButton, &QPushButton::clicked, this, &MainWindow::onStartNewGame);

    // Game page: Connect buttons.
    connect(ui->newGameButton, &QPushButton::clicked, this, &MainWindow::onNewGame);
   // connect(ui->dealButton, &QPushButton::clicked, this, &MainWindow::onDeal);
    connect(ui->determineWinnerButton, &QPushButton::clicked, this, &MainWindow::onDetermineWinner);
    connect(ui->placeBetButton, &QPushButton::clicked, this, &MainWindow::onRaise);
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWindow::close);
    connect(ui->foldButton, &QPushButton::clicked, this, &MainWindow::onFold);
    connect(ui->callButton, &QPushButton::clicked, this, &MainWindow::onCall);

    // Start with landing page
    ui->stackedWidget->setCurrentIndex(0);

    // Update UI to show chips and pot
    updateChipDisplay();
}

MainWindow::~MainWindow() {
    delete ui;
}

// Update chip displays
void MainWindow::updateChipDisplay() {
    // Use existing labels instead of the ones that don't exist
    ui->player1Label->setText(QString("Player 1: %1 chips").arg(player1Chips));
    ui->player2Label->setText(QString("Player 2: %1 chips").arg(player2Chips));
    ui->betLabel->setText(QString("Pot: %1 | Current Bet: %2").arg(pot).arg(currentBet));
}

// Slot to handle landing page "Start New Game" click.
void MainWindow::onStartNewGame() {
    // Switch to game page.
    ui->stackedWidget->setCurrentIndex(1);
    qApp->processEvents();
    onNewGame();  // Start a new game.
    ui->player2Label->raise();
}

// Starts a new game and updates button states.
void MainWindow::onNewGame() {
    game.startNewGame();

    // Post blinds and setup initial game state
    postBlinds();

    displayGame();

    // Disable New Game button after game starts.
    ui->newGameButton->setEnabled(false);

    // Set the initial stage
    currentStage = GameStage::PreFlop;

    // Set bettingRoundComplete to false
    bettingRoundComplete = false;

    // Update UI to enable appropriate buttons
    updateUIForStage();

    ui->player2Label->raise();
}

// Post blinds at the start of a hand
void MainWindow::postBlinds() {
    // Reset pot and bets
    pot = 0;
    currentBet = 0;
    player1CurrentBet = 0;
    player2CurrentBet = 0;
    bettingRoundComplete = false;

    // Post blinds based on dealer position
    if (game.isPlayer1Dealer()) {
        // Player 1 is dealer (small blind)
        player1Chips -= smallBlind;
        player1CurrentBet = smallBlind;
        player2Chips -= bigBlind;
        player2CurrentBet = bigBlind;
        currentBet = bigBlind;
        // Non-dealer acts first pre-flop
        isPlayer1Turn = false;
    } else {
        // Player 2 is dealer (small blind)
        player2Chips -= smallBlind;
        player2CurrentBet = smallBlind;
        player1Chips -= bigBlind;
        player1CurrentBet = bigBlind;
        currentBet = bigBlind;
        // Non-dealer acts first pre-flop
        isPlayer1Turn = true;
    }

    pot = smallBlind + bigBlind;
    updateChipDisplay();
}

void MainWindow::onDeal() {
    // Retrieve current stage from your game object.
    GameStage stage = game.getStage();

    // Add debug output to check the current stage
    qDebug() << "Current stage in onDeal: " << static_cast<int>(stage);

    if (stage == GameStage::Flop) {
        // Deal the flop cards
        game.dealFlop();
        currentStage = GameStage::SecondBetting;
        game.setStage(GameStage::SecondBetting);
        qDebug() << "Dealt flop, moving to SecondBetting";
    } else if (stage == GameStage::Turn) {
        // Deal the turn card
        game.dealTurn();
        currentStage = GameStage::ThirdBetting;
        game.setStage(GameStage::ThirdBetting);
        qDebug() << "Dealt turn, moving to ThirdBetting";
    } else if (stage == GameStage::River) {
        // Deal the river card
        game.dealRiver();
        currentStage = GameStage::FinalBetting;
        game.setStage(GameStage::FinalBetting);
        qDebug() << "Dealt river, moving to FinalBetting";
    } else if (stage == GameStage::Showdown) {
        // Handle showdown
        qDebug() << "At showdown stage";
        ui->determineWinnerButton->setEnabled(true);
    } else {
        qDebug() << "Unknown stage for dealing: " << static_cast<int>(stage);
    }

    // First update the display
    displayGame();
    // Then update the UI based on the new stage
    updateUIForStage();
}

// Start a new betting round
void MainWindow::startNewBettingRound() {
    // Reset betting amounts for the new round
    player1CurrentBet = 0;
    player2CurrentBet = 0;
    currentBet = 0;
    bettingRoundComplete = false;

    // Post-flop, dealer acts first
    isPlayer1Turn = isPlayer1Dealer;
}

// Determine Winner slot.
void MainWindow::onDetermineWinner() {
    try {
        int winner = game.determineWinner();
        QString message;

        if (winner == 1) {
            message = QString("Player 1 wins %1 chips!").arg(pot);
            player1Chips += pot;
        } else if (winner == 2) {
            message = QString("Player 2 wins %1 chips!").arg(pot);
            player2Chips += pot;
        } else {
            message = "It's a tie! Split pot.";
            player1Chips += pot / 2;
            player2Chips += pot / 2;
        }

        QMessageBox::information(this, "Result", message);

        // Reset the pot and bet amount
        pot = 0;
        currentBet = 0;
        ui->betLineEdit->setText("");  // Clear the bet input

        // Rotate dealer position
        rotateDealerPosition();

        // Update chip display
        updateChipDisplay();

    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Error", e.what());
    }

    // After determining the winner, enable New Game button to allow restarting.
    ui->newGameButton->setEnabled(true);
   // ui->dealButton->setEnabled(false);
    ui->determineWinnerButton->setEnabled(false);
}

// Displays the current game state (players' hands and community cards).
void MainWindow::displayGame() {
    scene->clear();
    int yPlayer1 = 300;
    int yPlayer2 = 50;
    int spacing = 110;

    auto hand1 = game.getPlayer1().getHand();
    for (size_t i = 0; i < hand1.size(); i++) {
        const Card* card = hand1[i];
        QGraphicsPixmapItem *item = scene->addPixmap(card->getCardImage());
        item->setPos(i * spacing, yPlayer1);
    }

    auto hand2 = game.getPlayer2().getHand();
    for (size_t i = 0; i < hand2.size(); i++) {
        const Card* card = hand2[i];
        QGraphicsPixmapItem *item = scene->addPixmap(card->getCardImage());
        item->setPos(i * spacing, yPlayer2);
    }

    auto community = game.getCommunityCards();
    int yCommunity = 175;
    for (size_t i = 0; i < community.size(); i++) {
        const Card* card = community[i];
        QGraphicsPixmapItem *item = scene->addPixmap(card->getCardImage());
        item->setPos(i * spacing + 100, yCommunity);
    }
}


// void MainWindow::updateUIForStage() {
//     // Disable all betting buttons by default.
//     ui->foldButton->setEnabled(false);
//     ui->callButton->setEnabled(false);
//     ui->placeBetButton->setEnabled(false);

//     switch (game.getStage()) {
//         case GameStage::PreFlop:
//         //case GameStage::FirstBetting:
//             // Allow betting actions at the start (raise/call/fold).
//             ui->foldButton->setEnabled(true);
//             ui->callButton->setEnabled(true);
//             ui->placeBetButton->setEnabled(true);
//             break;
//         case GameStage::SecondBetting:
//             // After flop betting: allow betting actions.
//             ui->foldButton->setEnabled(true);
//             ui->callButton->setEnabled(true);
//             ui->placeBetButton->setEnabled(true);
//             break;
//         case GameStage::ThirdBetting:
//             // After turn betting: allow betting actions.
//             ui->foldButton->setEnabled(true);
//             ui->callButton->setEnabled(true);
//             ui->placeBetButton->setEnabled(true);
//             break;
//         case GameStage::FinalBetting:
//             // Final betting round.
//             ui->foldButton->setEnabled(true);
//             ui->callButton->setEnabled(true);
//             ui->placeBetButton->setEnabled(true);
//             break;
//         case GameStage::Showdown:
//             // Disable betting actions during showdown; winner is determined.
//             ui->foldButton->setEnabled(false);
//             ui->callButton->setEnabled(false);
//             ui->placeBetButton->setEnabled(false);
//             ui->determineWinnerButton->setEnabled(true);
//             break;
//     }
// }

void MainWindow::updateUIForStage() {
    // Disable all betting buttons by default
    ui->foldButton->setEnabled(false);
    ui->callButton->setEnabled(false);
    ui->placeBetButton->setEnabled(false);
    ui->determineWinnerButton->setEnabled(false);

    GameStage stage = game.getStage();

    // Handle the current player's turn indicator
    if (isPlayer1Turn) {
        ui->player1Label->setText(QString("Player 1: %1 chips [ACTIVE]").arg(player1Chips));
        ui->player2Label->setText(QString("Player 2: %1 chips").arg(player2Chips));
    } else {
        ui->player1Label->setText(QString("Player 1: %1 chips").arg(player1Chips));
        ui->player2Label->setText(QString("Player 2: %1 chips [ACTIVE]").arg(player2Chips));
    }

    // Enable appropriate buttons based on game stage
    switch (stage) {
        case GameStage::PreFlop:
        case GameStage::SecondBetting:
        case GameStage::ThirdBetting:
        case GameStage::FinalBetting:
            // In any betting round, enable betting controls
            ui->foldButton->setEnabled(true);
            ui->callButton->setEnabled(true);
            ui->placeBetButton->setEnabled(true);
            break;

        case GameStage::Showdown:
            // At showdown, enable the determine winner button
            ui->determineWinnerButton->setEnabled(true);
            break;

        default:
            break;
    }

    // Update chip display
    updateChipDisplay();
}

void MainWindow::onFold() {
    // Determine who folded and who wins
    if (isPlayer1Turn) {
        // Player 1 folded, Player 2 wins
        player2Chips += pot;
        QMessageBox::information(this, "Fold", QString("Player 1 folded. Player 2 wins %1 chips!").arg(pot));
    } else {
        // Player 2 folded, Player 1 wins
        player1Chips += pot;
        QMessageBox::information(this, "Fold", QString("Player 2 folded. Player 1 wins %1 chips!").arg(pot));
    }

    // Reset pot and update chip display
    pot = 0;
    updateChipDisplay();

    // Rotate dealer position for next hand
    rotateDealerPosition();

    // Enable new game button for next hand
    ui->newGameButton->setEnabled(true);
}


// void MainWindow::onCall() {
//     // For simplicity, assume the call matches the current bet.
//     pot += currentBet;
//     QMessageBox::information(this, "Call",
//         QString("Called %1 chips. Pot is now %2 chips.").arg(currentBet).arg(pot));

//     // Advance game stage based on the current stage.
//     GameStage stage = game.getStage();

//     if (stage == GameStage::PreFlop) {
//         // After first betting round, deal the Flop (if not already dealt).
//         // (If your game already dealt the flop before betting, simply change the stage.)
//         // Here we assume the flop is already visible, so we move to the next betting round.
//         game.setStage(GameStage::SecondBetting);
//     } else if (stage == GameStage::SecondBetting) {
//         // Call in second betting round: deal the Turn.
//         game.dealTurn();
//         game.setStage(GameStage::ThirdBetting);
//     } else if (stage == GameStage::ThirdBetting) {
//         // Call in third betting round: deal the River.
//         game.dealRiver();
//         game.setStage(GameStage::FinalBetting);
//     } else if (stage == GameStage::FinalBetting) {
//         // Final betting round: move to Showdown.
//         game.setStage(GameStage::Showdown);
//     }

//     updateUIForStage();
//     displayGame();
// }


// void MainWindow::onRaise() {
//     bool ok;
//     int raiseAmount = ui->betLineEdit->text().toInt(&ok);
//     if (ok && raiseAmount > 0) {
//         currentBet = raiseAmount;
//         pot += currentBet;
//         QMessageBox::information(this, "Raise",
//             QString("Raised %1 chips. Pot is now %2 chips.").arg(currentBet).arg(pot));

//         // Advance the stage similar to onCall().
//         GameStage stage = game.getStage();
//         if (stage == GameStage::PreFlop) {
//             // Advance to post-flop betting round.
//             game.setStage(GameStage::SecondBetting);
//         } else if (stage == GameStage::SecondBetting) {
//             // Deal the Turn and move to turn betting.
//             game.dealTurn();
//             game.setStage(GameStage::ThirdBetting);
//         } else if (stage == GameStage::ThirdBetting) {
//             // Deal the River and move to final betting.
//             game.dealRiver();
//             game.setStage(GameStage::FinalBetting);
//         } else if (stage == GameStage::FinalBetting) {
//             // Move to showdown.
//             game.setStage(GameStage::Showdown);
//         }
//         updateUIForStage();
//         displayGame();
//     } else {
//         QMessageBox::warning(this, "Invalid Bet", "Please enter a valid bet amount.");
//     }
// }

void MainWindow::onCall() {
    // Match the current bet
    int amountToCall = currentBet - (isPlayer1Turn ? player1CurrentBet : player2CurrentBet);

    if (isPlayer1Turn) {
        player1Chips -= amountToCall;
        player1CurrentBet = currentBet;
        QMessageBox::information(this, "Call", QString("Player 1 called %1 chips.").arg(amountToCall));
    } else {
        player2Chips -= amountToCall;
        player2CurrentBet = currentBet;
        QMessageBox::information(this, "Call", QString("Player 2 called %1 chips.").arg(amountToCall));
    }

    pot += amountToCall;

    // Advance the game based on current stage
    GameStage stage = game.getStage();
    QString message;

    if (stage == GameStage::PreFlop) {
        // Deal the flop
        game.dealFlop();
        game.setStage(GameStage::SecondBetting);
        message = "The flop has been dealt.";
    } else if (stage == GameStage::SecondBetting) {
        // Deal the turn
        game.dealTurn();
        game.setStage(GameStage::ThirdBetting);
        message = "The turn has been dealt.";
    } else if (stage == GameStage::ThirdBetting) {
        // Deal the river
        game.dealRiver();
        game.setStage(GameStage::FinalBetting);
        message = "The river has been dealt.";
    } else if (stage == GameStage::FinalBetting) {
        // Move to showdown
        game.setStage(GameStage::Showdown);
        message = "All betting rounds complete. Ready for showdown.";
    }

    QMessageBox::information(this, "Game Progress", message);

    // Update display and UI
    displayGame();
    updateUIForStage();
}

void MainWindow::onRaise() {
    bool ok;
    int raiseAmount = ui->betLineEdit->text().toInt(&ok);

    if (!ok || raiseAmount <= currentBet) {
        QMessageBox::warning(this, "Invalid Bet", "Please enter a valid raise amount greater than the current bet.");
        return;
    }

    // Calculate how much to add to the pot
    int amountToAdd = raiseAmount - (isPlayer1Turn ? player1CurrentBet : player2CurrentBet);

    if (isPlayer1Turn) {
        if (amountToAdd > player1Chips) {
            QMessageBox::warning(this, "Insufficient Chips", "You don't have enough chips for this raise.");
            return;
        }
        player1Chips -= amountToAdd;
        player1CurrentBet = raiseAmount;
        QMessageBox::information(this, "Raise", QString("Player 1 raised to %1 chips.").arg(raiseAmount));
    } else {
        if (amountToAdd > player2Chips) {
            QMessageBox::warning(this, "Insufficient Chips", "You don't have enough chips for this raise.");
            return;
        }
        player2Chips -= amountToAdd;
        player2CurrentBet = raiseAmount;
        QMessageBox::information(this, "Raise", QString("Player 2 raised to %1 chips.").arg(raiseAmount));
    }

    pot += amountToAdd;
    currentBet = raiseAmount;

    // Advance the game based on current stage
    GameStage stage = game.getStage();
    QString message;

    if (stage == GameStage::PreFlop) {
        // Deal the flop
        game.dealFlop();
        game.setStage(GameStage::SecondBetting);
        message = "The flop has been dealt.";
    } else if (stage == GameStage::SecondBetting) {
        // Deal the turn
        game.dealTurn();
        game.setStage(GameStage::ThirdBetting);
        message = "The turn has been dealt.";
    } else if (stage == GameStage::ThirdBetting) {
        // Deal the river
        game.dealRiver();
        game.setStage(GameStage::FinalBetting);
        message = "The river has been dealt.";
    } else if (stage == GameStage::FinalBetting) {
        // Move to showdown
        game.setStage(GameStage::Showdown);
        message = "All betting rounds complete. Ready for showdown.";
    }

    QMessageBox::information(this, "Game Progress", message);

    // Update display and UI
    displayGame();
    updateUIForStage();
}

void MainWindow::advanceStageIfBettingComplete() {
    if (!bettingRoundComplete) {
        return;
    }

    // Advance to next game stage based on current stage
    switch (currentStage) {
        case GameStage::PreFlop:
            currentStage = GameStage::Flop;
            break;

        case GameStage::Flop:
        case GameStage::SecondBetting:
            currentStage = GameStage::Turn;
            break;

        case GameStage::Turn:
        case GameStage::ThirdBetting:
            currentStage = GameStage::River;
            break;

        case GameStage::River:
        case GameStage::FinalBetting:
            currentStage = GameStage::Showdown;
            break;

        default:
            break;
    }

    // Update the game's stage
    game.setStage(currentStage);
}

void MainWindow::rotateDealerPosition() {
    // Toggle the dealer position
    isPlayer1Dealer = !isPlayer1Dealer;
}

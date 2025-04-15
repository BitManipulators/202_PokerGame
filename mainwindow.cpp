#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "poker_game.hpp"
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QDebug>

std::map<const Card*, QPixmap> card_image_cache;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , game()
    , engine(game)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    // Landing page: Connect start new game button.
    connect(ui->startNewGameButton, &QPushButton::clicked, this, &MainWindow::onStartNewGame);

    // Game page: Connect buttons.
    connect(ui->newGameButton, &QPushButton::clicked, this, &MainWindow::onNewGame);
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
    const std::size_t pot = game.get_pot();
    const Player& human_player = game.get_human_player();
    const Player& computer_player = game.get_computer_player();
    const std::size_t current_bet = (human_player.current_bet > computer_player.current_bet)
                                        ? human_player.current_bet : computer_player.current_bet;

    // Use existing labels instead of the ones that don't exist
    ui->player1Label->setText(QString("Player 1: %1 chips").arg(human_player.chips));
    ui->player2Label->setText(QString("Player 2: %1 chips").arg(computer_player.chips));
    ui->betLabel->setText(QString("Pot: %1 | Current Bet: %2").arg(pot).arg(current_bet));
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
    engine.new_game();

    updateChipDisplay();
    displayGame();

    // Disable New Game button and Determine Winner button after game starts.
    ui->newGameButton->setDisabled(true);
    ui->determineWinnerButton->setDisabled(true);

    // Enable Fold, Call, and Place Bet buttons after game starts.
    ui->foldButton->setEnabled(true);
    ui->callButton->setEnabled(true);
    ui->placeBetButton->setEnabled(true);

    ui->player2Label->raise();
}

// Determine Winner slot.
void MainWindow::displayWinner() {
    try {
        QString message;

        if (game.get_winner().value() == PokerHandWinner::Tie) {
            message = "It's a tie! Split pot.";
        } else if (game.get_winner().value() == PokerHandWinner::Player1) {
            message = QString("Player 1 wins %1 chips!").arg(game.get_pot());
        } else {
            message = QString("Player 2 wins %1 chips!").arg(game.get_pot());
        }

        QMessageBox::information(this, "Result", message);

        // Reset the pot and bet amount
        ui->betLineEdit->setText("");  // Clear the bet input

    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Error", e.what());
    }

    // After determining the winner, enable New Game button to allow restarting.
    ui->newGameButton->setEnabled(true);

    ui->foldButton->setDisabled(true);
    ui->callButton->setDisabled(true);
    ui->placeBetButton->setDisabled(true);
}

static QPixmap& loadImage(const Card* card) {
    auto it = card_image_cache.find(card);

    if (it == card_image_cache.end()) {
        QPixmap pix(QString::fromStdString(card->getCardImagePath()));
        if (pix.isNull()) {
            qDebug() << "Failed to load image:" << card->getCardImagePath();
        }
        pix = pix.scaled(100, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        card_image_cache[card] = pix;
    }

    return card_image_cache[card];
}

// Displays the current game state (players' hands and community cards).
void MainWindow::displayGame() {
    scene->clear();
    int yPlayer1 = 300;
    int yPlayer2 = 50;
    int spacing = 110;

    auto hand1 = game.get_human_player().hand;
    for (size_t i = 0; i < hand1.size(); i++) {
        const Card* card = hand1[i];
        QGraphicsPixmapItem *item = scene->addPixmap(loadImage(card));
        item->setPos(i * spacing, yPlayer1);
    }

    auto hand2 = game.get_computer_player().hand;
    for (size_t i = 0; i < hand2.size(); i++) {
        const Card* card = hand2[i];
        QGraphicsPixmapItem *item = scene->addPixmap(loadImage(card));
        item->setPos(i * spacing, yPlayer2);
    }

    auto community = game.get_community_cards();
    int yCommunity = 175;
    for (size_t i = 0; i < community.size(); i++) {
        const Card* card = community[i];
        QGraphicsPixmapItem *item = scene->addPixmap(loadImage(card));
        item->setPos(i * spacing + 100, yCommunity);
    }
}

void MainWindow::onFold() {
    game.set_player_move(PlayerType::Human, Fold{});

    GameAction::Result fold_result = engine.make_moves();
    if (!fold_result.ok) {
        QMessageBox::warning(this, "Error", QString::fromStdString(*fold_result.error_message));
        return;
    }

    // Update chip display
    updateChipDisplay();

    // Enable new game button for next hand
    ui->newGameButton->setEnabled(true);

    // Update display and UI
    displayGame();

    if (game.has_ended()) {
        displayWinner();
    }
}

void MainWindow::onCall() {
    game.set_player_move(PlayerType::Human, Call{});

    GameAction::Result call_result = engine.make_moves();
    if (!call_result.ok) {
        QMessageBox::warning(this, "Error", QString::fromStdString(*call_result.error_message));
        return;
    }

    // Update chip display
    updateChipDisplay();

    // Update display and UI
    displayGame();

    if (game.has_ended()) {
        displayWinner();
    }
}

void MainWindow::onRaise() {
    bool ok;
    std::size_t raiseAmount = ui->betLineEdit->text().toLong(&ok);

    if (ok) {
        game.set_player_move(PlayerType::Human, Raise{raiseAmount});

        GameAction::Result raise_result = engine.make_moves();
        if (!raise_result.ok) {
            QMessageBox::warning(this, "Error", QString::fromStdString(*raise_result.error_message));
            return;
        }

        // Update chip display
        updateChipDisplay();

        // Update display and UI
        displayGame();

        if (game.has_ended()) {
            displayWinner();
        }
    }
}

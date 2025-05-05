#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "poker_game.hpp"
#include "console_logger.hpp"
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QDebug>
#include <QPushButton>
#include <QDialog>
#include <QStyle>
#include <QGraphicsColorizeEffect>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QScreen>


std::map<const Card*, QPixmap> card_image_cache;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , game()
    , engine(game)
{

    game.add_observer(this);

    ConsoleLogger* consoleLogger = new ConsoleLogger();
    game.add_observer(consoleLogger);
    ui->setupUi(this);
    ui->strategyComboBox->setCurrentIndex(0);


    this->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            QGuiApplication::primaryScreen()->availableGeometry()));

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    // Set up the graphics view
    QGraphicsScene *backgroundScene = new QGraphicsScene(this);
    ui->graphicsView->setBackgroundBrush(QBrush(QColor(25, 80, 50)));

    QPixmap texturePix(":/images/felt_texture.png");
    if (!texturePix.isNull()) {
        texturePix = texturePix.scaled(ui->graphicsView->width(), ui->graphicsView->height(),
                                      Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QBrush textureBrush(texturePix);
        ui->graphicsView->setBackgroundBrush(textureBrush);
    }

    // Landing page: Connect start new game button.
    connect(ui->startNewGameButton, &QPushButton::clicked, this, &MainWindow::onStartNewGame);

    // Game page: Connect buttons.
    connect(ui->newGameButton, &QPushButton::clicked, this, &MainWindow::onNewGame);
    connect(ui->placeBetButton, &QPushButton::clicked, this, &MainWindow::onRaise);
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWindow::close);
    connect(ui->foldButton, &QPushButton::clicked, this, &MainWindow::onFold);
    connect(ui->callButton, &QPushButton::clicked, this, &MainWindow::onCall);
    connect(ui->strategyComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onStrategyChanged);


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

    QString selectedStrategy = ui->strategyComboBox->currentText();
    if (selectedStrategy != "Easy" && selectedStrategy != "Medium") {
        QMessageBox::warning(this, "Strategy Not Selected",
                             "Please select a valid strategy to start the game.");
        return; // Early return if no valid strategy is selected
    }

    // Reset buttons and UI — wait for strategy
    ui->strategyComboBox->setEnabled(true);
    ui->strategyComboBox->setCurrentIndex(0);  // Reset to default

    ui->foldButton->setDisabled(true);
    ui->callButton->setDisabled(true);
    ui->placeBetButton->setDisabled(true);
    ui->newGameButton->setDisabled(true);  // Disable until game ends

    ui->moveHistoryList->clear();
    scene->clear();
    ui->betLabel->setText("Pot: 0 | Current Bet: 0");
    ui->player1Label->setText("Player 1: -- chips");
    ui->player2Label->setText("Player 2: -- chips");

    qDebug() << "[INFO] Waiting for strategy selection...";
}

// Starts a new game and updates button states.
void MainWindow::onNewGame() {
    QString selectedStrategy = ui->strategyComboBox->currentText();

    std::unique_ptr<ComputerStrategy> strategy;

    if (selectedStrategy == "Easy") {
        strategy = std::make_unique<EasyStrategy>();
    } else if (selectedStrategy == "Medium") {
        strategy = std::make_unique<MediumStrategy>();
    } else {
        QMessageBox::warning(this, "Strategy Not Selected",
                             "Please select a valid strategy to start the game.");
        return; // Early return if no valid strategy is selected
    }

    // Set the strategy directly to the computer player
    const Player& playerRef = game.get_computer_player();
    ComputerPlayer* computerPlayer = dynamic_cast<ComputerPlayer*>(const_cast<Player*>(&playerRef));


    if (computerPlayer) {
        computerPlayer->set_strategy(std::move(strategy));
    } else {
        QMessageBox::critical(this, "Error", "Computer player casting failed!");
        return;
    }
    ui->strategyComboBox->setDisabled(true);

    engine.new_game();

    updateChipDisplay();
    displayGame();

    ui->moveHistoryList->clear();

    // Disable New Game button and Determine Winner button after game starts.
    ui->newGameButton->setDisabled(true);

    // Enable Fold, Call, and Place Bet buttons after game starts.
    ui->foldButton->setEnabled(true);
    ui->callButton->setEnabled(true);
    ui->placeBetButton->setEnabled(true);
    //ui->strategyComboBox->setEnabled(true);

    ui->player2Label->raise();
}

// Determine Winner slot.
void MainWindow::displayWinner() {
    try {
        QString message;
        std::string handDescription = game.get_winning_hand_description();

        if (game.get_winner().value() == PokerHandWinner::Tie) {
           message = QString("It's a tie!.\nHand: %1").arg(QString::fromStdString(handDescription));
        } else if (game.get_winner().value() == PokerHandWinner::Player1) {
            message = QString("Player 1 wins %1 chips!\nWinning hand: %2")
            .arg(game.get_pot())
            .arg(QString::fromStdString(handDescription));
        } else {
            message = QString("Player 2 wins %1 chips!\nWinning hand: %2")
            .arg(game.get_pot())
            .arg(QString::fromStdString(handDescription));
        }

        QMessageBox msgBox(QMessageBox::Information, "Result", message, QMessageBox::Ok, this);

        // Center the message box
        msgBox.setGeometry(
            QStyle::alignedRect(
                Qt::LeftToRight,
                Qt::AlignCenter,
                msgBox.sizeHint(),
                this->geometry()
            )
        );

        msgBox.exec();

        ui->foldButton->setDisabled(true);
        ui->callButton->setDisabled(true);
        ui->placeBetButton->setDisabled(true);

        // Enable New Game button
        ui->newGameButton->setEnabled(true);

        // Reset the pot and bet amount
        ui->betLineEdit->setText("");  // Clear the bet input

    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Error", e.what());
    }

    // After determining the winner, enable New Game button to allow restarting.

    ui->strategyComboBox->setEnabled(true);
    ui->strategyComboBox->setCurrentIndex(0);

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
        pix = pix.scaled(90, 135, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        card_image_cache[card] = pix;
    }

    return card_image_cache[card];
}

void MainWindow::displayGame() {
    scene->clear();

    // Increase scene size significantly
    scene->setSceneRect(0, 0, 900, 600);

    // Better vertical positioning
    int yPlayer1 = 420;  // Move player 1 cards lower
    int yCommunity = 230; // Center community cards vertically
    int yPlayer2 = 50;   // Keep player 2 cards at top

    // More generous card spacing
    int spacing = 100;
    int cardWidth = 90;

    // Calculate positions for better centering
    auto hand1 = game.get_human_player().hand;
    auto hand2 = game.get_computer_player().hand;
    auto community = game.get_community_cards();

    // Center player 1's cards
    int player1StartX = (scene->width() - (hand1.size() * spacing)) / 2;

    // Center player 2's cards
    int player2StartX = (scene->width() - (hand2.size() * spacing)) / 2;

    // Center community cards
    int communityStartX = (scene->width() - (community.size() * spacing)) / 2;

    // Display community cards (center row)
    for (size_t i = 0; i < community.size(); i++) {
        const Card* card = community[i];
        QGraphicsPixmapItem *item = scene->addPixmap(loadImage(card));
        item->setPos(communityStartX + i * spacing, yCommunity);
    }

    // Display player 1's hand (bottom row)
    for (size_t i = 0; i < hand1.size(); i++) {
        const Card* card = hand1[i];
        QGraphicsPixmapItem *item = scene->addPixmap(loadImage(card));
        item->setPos(player1StartX + i * spacing, yPlayer1);

        // Highlight player 1's cards if they are the winner
        if (game.has_ended() && game.get_winner().has_value() &&
            game.get_winner().value() == PokerHandWinner::Player1) {
            QGraphicsRectItem *highlight = scene->addRect(
                player1StartX + i * spacing - 5, yPlayer1 - 5,
                cardWidth + 10, 135 + 10,
                QPen(QColor(255, 215, 0, 200), 3)
            );
             createGlowEffect(item);
            highlight->setZValue(-1);
        }
    }

    // Display player 2's hand (top row)
    for (size_t i = 0; i < hand2.size(); i++) {
        QGraphicsPixmapItem *item;

        if (game.has_ended()) {
            // Show actual cards when game has ended
            const Card* card = hand2[i];
            item = scene->addPixmap(loadImage(card));
        } else {
            // Show card backs during gameplay
            static QPixmap cardBackImage(":/images/back_light.png");
            cardBackImage = cardBackImage.scaled(90, 135, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            item = scene->addPixmap(cardBackImage);
        }

        item->setPos(player2StartX + i * spacing, yPlayer2);

        // Highlight player 2's cards if they are the winner
        if (game.has_ended() && game.get_winner().has_value() &&
            game.get_winner().value() == PokerHandWinner::Player2) {
            QGraphicsRectItem *highlight = scene->addRect(
                player2StartX + i * spacing - 5, yPlayer2 - 5,
                cardWidth + 10, 135 + 10,
                QPen(QColor(255, 215, 0, 200), 3)
            );
            createGlowEffect(item);
            highlight->setZValue(-1);
        }
    }
}

void MainWindow::onFold() {
    game.set_player_move(PlayerType::Human, Fold{});

    GameAction::Result fold_result = engine.make_moves();
    if (!fold_result.ok) {
        QMessageBox::warning(this, "Error", QString::fromStdString(*fold_result.error_message));
        return;
    }

    // Update display
    updateChipDisplay();
    displayGame();

    const Player& computer = game.get_computer_player();
    const Move& move = computer.getLatestMove();

    // Only display winner if game has ended
    if (game.has_ended()) {
        displayGame();
        displayWinner();
        ui->newGameButton->setEnabled(true);
    }
}


void MainWindow::onCall() {
    // Human chooses to call
    game.set_player_move(PlayerType::Human, Call{});

    GameAction::Result call_result = engine.make_moves();
    if (!call_result.ok) {
        QMessageBox::warning(this, "Error", QString::fromStdString(*call_result.error_message));
        return;
    }

    // Update chip display and board
    updateChipDisplay();
    displayGame();

    const Player& computer = game.get_computer_player();
    const Move& move = computer.getLatestMove();

    // Check if game has ended
    if (game.has_ended()) {
        displayGame();
        displayWinner();
        ui->newGameButton->setEnabled(true);
    }
}

void MainWindow::onRaise() {
    bool ok;
    std::size_t raiseAmount = ui->betLineEdit->text().toLong(&ok);

    if (!ok || raiseAmount == 0) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid raise amount.");
        return;
    }

    game.set_player_move(PlayerType::Human, Raise{raiseAmount});

    GameAction::Result raise_result = engine.make_moves();
    if (!raise_result.ok) {
        QMessageBox::warning(this, "Error", QString::fromStdString(*raise_result.error_message));
        return;
    }

    // Update chip display and board
    updateChipDisplay();
    displayGame();

    const Player& computer = game.get_computer_player();
    const Move& move = computer.getLatestMove();

    // Only display winner if game has ended
    if (game.has_ended()) {
        displayGame();
        displayWinner();
        ui->newGameButton->setEnabled(true);
        return;
    }
}

void MainWindow::createGlowEffect(QGraphicsPixmapItem *cardItem) {
    // Create the glow effect
    QGraphicsColorizeEffect *glowEffect = new QGraphicsColorizeEffect();
    glowEffect->setColor(QColor(255, 215, 0));  // Gold color
    glowEffect->setStrength(0.3);
    cardItem->setGraphicsEffect(glowEffect);

    // Create animation for the glow effect
    QPropertyAnimation *glowAnimation = new QPropertyAnimation(glowEffect, "strength");
    glowAnimation->setDuration(1500);  // 1.5 seconds per cycle
    glowAnimation->setStartValue(0.1);
    glowAnimation->setEndValue(0.5);
    glowAnimation->setLoopCount(-1);  // Infinite looping
    glowAnimation->setEasingCurve(QEasingCurve::InOutSine);

    // Make animation go back and forth
    QSequentialAnimationGroup *glowGroup = new QSequentialAnimationGroup(this);
    glowGroup->addAnimation(glowAnimation);

    QPropertyAnimation *reverseGlow = new QPropertyAnimation(glowEffect, "strength");
    reverseGlow->setDuration(1500);
    reverseGlow->setStartValue(0.5);
    reverseGlow->setEndValue(0.1);
    reverseGlow->setEasingCurve(QEasingCurve::InOutSine);

    glowGroup->addAnimation(reverseGlow);
    glowGroup->setLoopCount(-1);  // Infinite looping
    glowGroup->start();
}

void MainWindow::onGameEvent(const GameEvent& event) {

qDebug() << "[DEBUG] onGameEvent triggered";
    if (auto* moveEvent = dynamic_cast<const MoveEvent*>(&event)) {
        QString text;
        if (std::holds_alternative<Fold>(moveEvent->move)) {
            text = moveEvent->player == PlayerType::Human ? "You folded!" : "Computer folded!";
        } else if (std::holds_alternative<Call>(moveEvent->move)) {
            text = moveEvent->player == PlayerType::Human ? "You called!" : "Computer called!";
        } else if (std::holds_alternative<Raise>(moveEvent->move)) {
            auto raiseVal = std::get<Raise>(moveEvent->move).amount;
            text = moveEvent->player == PlayerType::Human
                ? QString("You raised to %1!").arg(raiseVal)
                : QString("Computer raised to %1!").arg(raiseVal);
        }

        qDebug() << "[DEBUG] MoveEvent received:" << text;

        ui->moveHistoryList->addItem(text);
        ui->moveHistoryList->scrollToBottom();

    }
}


void MainWindow::onStrategyChanged(const QString& strategy) {
    if (strategy != "Easy" && strategy != "Medium")
        return;

    std::unique_ptr<ComputerStrategy> selectedStrategy;

    if (strategy == "Easy") {
        selectedStrategy = std::make_unique<EasyStrategy>();
    } else {
        selectedStrategy = std::make_unique<MediumStrategy>();
    }

    const Player& playerRef = game.get_computer_player();
    ComputerPlayer* computerPlayer = dynamic_cast<ComputerPlayer*>(const_cast<Player*>(&playerRef));

    if (!computerPlayer) {
        QMessageBox::critical(this, "Error", "Computer player casting failed!");
        return;
    }

    computerPlayer->set_strategy(std::move(selectedStrategy));
    ui->strategyComboBox->setDisabled(true);

    engine.new_game();
    updateChipDisplay();
    displayGame();

    ui->moveHistoryList->clear();

    ui->foldButton->setEnabled(true);
    ui->callButton->setEnabled(true);
    ui->placeBetButton->setEnabled(true);
    ui->newGameButton->setDisabled(true);  // Until game ends

    ui->player2Label->raise();

    qDebug() << "[INFO] Game started with strategy:" << strategy;
}

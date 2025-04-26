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

std::map<const Card *, QPixmap> card_image_cache;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), controller(GameController())
{

    // game.add_observer(this);

    // ConsoleLogger* consoleLogger = new ConsoleLogger();
    // game.add_observer(consoleLogger);
    ui->setupUi(this);

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
    if (!texturePix.isNull())
    {
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

    // Start with landing page
    ui->stackedWidget->setCurrentIndex(0);

    // Update UI to show chips and pot
    UIComponents ui_components_state = controller.get_ui_components();
    updateChipDisplay(ui_components_state);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Update chip displays
void MainWindow::updateChipDisplay(UIComponents ui_components_state)
{

    const std::size_t pot = ui_components_state.pot_value;
    const std::size_t current_bet = ui_components_state.current_bet;
    
    // Use existing labels instead of the ones that don't exist
    ui->player1Label->setText(QString("Player 1: %1 chips").arg(ui_components_state.human_chips));
    ui->player2Label->setText(QString("Player 2: %1 chips").arg(ui_components_state.computer_chips));
    ui->betLabel->setText(QString("Pot: %1 | Current Bet: %2").arg(pot).arg(current_bet));
}

// Slot to handle landing page "Start New Game" click.
void MainWindow::onStartNewGame()
{
    // Switch to game page.
    ui->stackedWidget->setCurrentIndex(1);
    qApp->processEvents();
    onNewGame(); // Start a new game.
    ui->player2Label->raise();
}

// Starts a new game and updates button states.
void MainWindow::onNewGame()
{

    UIState state = controller.new_game();

    UIComponents ui_components_state = controller.get_ui_components();
    updateChipDisplay(ui_components_state);
    displayGame(ui_components_state);

    ui->moveHistoryList->clear();

    // Disable New Game button and Determine Winner button after game starts.
    ui->newGameButton->setDisabled(true);

    // Enable Fold, Call, and Place Bet buttons after game starts.
    ui->foldButton->setEnabled(true);
    ui->callButton->setEnabled(true);
    ui->placeBetButton->setEnabled(true);

    ui->player2Label->raise();

    callEngine();
}

void MainWindow::callEngine()
{

    UIState state = controller.next_move();

    UIComponents ui_components_state = controller.get_ui_components();

    if (state == UIState::HUMAN_PLAYER_ACTION)
    {

        updateChipDisplay(ui_components_state);
        displayGame(ui_components_state);
        // Enable Fold, Call, and Place Bet buttons after game starts.
        ui->foldButton->setEnabled(true);
        ui->callButton->setEnabled(true);
        ui->placeBetButton->setEnabled(true);
        ui->player2Label->raise();
    }

    if (state == UIState::HUMAN_PLAYER_ACTION_ERROR)
    {
        QMessageBox::warning(this, "Error", QString::fromStdString(ui_components_state.error_message.value()));
        return;
    }

    if (state == UIState::GAME_ENDED)
    {
        displayWinner(ui_components_state);
        ui->newGameButton->setEnabled(true);
    }
}

// Determine Winner slot.
void MainWindow::displayWinner(UIComponents ui_components_state)
{
    try
    {
        QString message;
        std::string handDescription = ui_components_state.winning_hand_desc;

        if (ui_components_state.winner == PokerHandWinner::Tie)
        {
            message = QString("It's a tie!.\nHand: %1").arg(QString::fromStdString(handDescription));
        }
        else if (ui_components_state.winner == PokerHandWinner::Player1)
        {
            message = QString("Player 1 wins %1 chips!\nWinning hand: %2")
                          .arg(ui_components_state.pot_value)
                          .arg(QString::fromStdString(handDescription));
        }
        else
        {
            message = QString("Player 2 wins %1 chips!\nWinning hand: %2")
                          .arg(ui_components_state.pot_value)
                          .arg(QString::fromStdString(handDescription));
        }

        QMessageBox msgBox(QMessageBox::Information, "Result", message, QMessageBox::Ok, this);

        // Center the message box
        msgBox.setGeometry(
            QStyle::alignedRect(
                Qt::LeftToRight,
                Qt::AlignCenter,
                msgBox.sizeHint(),
                this->geometry()));

        msgBox.exec();

        ui->foldButton->setDisabled(true);
        ui->callButton->setDisabled(true);
        ui->placeBetButton->setDisabled(true);

        // Enable New Game button
        ui->newGameButton->setEnabled(true);

        // Reset the pot and bet amount
        ui->betLineEdit->setText(""); // Clear the bet input
    }
    catch (const std::exception &e)
    {
        QMessageBox::warning(this, "Error", e.what());
    }

    // After determining the winner, enable New Game button to allow restarting.
    ui->newGameButton->setEnabled(true);

    ui->foldButton->setDisabled(true);
    ui->callButton->setDisabled(true);
    ui->placeBetButton->setDisabled(true);
}

static QPixmap &loadImage(const Card *card)
{
    auto it = card_image_cache.find(card);

    if (it == card_image_cache.end())
    {
        QPixmap pix(QString::fromStdString(card->getCardImagePath()));
        if (pix.isNull())
        {
            qDebug() << "Failed to load image:" << card->getCardImagePath();
        }
        pix = pix.scaled(90, 135, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        card_image_cache[card] = pix;
    }

    return card_image_cache[card];
}

void MainWindow::displayGame(UIComponents ui_components_state)
{
    scene->clear();

    // Increase scene size significantly
    scene->setSceneRect(0, 0, 900, 600);

    // Better vertical positioning
    int yPlayer1 = 420;   // Move player 1 cards lower
    int yCommunity = 230; // Center community cards vertically
    int yPlayer2 = 50;    // Keep player 2 cards at top

    // More generous card spacing
    int spacing = 100;
    int cardWidth = 90;

    // Calculate positions for better centering
    auto hand1 = ui_components_state.human_player_cards;
    auto hand2 = ui_components_state.computer_player_cards;
    auto community = ui_components_state.community_cards;

    // Center player 1's cards
    int player1StartX = (scene->width() - (hand1.size() * spacing)) / 2;

    // Center player 2's cards
    int player2StartX = (scene->width() - (hand2.size() * spacing)) / 2;

    // Center community cards
    int communityStartX = (scene->width() - (community.size() * spacing)) / 2;

    // Display community cards (center row)
    for (size_t i = 0; i < community.size(); i++)
    {
        const Card *card = community[i];
        QGraphicsPixmapItem *item = scene->addPixmap(loadImage(card));
        item->setPos(communityStartX + i * spacing, yCommunity);
    }

    // Display player 1's hand (bottom row)
    for (size_t i = 0; i < hand1.size(); i++)
    {
        const Card *card = hand1[i];
        QGraphicsPixmapItem *item = scene->addPixmap(loadImage(card));
        item->setPos(player1StartX + i * spacing, yPlayer1);

        // Highlight player 1's cards if they are the winner
        if (ui_components_state.is_game_ended &&
            ui_components_state.winner == PokerHandWinner::Player1)
        {
            QGraphicsRectItem *highlight = scene->addRect(
                player1StartX + i * spacing - 5, yPlayer1 - 5,
                cardWidth + 10, 135 + 10,
                QPen(QColor(255, 215, 0, 200), 3));
            createGlowEffect(item);
            highlight->setZValue(-1);
        }
    }

    // Display player 2's hand (top row)
    for (size_t i = 0; i < hand2.size(); i++)
    {
        QGraphicsPixmapItem *item;

        if (ui_components_state.is_game_ended)
        {
            // Show actual cards when game has ended
            const Card *card = hand2[i];
            item = scene->addPixmap(loadImage(card));
        }
        else
        {
            // Show card backs during gameplay
            static QPixmap cardBackImage(":/images/back_light.png");
            cardBackImage = cardBackImage.scaled(90, 135, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            item = scene->addPixmap(cardBackImage);
        }

        item->setPos(player2StartX + i * spacing, yPlayer2);

        // Highlight player 2's cards if they are the winner
        if (ui_components_state.is_game_ended &&
            ui_components_state.winner == PokerHandWinner::Player2)
        {
            QGraphicsRectItem *highlight = scene->addRect(
                player2StartX + i * spacing - 5, yPlayer2 - 5,
                cardWidth + 10, 135 + 10,
                QPen(QColor(255, 215, 0, 200), 3));
            createGlowEffect(item);
            highlight->setZValue(-1);
        }
    }
}

void MainWindow::onFold()
{
    controller.set_human_move(Fold{});
    controller.set_human_made_move();
    callEngine();
}

void MainWindow::onCall()
{
    // Human chooses to call
    controller.set_human_move(Call{});
    controller.set_human_made_move();
    callEngine();
}

void MainWindow::onRaise()
{
    bool ok;
    std::size_t raiseAmount = ui->betLineEdit->text().toLong(&ok);
    if (ok)
    {
        controller.set_human_move(Raise{raiseAmount});
        controller.set_human_made_move();
        callEngine();
    }
    else
    {

        const std::string input_empty_error = "Input the amount to be raised";
        QMessageBox::warning(this, "Error", QString::fromStdString(input_empty_error));
        return;
    }
}

void MainWindow::createGlowEffect(QGraphicsPixmapItem *cardItem)
{
    // Create the glow effect
    QGraphicsColorizeEffect *glowEffect = new QGraphicsColorizeEffect();
    glowEffect->setColor(QColor(255, 215, 0)); // Gold color
    glowEffect->setStrength(0.3);
    cardItem->setGraphicsEffect(glowEffect);

    // Create animation for the glow effect
    QPropertyAnimation *glowAnimation = new QPropertyAnimation(glowEffect, "strength");
    glowAnimation->setDuration(1500); // 1.5 seconds per cycle
    glowAnimation->setStartValue(0.1);
    glowAnimation->setEndValue(0.5);
    glowAnimation->setLoopCount(-1); // Infinite looping
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
    glowGroup->setLoopCount(-1); // Infinite looping
    glowGroup->start();
}

void MainWindow::onGameEvent(const GameEvent &event)
{

    qDebug() << "[DEBUG] onGameEvent triggered";
    if (auto *moveEvent = dynamic_cast<const MoveEvent *>(&event))
    {
        QString text;
        if (std::holds_alternative<Fold>(moveEvent->move))
        {
            text = moveEvent->player == PlayerType::Human ? "You folded!" : "Computer folded!";
        }
        else if (std::holds_alternative<Call>(moveEvent->move))
        {
            text = moveEvent->player == PlayerType::Human ? "You called!" : "Computer called!";
        }
        else if (std::holds_alternative<Raise>(moveEvent->move))
        {
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

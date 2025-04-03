#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    connect(ui->newGameButton, &QPushButton::clicked, this, &MainWindow::onNewGame);
    connect(ui->dealFlopButton, &QPushButton::clicked, this, &MainWindow::onDealFlop);
    connect(ui->dealTurnButton, &QPushButton::clicked, this, &MainWindow::onDealTurn);
    connect(ui->dealRiverButton, &QPushButton::clicked, this, &MainWindow::onDealRiver);
    connect(ui->determineWinnerButton, &QPushButton::clicked, this, &MainWindow::onDetermineWinner);
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWindow::close);

    displayGame();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onNewGame() {
    game.startNewGame();
    displayGame();
}

void MainWindow::onDealFlop() {
    game.dealFlop();
    displayGame();
}

void MainWindow::onDealTurn() {
    game.dealTurn();
    displayGame();
}

void MainWindow::onDealRiver() {
    game.dealRiver();
    displayGame();
}

void MainWindow::onDetermineWinner() {
    try {
        int winner = game.determineWinner();
        QString message;
        if (winner == 1)
            message = "Player 1 wins!";
        else if (winner == 2)
            message = "Player 2 wins!";
        else
            message = "It's a tie!";
        QMessageBox::information(this, "Result", message);
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void MainWindow::loadCardImages(Card &card) {
    // Assuming your card images are in the Qt resource system under ":/cards/"
    QString cardImagePath = ":/images/" + card.getCardName() + ".png";
    QPixmap pix(cardImagePath);
    if (pix.isNull()) {
        qDebug() << "Failed to load image:" << cardImagePath;
    }
    // Scale the image if needed (adjust size as required)
    pix = pix.scaled(100, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    card.setCardImage(pix);
}

void MainWindow::displayGame() {
    scene->clear();

    // Define positions
    int yPlayer1 = 300;  // Bottom for player1
    int yPlayer2 = 50;   // Top for player2
    int spacing = 110;

    // Display Player 1 hand.
    auto hand1 = game.getPlayer1().getHand();
    for (size_t i = 0; i < hand1.size(); i++) {
        // Note: we work on a copy here; in a more advanced design, you might store loaded images.
        Card card = hand1[i];
        if (card.getCardImage().isNull()) {
            loadCardImages(card);
        }
        QGraphicsPixmapItem *item = scene->addPixmap(card.getCardImage());
        item->setPos(i * spacing, yPlayer1);
    }

    // Display Player 2 hand.
    auto hand2 = game.getPlayer2().getHand();
    for (size_t i = 0; i < hand2.size(); i++) {
        Card card = hand2[i];
        if (card.getCardImage().isNull()) {
            loadCardImages(card);
        }
        QGraphicsPixmapItem *item = scene->addPixmap(card.getCardImage());
        item->setPos(i * spacing, yPlayer2);
    }

    // Display community cards.
    auto community = game.getCommunityCards();
    int yCommunity = 175;
    for (size_t i = 0; i < community.size(); i++) {
        Card card = community[i];
        if (card.getCardImage().isNull()) {
            loadCardImages(card);
        }
        QGraphicsPixmapItem *item = scene->addPixmap(card.getCardImage());
        item->setPos(i * spacing + 100, yCommunity);
    }
}

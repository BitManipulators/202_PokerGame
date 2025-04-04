#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsScene>
#include "pokergame.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartNewGame();
    void onNewGame();
    void onDeal();
    void onDetermineWinner();
    void onFold();
    void onCall();
    void onRaise();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    PokerGame game;
    int pot;
    int currentBet;
    int player1Chips;
    int player2Chips;
    int smallBlind;
    int bigBlind;
    bool isPlayer1Turn;
    int player1CurrentBet;
    int player2CurrentBet;
    bool bettingRoundComplete;
    GameStage currentStage;
    bool isPlayer1Dealer;

    void loadCardImages(Card &card);
    void displayGame();
    void updateUIForStage();
    void postBlinds();
    void startNewBettingRound();
    void updateChipDisplay();
    void advanceStageIfBettingComplete();
    void rotateDealerPosition();
};

#endif // MAINWINDOW_H

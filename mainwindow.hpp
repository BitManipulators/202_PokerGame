#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsScene>
#include "poker_engine.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow, public Observer
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void on_game_event(const GameEvent& event) override;

private slots:
    void onStartNewGame();
    void onNewGame();
    void onFold();
    void onCall();
    void onRaise();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    PokerGame game;
    PokerEngine engine;

    void displayGame();
    void displayWinner();
    void updateChipDisplay();

    static std::map<const Card*, QPixmap> card_image_cache;

    void createGlowEffect(QGraphicsPixmapItem *cardItem);
    void onStrategyChanged(const QString& strategy);
    void showPokerHandRanking();

    void onRestartGame();

};

#endif // MAINWINDOW_H

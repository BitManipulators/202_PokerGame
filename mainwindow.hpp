#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "pokergame.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void onNewGame();
    void onDealFlop();
    void onDealTurn();
    void onDealRiver();
    void onDetermineWinner();
private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    PokerGame game;
    void loadCardImages(Card &card);
    void displayGame();
};

#endif // MAINWINDOW_H

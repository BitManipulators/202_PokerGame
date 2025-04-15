#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsScene>
#include "poker_engine.hpp"


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
    void onFold();
    void onCall();
    void onRaise();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    PokerEngine engine;

    void displayGame();
    void displayWinner();
    void updateChipDisplay();

    static std::map<const Card*, QPixmap> card_image_cache;
};

#endif // MAINWINDOW_H

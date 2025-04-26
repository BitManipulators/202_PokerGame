#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsScene>
#include "game_controller.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow, public Observer
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void onGameEvent(const GameEvent& event) override;

private slots:
    void onStartNewGame();
    void onNewGame();
    void onFold();
    void onCall();
    void onRaise();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    GameController controller;

    void displayGame(UIComponents ui_components_state);
    void displayWinner(UIComponents ui_components_state);
    void updateChipDisplay(UIComponents ui_components_state);
    void callEngine();

    static std::map<const Card*, QPixmap> card_image_cache;

    void createGlowEffect(QGraphicsPixmapItem *cardItem);

};

#endif // MAINWINDOW_H

// gameplane.h
// -*- encoding: utf-8 -*-
// @Author: RZH

#ifndef SNAKE_GAMEPLANE_H
#define SNAKE_GAMEPLANE_H

#include <random>
#include <vector>

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>

struct Point {
    int x;
    int y;

    bool operator==(const Point other) const { return (x == other.x) && (y == other.y); }
};

QT_BEGIN_NAMESPACE
namespace Ui { class GamePlane; }
QT_END_NAMESPACE

class GamePlane : public QWidget {
Q_OBJECT

#define INIT 0
#define GAME 1
#define PAUSE 2
#define END (-1)

#define RIGHT 0
#define UP 1
#define LEFT 2
#define DOWN 3

public:
    explicit GamePlane(QWidget *parent = nullptr);

    ~GamePlane() override;

    void actionStart();

    void actionPause();

    void actionResume();

    void actionRestart();

    void actionSave();

    void actionLoad();

    void mousePressEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

private:
    const int pixel;
    const int fruit_bonus;
    const double speedup;
    const double max_speed;

    int width;
    int height;
    int time = 0;
    int score = 0;
    int phase = INIT;
    int direction = RIGHT;
    int real_direction = RIGHT;
    int growth = 0;
    double speed;
    std::vector<Point> snake;
    std::vector<Point> wall;
    std::vector<Point> fruit;

    QTimer *timer;
    Ui::GamePlane *ui;

    std::mt19937 *mt = nullptr;
    std::uniform_int_distribution<int> *gen_fruit_x = nullptr;
    std::uniform_int_distribution<int> *gen_fruit_y = nullptr;

    void _update();

    void paintEvent(QPaintEvent *event) override;

    bool noOverlap(Point point, bool check_snake = true,
                   bool check_wall = true, bool check_fruit = true) const;

    bool canGenFruit() const;

    void gameOver();

signals:

    void updateTime(int time, int score);

    void updatePhase(int phase);
};


#endif //SNAKE_GAMEPLANE_H

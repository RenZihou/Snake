// gameplane.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

#include "gameplane.h"
#include "ui_GamePlane.h"
#include "config.h"

#include <ctime>
#include <fstream>
#include <iomanip>

#include <QFileDialog>
#include <QDebug>
#include <QPainterPath>

void to_json(json &j, const Point &p) {
    std::vector<int> cord{p.x, p.y};
    j = cord;
}

void from_json(const json &j, Point &p) {
    p.x = j[0];
    p.y = j[1];
}

GamePlane::GamePlane(QWidget *parent) :
        QWidget(parent), 
        pixel(static_cast<int>(Config::config().get_num_setting("pixel_per_grid"))),
        fruit_bonus(static_cast<int>(Config::config().get_num_setting("fruit_bonus"))),
        speedup(Config::config().get_num_setting("fruit_speedup")),
        max_speed(Config::config().get_num_setting("max_speed")),
        width(static_cast<int>(Config::config().get_num_setting("plane_width")) + 2),
        height(static_cast<int>(Config::config().get_num_setting("plane_height")) + 2),
        speed(Config::config().get_num_setting("init_speed")),
        timer(new QTimer(this)),
        ui(new Ui::GamePlane) {
    std::random_device rd{};
    // this->mt = new std::mt19937(rd());
    // ==== NOTICE begin ====
    // MinGW-w64 has a fatal bug before 10.0
    // the random device doesn't work (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85494)
    // so i have to choose a not recommended way (use time as seed)
    // ==== NOTICE end ====
    this->mt = new std::mt19937(std::time(nullptr));
    this->gen_fruit_x = new std::uniform_int_distribution<int>(1, this->width - 2);
    this->gen_fruit_y = new std::uniform_int_distribution<int>(1, this->height - 2);

    ui->setupUi(this);
    this->setFixedSize(width * pixel, height * pixel);
    this->setFocusPolicy(Qt::StrongFocus);
    for (int offset = static_cast<int>(Config::config().get_num_setting("init_len"));
         offset; --offset) {
        this->snake.push_back({.x=width / 2 - offset, .y=height / 2});
    }
    QObject::connect(timer, &QTimer::timeout, this, &GamePlane::_update);
}

GamePlane::~GamePlane() {
    delete timer;
    delete ui;
    delete gen_fruit_x;
    delete gen_fruit_y;
}

void GamePlane::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    {  // paint background
        QPainterPath path;
        QRectF background(0, 0, width * pixel, height * pixel);
        path.addRect(background);
        std::string color_name = Config::config().get_str_setting("background_color");
        QColor color(QString(color_name.c_str()).toUInt(nullptr, 16));
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawPath(path);
    }
    {  // paint solid wall
        QPainterPath path;
        for (int x = 0; x < width; ++x) {
            QRectF top(x * pixel, 0, pixel, pixel);
            QRectF bottom(x * pixel, (height - 1) * pixel, pixel, pixel);
            path.addRect(top);
            path.addRect(bottom);
        }
        for (int y = 1; y < height - 1; ++y) {
            QRectF left(0, y * pixel, pixel, pixel);
            QRectF right((width - 1) * pixel, y * pixel, pixel, pixel);
            path.addRect(left);
            path.addRect(right);
        }
        std::string color_name = Config::config().get_str_setting("wall_color");
        QColor color(QString(color_name.c_str()).toUInt(nullptr, 16));
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawPath(path);
    }
    {  // paint wall
        QPainterPath path;
        for (const auto &w : wall) {
            QRectF w_rect(w.x * pixel, w.y * pixel, pixel, pixel);
            path.addRect(w_rect);
        }
        std::string color_name = Config::config().get_str_setting("wall_color");
        QColor color(QString(color_name.c_str()).toUInt(nullptr, 16));
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawPath(path);
    }
    {  // paint snake
        QPainterPath path;
        path.setFillRule(Qt::OddEvenFill);
        for (const auto &s : snake) {
            QRectF s_rect(s.x * pixel, s.y * pixel, pixel, pixel);
            path.addRect(s_rect);
        }
        QRectF s_head(snake.rbegin()->x * pixel + 0.3 * pixel,
                      snake.rbegin()->y * pixel + 0.3 * pixel,
                      0.4 * pixel, 0.4 * pixel);
        path.addEllipse(s_head);
        std::string color_name = Config::config().get_str_setting("snake_color");
        QColor color(QString(color_name.c_str()).toUInt(nullptr, 16));
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawPath(path);
    }
    {  // paint fruit
        QPainterPath path;
        for (const auto &f : fruit) {
            QRectF f_rect(f.x * pixel, f.y * pixel, pixel, pixel);
            path.addRect(f_rect);
        }
        std::string color_name = Config::config().get_str_setting("fruit_color");
        QColor color(QString(color_name.c_str()).toUInt(nullptr, 16));
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawPath(path);
    }
}

void GamePlane::keyPressEvent(QKeyEvent *event) {
    QWidget::keyPressEvent(event);
    if (phase != GAME) return;
    switch (event->key()) {
        case Qt::Key_A:
        case Qt::Key_Left:
            if (real_direction != RIGHT) direction = LEFT;
            break;
        case Qt::Key_W:
        case Qt::Key_Up:
            if (real_direction != DOWN) direction = UP;
            break;
        case Qt::Key_S:
        case Qt::Key_Down:
            if (real_direction != UP) direction = DOWN;
            break;
        case Qt::Key_D:
        case Qt::Key_Right:
            if (real_direction != LEFT) direction = RIGHT;
            break;
    }
}

void GamePlane::mousePressEvent(QMouseEvent *event) {
    if (phase != INIT) return;
    if (event->button() == Qt::LeftButton) {
        Point press = {.x=static_cast<int>(event->position().x()) / pixel, 
                       .y=static_cast<int>(event->position().y()) / pixel};
        if (press.x == 0 || press.y == 0 || press.x == width - 1 || press.y == width - 1) return;
        for (auto it = wall.begin(); it != wall.end(); ++it) {
            if (*it == press) {
                wall.erase(it);
                QWidget::repaint();
                return;
            }
        }
        wall.push_back(press);
        QWidget::repaint();
    }
}

void GamePlane::actionStart() {
    phase = GAME;
    timer->start(static_cast<int>(1000.0 / speed));
    emit updatePhase(phase);
}

void GamePlane::actionPause() {
    phase = PAUSE;
    timer->stop();
    emit updatePhase(phase);
}

void GamePlane::actionResume() {
    phase = GAME;
    timer->start(static_cast<int>(1000.0 / speed));
    emit updatePhase(phase);
}

void GamePlane::actionRestart() {
    phase = INIT;
    wall.clear();
    snake.clear();
    fruit.clear();
    for (int offset = static_cast<int>(Config::config().get_num_setting("init_len"));
         offset; --offset) {
        this->snake.push_back({.x=width / 2 - offset, .y=height / 2});
    }
    speed = Config::config().get_num_setting("init_speed");
    direction = RIGHT;
    growth = 0;
    time = 0;
    score = 0;
    timer->stop();
    QWidget::repaint();
    emit updatePhase(phase);
}

void GamePlane::actionSave() {
    json state;
    state["plane_width"] = width - 2;
    state["plane_height"] = height - 2;
    state["time"] = time;
    state["score"] = score;
    state["direction"] = direction;
    state["growth"] = growth;
    state["speed"] = speed;
    state["snake"] = snake;
    state["wall"] = wall;
    state["fruit"] = fruit;

    std::string file_name = QFileDialog::getSaveFileName(
            this, "Save", "../saves",
            tr("SNAKE Saves (*.snake)")).toStdString();
    std::ofstream save_file(file_name);
    save_file << std::setw(4) << state << std::endl;
}

void GamePlane::actionLoad() {
    phase = PAUSE;

    std::string file_name = QFileDialog::getOpenFileName(
            this, "Load", "../saves",
            tr("SNAKE Saves (*.snake)")).toStdString();
    std::ifstream save_file(file_name);

    json state;
    save_file >> state;
    width = static_cast<int>(state["plane_width"]) + 2;
    height = static_cast<int>(state["plane_height"]) + 2;
    time = state["time"];
    score = state["score"];
    direction = state["direction"];
    growth = state["growth"];
    speed = state["speed"];
    state["snake"].get_to(snake);
    state["wall"].get_to(wall);
    state["fruit"].get_to(fruit);

    QWidget::repaint();
    emit updateTime(time, score);
    emit updatePhase(phase);
}

void GamePlane::_update() {
    // snake move
    auto head = snake.rbegin();
    const Point new_head = {
            .x = head->x + !(direction & 1) * (1 - direction),
            .y = head->y + (direction & 1) * (direction - 2)};
    snake.push_back(new_head);
    real_direction = direction;
    if (growth) {
        --growth;
    } else {
        snake.erase(snake.begin());
    }

    // collision check
    for (auto it = snake.begin(); it != snake.end() - 1; ++it) {
        if (new_head == *it) {
            return this->gameOver();
        }
    }
    for (const auto &w : wall) {
        if (new_head == w) {
            return this->gameOver();
        }
    }
    if (new_head.x == 0 || new_head.x == width - 1 ||
        new_head.y == 0 || new_head.y == height - 1) {
        return this->gameOver();
    }

    // eat fruit
    for (auto it = fruit.begin(); it != fruit.end(); ++it) {
        if (new_head == *it) {
            fruit.erase(it);
            growth += fruit_bonus;
            speed = speed * speedup > max_speed ? max_speed : speed * speedup;
            timer->stop();
            timer->start(static_cast<int>(1000.0 / speed));
            ++score;
            break;
        }
    }

    // generate fruit
    if (canGenFruit()) {
        int max_try = 10;
        while (max_try--) {
            Point new_fruit = {.x = (*gen_fruit_x)(*mt),
                    .y = (*gen_fruit_y)(*mt)};
            if (noOverlap(new_fruit)) {
                fruit.push_back(new_fruit);
                break;
            }
        }
    }

    // repaint plane
    QWidget::repaint();

    // emit time update signal
    emit updateTime(++time, score);
}

void GamePlane::gameOver() {
    phase = END;
    timer->stop();
    emit updatePhase(phase);
}

bool GamePlane::noOverlap(Point point, bool check_snake /* = true */,
                          bool check_wall /* = true */, bool check_fruit /* = true */) const {
    if (check_snake) {
        for (const auto &s : snake) {
            if (s == point) return false;
        }
    }
    if (check_wall) {
        for (const auto &w : wall) {
            if (w == point) return false;
        }
    }
    if (check_fruit) {
        for (const auto &f : fruit) {
            if (f == point) return false;
        }
    }
    return true;
}

bool GamePlane::canGenFruit() const {
    std::uniform_real_distribution<double> dist(0, 1);

    double threshold = Config::config().get_num_setting("fruit_prob");
    int fruit_num = static_cast<int>(fruit.size());
    while (fruit_num--) {
        threshold *= Config::config().get_num_setting("fruit_prob_dec");
    }

    return dist(*mt) < threshold;
}
// mainwindow.h
// -*- encoding: utf-8 -*-
// @Author: RZH

#ifndef SNAKE_MAINWINDOW_H
#define SNAKE_MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    static MainWindow &instance() {
        static MainWindow w;
        return w;
    }

    ~MainWindow() override;

private:
    explicit MainWindow(QWidget *parent = nullptr);

    Ui::MainWindow *ui;

    void keyPressEvent(QKeyEvent *event) override;

    void updateTimeLabel(int time, int score);

    void updateButton(int phase);

    void showAbout();

    void showManual();

};


#endif //SNAKE_MAINWINDOW_H

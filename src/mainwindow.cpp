// mainwindow.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

#include "mainwindow.h"
#include "ui_MainWindow.h"

#include <QDialog>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setFocusPolicy(Qt::NoFocus);

    QObject::connect(ui->actionStart, &QAction::triggered, ui->plane, &GamePlane::actionStart);
    QObject::connect(ui->actionPause, &QAction::triggered, ui->plane, &GamePlane::actionPause);
    QObject::connect(ui->actionResume, &QAction::triggered, ui->plane, &GamePlane::actionResume);
    QObject::connect(ui->actionRestart, &QAction::triggered, ui->plane, &GamePlane::actionRestart);
    QObject::connect(ui->actionSave, &QAction::triggered, ui->plane, &GamePlane::actionSave);
    QObject::connect(ui->actionLoad, &QAction::triggered, ui->plane, &GamePlane::actionLoad);
    QObject::connect(ui->actionExit, &QAction::triggered, [=]() { QApplication::quit(); });
    QObject::connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAbout);
    QObject::connect(ui->actionManual, &QAction::triggered, this, &MainWindow::showManual);
    QObject::connect(ui->plane, &GamePlane::updateTime, this, &MainWindow::updateTimeLabel);
    QObject::connect(ui->plane, &GamePlane::updatePhase, this, &MainWindow::updateButton);

    ui->buttonStart->setAction(ui->actionStart);
    ui->buttonPause->setAction(ui->actionPause);
    ui->buttonResume->setAction(ui->actionResume);
    ui->buttonRestart->setAction(ui->actionRestart);
    ui->buttonSave->setAction(ui->actionSave);
    ui->buttonLoad->setAction(ui->actionLoad);
    ui->buttonExit->setAction(ui->actionExit);

    updateButton(INIT);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    QWidget::keyPressEvent(event);
    ui->plane->keyPressEvent(event);
}

void MainWindow::updateTimeLabel(int time, int score) {
    ui->label->setText(QString("Time: %1\nScore: %2").arg(time).arg(score));
}

void MainWindow::updateButton(int phase) {
    ui->actionStart->setEnabled(phase == INIT);
    ui->actionPause->setEnabled(phase == GAME);
    ui->actionResume->setEnabled(phase == PAUSE);
    ui->actionRestart->setEnabled(phase == PAUSE || phase == END);
    ui->actionSave->setEnabled(phase == INIT || phase == PAUSE);
    ui->actionLoad->setEnabled(phase == INIT);
}

void MainWindow::showAbout() {
    // TODO: show about
}

void MainWindow::showManual() {
    // TODO: show manual
}
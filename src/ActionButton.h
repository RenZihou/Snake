// ActionButton.h
// -*- encoding: utf-8 -*-
// @Author: RZH

#ifndef SNAKE_ACTIONBUTTON_H
#define SNAKE_ACTIONBUTTON_H

#include <QAction>
#include <QPushButton>
#include "mainwindow.h"

class ActionButton : public QPushButton {
Q_OBJECT

private:
    QAction *actionOwner = nullptr;

public:
    explicit ActionButton(QWidget *parent = nullptr);

    void setAction(QAction *action);

public slots:

    void updateButtonStatusFromAction();
};


#endif //SNAKE_ACTIONBUTTON_H

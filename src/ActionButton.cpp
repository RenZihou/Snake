// ActionButton.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

#include "ActionButton.h"

ActionButton::ActionButton(QWidget *parent) : QPushButton(parent) {
    this->setFocusPolicy(Qt::NoFocus);
}

void ActionButton::setAction(QAction *action) {
    if (actionOwner && actionOwner != action) {
        QObject::disconnect(actionOwner, &QAction::changed,
                            this, &ActionButton::updateButtonStatusFromAction);
        QObject::disconnect(this, &QPushButton::clicked,
                            actionOwner, &QAction::trigger);
    }

    actionOwner = action;
    updateButtonStatusFromAction();
    QObject::connect(actionOwner, &QAction::changed,
                     this, &ActionButton::updateButtonStatusFromAction);
    QObject::connect(this, &QPushButton::clicked,
                     actionOwner, &QAction::trigger);
}

void ActionButton::updateButtonStatusFromAction() {
    if (!actionOwner) {
        return;
    }
    setText(actionOwner->text());
    setStatusTip(actionOwner->statusTip());
    setToolTip(actionOwner->toolTip());
    setIcon(actionOwner->icon());
    setEnabled(actionOwner->isEnabled());
    setCheckable(actionOwner->isCheckable());
    setChecked(actionOwner->isChecked());
}
// main.cpp
// -*- encoding: utf-8 -*-
// @Author: RZH

#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow::instance().show();
    return QApplication::exec();
}

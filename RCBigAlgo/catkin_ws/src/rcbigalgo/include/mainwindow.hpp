#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QMouseEvent>
#include <vector>
#include "visualwidget.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private Q_SLOTS:
    void on_pbPlaceBall_clicked();
    void on_pbPlaceCup_clicked();

    void on_pEndTurn_clicked();

    void on_pbSearch_clicked();

private:
    Ui::MainWindow *ui;
    VisualWidget *canvas;
};

#endif // MAINWINDOW_H

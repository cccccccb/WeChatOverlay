#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *paintEvent) override;
    void showEvent(QShowEvent *event) override;
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "qcustomplot.h"
#include "qcplivetracer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setupPlot(QCustomPlot *customPlot);

private slots:
    void updateDataSlot();
    void updateTracerList(QCustomPlot*, QCPLiveTracer* = 0);

    void plotMouseMove(QMouseEvent*);
    void plotMouseRelease(QMouseEvent*);
    void plotItemDoubleClicked(QCPAbstractItem*,QMouseEvent*);
    void showMax(bool);
    void addToggled();

private:
    Ui::MainWindow *ui;
    QTimer dataTimer;

    QCPLiveTracer* maxTracer;
    QCPItemText* maxTextHeader;
    QCPItemText* maxText;

    QCPItemStraightLine* liveTracerTarget;
    QVector<QCPLiveTracer*> liveTracerList;
};

#endif // MAINWINDOW_H

#ifndef QCPLIVETRACER_H
#define QCPLIVETRACER_H

#include "qcustomplot.h"
#include "colorbutton.h"
#include <QDialog>
#include <QLabel>
#include <QColorDialog>
#include <QComboBox>
#include <QGridLayout>

class QCPLiveTracer : public QObject
{
    Q_OBJECT

    QCustomPlot* pplot;

    QCPItemStraightLine* hLine;
    QCPItemStraightLine* vLine;

    QCPItemTracer* tracer;

    bool movable;
    bool catched;

    QCP::Interactions p_inter;

public:
    QCPLiveTracer(QCustomPlot* plot);
    ~QCPLiveTracer();

    static QDialog::DialogCode requestConfig(QCPLiveTracer* tr);

    bool contains(QCPAbstractItem* item);

    QPointF pos();
    void updatePosition();

    QColor color();
    Qt::PenStyle style();

    void setMovable(bool m);
    void setGraph(QCPGraph* gr);
    void setInterpolating(bool in);
    void setGraphKey(double k);
    void setCoords(QPointF c);
    void setCoords(double x, double y);
    void setX(double x);
    void setY(double y);

    void setTracerStyle(QCPItemTracer::TracerStyle st);
    void setTracerPen(QPen p);
    void setTracerBrush(QColor c);
    void setTracerSize(int s);

public slots:
    void setTracerColor(QColor col);
    void setLineColor(QColor col);
    void setLineStyle(Qt::PenStyle ps);

private slots:
    void mousePress(QMouseEvent*);
    void mouseMove(QMouseEvent*);
    void mouseRelease(QMouseEvent*);

signals:
    void moved(QCustomPlot* p, QCPLiveTracer* tr);
};

#endif // QCPLIVETRACER_H

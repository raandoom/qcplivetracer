#include <float.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    maxTracer(0),
    maxTextHeader(0),
    maxText(0),
    liveTracerTarget(0)
{
    ui->setupUi(this);
    setGeometry(400, 250, 542, 390);

    setupPlot(ui->customPlot);
    setWindowTitle("QCustomPlot: Live Tracer Demo");
    statusBar()->clearMessage();
    ui->customPlot->replot();

    connect(ui->customPlot,SIGNAL(mouseMove(QMouseEvent*)),
            this,SLOT(plotMouseMove(QMouseEvent*)));
    connect(ui->customPlot,SIGNAL(mouseRelease(QMouseEvent*)),
            this,SLOT(plotMouseRelease(QMouseEvent*)));
    connect(ui->customPlot,SIGNAL(itemDoubleClick(QCPAbstractItem*,QMouseEvent*)),
            this,SLOT(plotItemDoubleClicked(QCPAbstractItem*,QMouseEvent*)));
    connect(ui->max,SIGNAL(toggled(bool)),
            this,SLOT(showMax(bool)));
    connect(ui->add,SIGNAL(toggled(bool)),
            this,SLOT(addToggled()));
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupPlot(QCustomPlot *customPlot)
{
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
    QMessageBox::critical(this, "", "You're using Qt < 4.7, the animation of the item demo needs functions that are available with Qt 4.7 to work properly");
#endif

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    QCPGraph *graph = customPlot->addGraph();
    int n = 500;
    double phase = 0;
    double k = 1;
    QVector<double> x(n), y(n);
    for (int i = 0; i < n; ++i)
    {
        x[i] = i/(double)(n-1)*34 - 17;
        y[i] = qExp(-x[i]*x[i]/20.0)*qSin(k*x[i]+phase);
    }
    graph->setData(x, y);
    graph->setPen(QPen(Qt::blue));
    graph->rescaleKeyAxis();
    customPlot->yAxis->setRange(-1.45, 1.65);
    customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);

    QFont f = customPlot->xAxis->labelFont();
    f.setPointSize(8);
    customPlot->xAxis2->setSubTickCount(0);
    customPlot->xAxis2->setAutoTicks(false);
    customPlot->xAxis2->setAutoTickLabels(false);
    customPlot->xAxis2->setTickLabelFont(f);
    connect(customPlot->xAxis,SIGNAL(rangeChanged(QCPRange)),
            customPlot->xAxis2,SLOT(setRange(QCPRange)));

    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(updateDataSlot()));
    dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}

void MainWindow::updateDataSlot()
{
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
    double secs = 0;
#else
    double secs = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
#endif

    double max_key = 0;
    double max_value = - DBL_MAX;

    // update data to make phase move:
    int n = 500;
    double phase = secs * 5;
    double k = 1;
    QVector<double> x(n), y(n);
    for (int i = 0; i < n; ++i)
    {
        x[i] = i / (double)(n - 1) * 34 - 17;
        y[i] = qExp(-x[i] * x[i] / 20.0) * qSin(k * x[i] + phase);

        if (y[i] > max_value)
        {
            max_value = y[i];
            max_key = x[i];
        }
    }
    ui->customPlot->graph()->setData(x, y);

    if (maxTracer)
    {
        maxTracer->setGraphKey(max_key);
        maxTracer->updatePosition();

        QPointF maxPoint = maxTracer->pos();
        maxTracer->setY(maxPoint.y());
        maxText->setText(QString("x: %1\ny: %2").
                         arg(maxPoint.x(),0,'f',3).
                         arg(maxPoint.y(),0,'f',3));
    }

    updateTracerList(ui->customPlot);

    ui->customPlot->replot();

    // calculate frames per second:
    double key = secs;
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key - lastFpsKey > 2) // average fps over 2 seconds
    {
        ui->statusBar->showMessage(
                    QString("%1 FPS, Total Data points: %2").
                    arg(frameCount/(key-lastFpsKey), 0, 'f', 0).
                    arg(ui->customPlot->graph(0)->data()->count()));
        lastFpsKey = key;
        frameCount = 0;
    }
}

void MainWindow::updateTracerList(QCustomPlot *p, QCPLiveTracer *tr)
{
    Q_UNUSED(tr)

    // update all tracers and top xAxis
    QVector<double> tv;
    QVector<QString> lv;
    for (int i = 0; i < liveTracerList.size(); i++)
    {
        QCPLiveTracer* tr = liveTracerList.at(i);
        tr->updatePosition();
        QPointF p = tr->pos();

        tr->setY(p.y());

        tv.append(p.x());
        lv.append(QString("%1").arg(p.y(),0,'f',3));
    }
    p->xAxis2->setTickVector(tv);
    p->xAxis2->setTickVectorLabels(lv);

    p->xAxis2->setVisible(!liveTracerList.isEmpty());
    p->replot();
}

void MainWindow::plotMouseMove(QMouseEvent *me)
{
    if (ui->add->isChecked() && liveTracerTarget)
    {
        double posx = ui->customPlot->xAxis->pixelToCoord(me->pos().x());
        liveTracerTarget->point1->setCoords(posx,0);
        liveTracerTarget->point2->setCoords(posx,1);
        ui->customPlot->replot();
    }
}

void MainWindow::plotMouseRelease(QMouseEvent *me)
{
    if (ui->add->isChecked())
    {
        if (liveTracerTarget)
        {
            ui->add->setChecked(false);

            delete liveTracerTarget;
            liveTracerTarget = 0;

            QCPLiveTracer* tr = new QCPLiveTracer(ui->customPlot);
            connect(tr,SIGNAL(moved(QCustomPlot*,QCPLiveTracer*)),
                    this,SLOT(updateTracerList(QCustomPlot*,QCPLiveTracer*)));
            tr->setInterpolating(true);
            tr->setTracerStyle(QCPItemTracer::tsSquare);
            tr->setTracerPen(QPen(Qt::darkGray));
            tr->setTracerBrush(ui->customPlot->graph()->pen().color());
            tr->setTracerSize(6);
            tr->setLineColor(Qt::darkGray);
            tr->setLineStyle(Qt::DashLine);

            double posx = ui->customPlot->xAxis->pixelToCoord(me->pos().x());

            tr->setGraph(ui->customPlot->graph());
            tr->setGraphKey(posx);
            tr->updatePosition();
            tr->setY(tr->pos().y());

            tr->setMovable(true);
            liveTracerList.append(tr);

            updateTracerList(ui->customPlot);
        }
        else
        {
            double posx = ui->customPlot->xAxis->pixelToCoord(me->pos().x());
            liveTracerTarget = new QCPItemStraightLine(ui->customPlot);
            liveTracerTarget->point1->setCoords(posx,0);
            liveTracerTarget->point2->setCoords(posx,1);

            QPen p;
            p.setStyle(Qt::DashLine);
            p.setColor(Qt::darkGray);
            liveTracerTarget->setPen(p);
        }
    }
}

void MainWindow::plotItemDoubleClicked(QCPAbstractItem *item, QMouseEvent *me)
{
    Q_UNUSED(me)

    QCustomPlot* plot = qobject_cast<QCustomPlot*>(QObject::sender());

    if (plot && item)
    {
        // maybe item is a part of tracer
        for (int i = 0; i < liveTracerList.size(); i++)
        {
            if (liveTracerList.at(i)->contains(item))
            {
                if (QCPLiveTracer::requestConfig(liveTracerList.at(i)) ==
                        QDialog::Rejected)
                {   // reject - need to delete tracer
                    delete liveTracerList.at(i);
                    liveTracerList.remove(i);
                }

                updateTracerList(ui->customPlot);
                return;
            }
        }
    }
}

void MainWindow::showMax(bool show)
{
    if (show)
    {
        delete maxTracer;
        maxTracer = new QCPLiveTracer(ui->customPlot);
        maxTracer->setInterpolating(true);
        maxTracer->setTracerStyle(QCPItemTracer::tsCircle);
        maxTracer->setTracerPen(QPen(Qt::darkGray));
        maxTracer->setTracerBrush(ui->customPlot->graph()->pen().color());
        maxTracer->setTracerSize(6);
        maxTracer->setLineColor(Qt::darkGray);
        maxTracer->setLineStyle(Qt::DashLine);
        maxTracer->setGraph(ui->customPlot->graph());

        // find current max
        double max_key = 0;
        double max_value = - DBL_MAX;
        QCPDataMapIterator mi(*ui->customPlot->graph()->data());
        while (mi.hasNext())
        {
            mi.next();
            const QCPData& d = mi.value();
            if (d.value > max_value)
            {
                max_value = d.value;
                max_key = d.key;
            }
        }
        maxTracer->setGraphKey(max_key);
        maxTracer->updatePosition();
        maxTracer->setY(maxTracer->pos().y());

        QFont f;

        if (maxTextHeader)
            ui->customPlot->removeItem(maxTextHeader);

        maxTextHeader = new QCPItemText(ui->customPlot);
        ui->customPlot->addItem(maxTextHeader);
        maxTextHeader->position->setType(QCPItemPosition::ptAxisRectRatio);
        maxTextHeader->position->setCoords(0,0);
        maxTextHeader->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
        maxTextHeader->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        maxTextHeader->setText("Maximum value:");
        maxTextHeader->setPadding(QMargins(20,0,0,0));
        f = maxTextHeader->font();
        f.setPixelSize(12);
        f.setBold(true);
        f.setUnderline(true);
        maxTextHeader->setFont(f);

        if (maxText)
            ui->customPlot->removeItem(maxText);
        maxText = new QCPItemText(ui->customPlot);
        ui->customPlot->addItem(maxText);
        maxText->position->setParentAnchor(maxTextHeader->bottomLeft);
        maxText->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
        maxText->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        maxText->setText(QString("x: %1\ny: %2").arg(max_key).arg(max_value));
        maxText->setPadding(QMargins(20,0,0,0));
    }
    else
    {
        delete maxTracer;
        maxTracer = 0;

        if (maxTextHeader)
            ui->customPlot->removeItem(maxTextHeader);
        maxTextHeader = 0;

        if (maxText)
            ui->customPlot->removeItem(maxText);
        maxText = 0;
    }

    ui->customPlot->replot();
}

void MainWindow::addToggled()
{
    delete liveTracerTarget;
    liveTracerTarget = 0;
    ui->customPlot->replot();
}

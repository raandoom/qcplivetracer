#include "qcplivetracer.h"

QCPLiveTracer::QCPLiveTracer(QCustomPlot *plot) :
    pplot(plot),
    hLine(new QCPItemStraightLine(plot)),
    vLine(new QCPItemStraightLine(plot)),
    tracer(new QCPItemTracer(plot)),
    movable(false),
    catched(false),
    p_inter(plot->interactions())
{
    plot->addItem(tracer);
    plot->addItem(hLine);
    plot->addItem(vLine);

    connect(plot,SIGNAL(mousePress(QMouseEvent*)),SLOT(mousePress(QMouseEvent*)));
    connect(plot,SIGNAL(mouseMove(QMouseEvent*)),SLOT(mouseMove(QMouseEvent*)));
    connect(plot,SIGNAL(mouseRelease(QMouseEvent*)),SLOT(mouseRelease(QMouseEvent*)));
}

QCPLiveTracer::~QCPLiveTracer()
{
    pplot->removeItem(tracer);
    pplot->removeItem(hLine);
    pplot->removeItem(vLine);
}

QDialog::DialogCode QCPLiveTracer::requestConfig(QCPLiveTracer *tr)
{
    QDialog dialog;
    dialog.setWindowTitle("Edit");

    // color
    QLabel mColor("Color");
    mColor.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ColorButton mColorEdit;
    QColor c = tr->color();
    mColorEdit.setColor(c);
    QColorDialog cd;
    cd.setCurrentColor(c);
    QObject::connect(&mColorEdit,SIGNAL(clicked()),&cd,SLOT(exec()));
    QObject::connect(&cd,SIGNAL(colorSelected(QColor)),&mColorEdit,SLOT(setColor(QColor)));
    QObject::connect(&cd,SIGNAL(colorSelected(QColor)),tr,SLOT(setLineColor(QColor)));
    // style
    QLabel mStyle("Line style");
    mStyle.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QComboBox mStyleEdit;
    QSize sz(120,mStyleEdit.sizeHint().height() / 2);
    mStyleEdit.setIconSize(sz);
    for (int style = Qt::SolidLine; style < Qt::CustomDashLine; style++)
    {
        QPixmap pix(sz);
        pix.fill(Qt::white);

        QBrush brush(Qt::black);
        QPen pen(brush,2.5,(Qt::PenStyle)style);

        QPainter painter(&pix);
        painter.setPen(pen);
        painter.drawLine(0,sz.height() / 2,sz.width(),sz.height() / 2);
        mStyleEdit.addItem(QIcon(pix),QString());
    }
    mStyleEdit.setCurrentIndex(tr->style() - 1);
    // buttons
    QPushButton mDelete("Delete");
    QObject::connect(&mDelete,SIGNAL(clicked()),&dialog,SLOT(reject()));
    QPushButton mSave("Save");
    QObject::connect(&mSave,SIGNAL(clicked()),&dialog,SLOT(accept()));

    QGridLayout grid;
    grid.addWidget(&mColor,0,0);
    grid.addWidget(&mColorEdit,0,1);
    grid.addWidget(&mStyle,1,0);
    grid.addWidget(&mStyleEdit,1,1);

    grid.addWidget(&mDelete,2,0);
    grid.addWidget(&mSave,2,1);

    dialog.setLayout(&grid);
    dialog.setFixedSize(dialog.sizeHint());

    int result = dialog.exec();
    if (result == QDialog::Accepted)
    {
        tr->setLineStyle(Qt::PenStyle(mStyleEdit.currentIndex() + 1));
    }

    return QDialog::DialogCode(result);
}

bool QCPLiveTracer::contains(QCPAbstractItem *item)
{
    return ((hLine == item) ||
            (vLine == item) ||
            (tracer == item));
}

QPointF QCPLiveTracer::pos()
{
    return tracer->position->coords();
}

void QCPLiveTracer::updatePosition()
{
    tracer->updatePosition();
}

QColor QCPLiveTracer::color()
{
    return tracer->pen().color();
}

Qt::PenStyle QCPLiveTracer::style()
{
    return hLine->pen().style();
}

void QCPLiveTracer::setMovable(bool m)
{
    movable = m;
}

void QCPLiveTracer::setGraph(QCPGraph *gr)
{
    tracer->setGraph(gr);
}

void QCPLiveTracer::setInterpolating(bool in)
{
    tracer->setInterpolating(in);
}

void QCPLiveTracer::setTracerStyle(QCPItemTracer::TracerStyle st)
{
    tracer->setStyle(st);
}

void QCPLiveTracer::setTracerPen(QPen p)
{
    tracer->setPen(p);
}

void QCPLiveTracer::setTracerBrush(QColor c)
{
    tracer->setBrush(c);
}

void QCPLiveTracer::setTracerSize(int s)
{
    tracer->setSize(s);
}

void QCPLiveTracer::setGraphKey(double k)
{
    setX(k);
    tracer->setGraphKey(k);
}

void QCPLiveTracer::setCoords(QPointF c)
{
    tracer->position->setCoords(c);

    hLine->point1->setCoords(c);
    vLine->point1->setCoords(c);

    hLine->point2->setCoords(c.x() + 1,c.y());
    vLine->point2->setCoords(c.x(),c.y() + 1);
}

void QCPLiveTracer::setCoords(double x, double y)
{
    setCoords(QPointF(x,y));
}

void QCPLiveTracer::setX(double x)
{
    QPointF p = tracer->position->coords();
    tracer->position->setCoords(x,p.y());

    vLine->point1->setCoords(x,p.y());
    vLine->point2->setCoords(x,p.y() + 1);
}

void QCPLiveTracer::setY(double y)
{
    QPointF p = tracer->position->coords();
    tracer->position->setCoords(p.x(),y);

    hLine->point1->setCoords(p.x(),y);
    hLine->point2->setCoords(p.x() + 1,y);
}

void QCPLiveTracer::setTracerColor(QColor col)
{
    QBrush brush = tracer->brush();
    brush.setColor(col);
    tracer->setBrush(brush);
}

void QCPLiveTracer::setLineColor(QColor col)
{
    QPen pen = hLine->pen();
    pen.setColor(col);
    hLine->setPen(pen);
    vLine->setPen(pen);

    pen = tracer->pen();
    pen.setColor(col);
    tracer->setPen(pen);
}

void QCPLiveTracer::setLineStyle(Qt::PenStyle ps)
{
    QPen pen = hLine->pen();
    pen.setStyle(ps);
    hLine->setPen(pen);
    vLine->setPen(pen);
}

void QCPLiveTracer::mousePress(QMouseEvent* me)
{
    catched = (movable && (pplot->itemAt(me->pos()) == vLine));

    if (catched)
    {
        p_inter = pplot->interactions();
        pplot->setInteraction(QCP::iRangeDrag,false);
    }
}

void QCPLiveTracer::mouseMove(QMouseEvent* me)
{
    if (catched)
    {
        double posx = pplot->xAxis->pixelToCoord(me->pos().x());
        setGraphKey(posx);
        emit moved(pplot,this);
        pplot->replot();
    }
}

void QCPLiveTracer::mouseRelease(QMouseEvent*)
{
    if (catched)
    {
        catched = false;
        pplot->setInteractions(p_inter);
    }
}

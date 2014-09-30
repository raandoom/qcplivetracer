#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>

class ColorButton : public QPushButton
{
    Q_OBJECT
public slots:
    void setColor(QColor c)
    {
        setStyleSheet(QString("background-color:rgb(%1,%2,%3)").
                                 arg(c.red()).
                                 arg(c.green()).
                                 arg(c.blue()));
    }
};

#endif // COLORBUTTON_H

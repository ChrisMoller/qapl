#ifndef COMPLEXSPINBOX_H
#define COMPLEXSPINBOX_H

#include <QtWidgets>
#include <complex>

#include "enums.h"

typedef enum {
  WHICH_REAL,
  WHICH_IMAG
} which_e;

class
ComplexSpinBox : public QAbstractSpinBox
{
    Q_OBJECT

public:
    ComplexSpinBox (QWidget *parent = nullptr);
    ~ComplexSpinBox ();
    double getReal ();
    double getImag ();
    std::complex<double> getComplex ();
    void setReal (double rv);
    void setImag (double iv);
    void setComplex (double rv, double iv);
    void setComplex (std::complex<double> cv);

 Q_SIGNALS:
    void valueChanged ();

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

private:
    void parseComplex (QString txt);
    void incdecValue (which_e which, double val);
    void formatValue ();
 
    QValidator  *validator;
    double real;
    double imag;
    QTimer *timer;
    which_e which;
    double incr;
    bool mousePressActive;
    coord_e coord_type;
};
#endif // COMPLEXSPINBOX_H

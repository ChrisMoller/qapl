#ifndef SCIDOUBLESPINBOX_H
#define SCIDOUBLESPINBOX_H

#include <QtWidgets>

#include "enums.h"

class
SciDoubleSpinBox : public QAbstractSpinBox
{
    Q_OBJECT

public:
    typedef enum {
      Min,
      Max,
      Unset
    } SpinType;

    SciDoubleSpinBox (SpinType t, QWidget *parent = nullptr);
    ~SciDoubleSpinBox ();
    double getValue ();
    void setValue (double v);
    void setLockBox (QCheckBox *lb) { lockBox = lb; }
    //    double clampValue (double v);

 Q_SIGNALS:
    void valueChanged ();
    
protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    bool eventFilter(QObject *object, QEvent *event);
    
 private:
    double value;
    QCheckBox *lockBox;
    SpinType stype;
    bool mousePressActive;
    QTimer *timer;
    double incr;
};

#endif // SCIDOUBLESPINBOX_H

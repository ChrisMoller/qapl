
#include <QtWidgets>
#include <complex>

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
#if 0
  void valueChanged ();
#endif

protected:
  void keyPressEvent(QKeyEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

private slots:

private:
  void parseComplex (QString txt);
 void incdecValue (which_e which, double val);
 
 QValidator  *validator;
 double real;
 double imag;
};

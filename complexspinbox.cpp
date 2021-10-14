#include <QtWidgets>

#include "complexspinbox.h"

#ifndef toCString
#define toCString(v) ((v).toStdString ().c_str ())
#endif

#define RE_FP "([-+]?([0-9]+(\\.[0-9]+)?|\\.[0-9]+))([eE]([-+]?[0-9]+))?"
//#define RE_FP "[^jJ]*"
static QString cpxval = QString ("(%1)([jJ](%1))?").arg (RE_FP);
static QRegularExpression rx(cpxval);

/***
list[0] = 7.6e9j77		// whole thing
list[1] = 7.6e9			// real
list[2] = 7.6			// real mant
list[3] = 7.6			// real mant
list[4] = .6
list[5] = e9
list[6] = 9			// real exp
list[7] = 
list[8] =  			// imag
list[9] =			// imag mant
list[10] = 			// imag mant
list[11] = 			
list[12] = 			
list[13] =			//imag exp
 ***/

#if 0
QValidator::State validate(QString &input, int &pos)
{
  return QValidator::Acceptable;
}
#endif

  void
ComplexSpinBox::parseComplex (QString txt)
{
  QRegularExpressionMatch match = rx.match (txt);
  if (match.hasMatch ()) {
    real = match.captured (1).toFloat ();
    imag = match.captured (8).toFloat ();
    Q_EMIT valueChanged ();
  }
}

double
ComplexSpinBox::getReal ()
{
  return real;
}


double
ComplexSpinBox::getImag ()
{
  return imag;
}


std::complex<double>
ComplexSpinBox::getComplex ()
{
  std::complex<double> val (real, imag);
  return val;
}

void
ComplexSpinBox::setReal (double rv)
{
  real = rv;
  QString txt = QString ("%1j%2").arg (real).arg (imag);
  this->lineEdit ()->setText (txt);
  Q_EMIT valueChanged ();
}

void
ComplexSpinBox::setImag (double iv)
{
  imag = iv;
  QString txt = QString ("%1j%2").arg (real).arg (imag);
  this->lineEdit ()->setText (txt);
  Q_EMIT valueChanged ();
}

void
ComplexSpinBox::setComplex (double rv, double iv)
{
  real = rv;
  imag = iv;
  QString txt = QString ("%1j%2").arg (real).arg (imag);
  this->lineEdit ()->setText (txt);
  Q_EMIT valueChanged ();
}

void
ComplexSpinBox::setComplex (std::complex<double> cv)
{
  real = cv.real ();
  imag = cv.imag ();
  QString txt = QString ("%1j%2").arg (real).arg (imag);
  this->lineEdit ()->setText (txt);
  Q_EMIT valueChanged ();
}

void ComplexSpinBox::incdecValue (which_e wch, double val)
{
  if (wch == WHICH_REAL)
    real += val;
  else
    imag += val;
  QString txt = QString ("%1j%2").arg (real).arg (imag);
  this->lineEdit ()->setText (txt);
  Q_EMIT valueChanged ();
}

void ComplexSpinBox::keyPressEvent(QKeyEvent *keyEvent)
{
  bool handled = false;
  Qt::KeyboardModifiers mods = keyEvent->modifiers ();
  bool ctl = (0 == (mods & Qt::ControlModifier));
  which_e which = ctl ? WHICH_REAL : WHICH_IMAG;
  switch(keyEvent->key()) {
  case Qt::Key_PageUp:
    incdecValue (which, 10.0);
    handled = true;
    break;
  case Qt::Key_PageDown:
    incdecValue (which, -10.0);
    handled = true;
    break;
  case Qt::Key_Up:
    incdecValue (which, 1.0);
    handled = true;
    break;
  case Qt::Key_Down:
    incdecValue (which, -1.0);
    handled = true;
    break;
  }
  if (!handled)
    QAbstractSpinBox::keyPressEvent(keyEvent);
}

void ComplexSpinBox::mousePressEvent(QMouseEvent *mouseEvent)
{
  bool handled = false;
  if (mouseEvent->button () == Qt::LeftButton) {
    Qt::KeyboardModifiers mods = mouseEvent->modifiers ();
    bool ctl = (0 == (mods & Qt::ControlModifier));
    which = ctl ? WHICH_REAL : WHICH_IMAG;

    int y = mouseEvent->pos ().y ();
    int hgt = this->height ();
    incr = (y < hgt / 2) ? 1.0 : -1.0;

    incdecValue (which, incr);
    handled = true;
  }
  if (handled)
    timer->start (100);
  else
    QAbstractSpinBox::mousePressEvent(mouseEvent);
}

void ComplexSpinBox::wheelEvent(QWheelEvent *wheelEvent)
{
  Qt::KeyboardModifiers mods = wheelEvent->modifiers ();
  bool ctl = (0 == (mods & Qt::ControlModifier));
  which_e which = ctl ? WHICH_REAL : WHICH_IMAG;

  int y = wheelEvent->angleDelta().y();
  double incr = (0 > y) ? 1.0 : -1.0;

  incdecValue (which, incr);
}

bool ComplexSpinBox::eventFilter(QObject *object, QEvent *event)
{
  if (object == this && event->type() == QEvent::MouseButtonRelease) {
    //    QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
    timer->stop ();
  }
  return false;
}

ComplexSpinBox::ComplexSpinBox (QWidget *parent)
  : QAbstractSpinBox (parent)
{
  real = 0.0;
  imag = 0.0;
  timer = new QTimer(this);
  connect (timer, &QTimer::timeout,
	   [=]() {
	     incdecValue (which, incr);
	   });
  validator = new QRegularExpressionValidator(rx, this);
  
  QLineEdit *line = new QLineEdit ();
  line->setValidator(validator);
  QString txt = QString ("%1j%2").arg (real).arg (imag);
  line->setText (txt);
  this->setLineEdit (line);
  this->setAccelerated (true);
  this->setAlignment (Qt::AlignHCenter);
  this->setMinimumWidth (100);
  connect (this, &QAbstractSpinBox::editingFinished,
	   [=]() {
	     QString txt = this->lineEdit ()->text ();
	     parseComplex (txt);
	   });
  this->installEventFilter(this);
}

ComplexSpinBox::~ComplexSpinBox ()
{
  delete validator;
}


#include <QtWidgets>
#include <values.h>

#include "enums.h"
#include "complexspinbox.h"

#ifndef toCString
#define toCString(v) ((v).toStdString ().c_str ())
#endif

#define MOUSE_BUTTON_DELAY 100

#define RE_SEPARATOR "[j<]*"
#define RE_COORD_TYPE "[drp○]*"
static QString cpxval
  = QString ("(([^j<]*)(([j<])([^drp○]*)([drp○])?)?)");
/***
    [0] 	67.8<9e7d	j9e7	<9e7r
    [1]		67.8<9e7d	j9e7	<9e7r
    [2]		67.8
    [3]		<9e7d		j9e7	<9e7r
    [4]		<		j	<
    [5]		9e7		9e7	9e7
    [6]		d			r
 ***/
#define MATCH_REAL	2
#define MATCH_IMAG	5
#define MATCH_SEPARATOR	4
#define MATCH_COORD	6

static QRegularExpression rx(cpxval,
			     QRegularExpression::CaseInsensitiveOption);
#if 0
QValidator::State validate(QString &input, int &pos)
{
  return QValidator::Acceptable;
}
#endif

void ComplexSpinBox::parseComplex (coord_e &coord_type,
				   double &real, double &imag, QString txt)
{
  QRegularExpressionMatch match = rx.match (txt);
  if (match.hasMatch ()) {
    if (match.lastCapturedIndex() >= 2) {
      QString realString = match.captured (MATCH_REAL).trimmed ();
      if (realString.endsWith (QChar ('p'), Qt::CaseInsensitive)) {
	realString.chop (1);
	real = M_PI * realString.toFloat ();
      }
      else real = realString.toFloat ();
      imag = 0.0;
      coord_type = COORD_RECTANGULAR;
      if (match.lastCapturedIndex() >= 5) {
	imag = match.captured (MATCH_IMAG).toFloat ();
	if (0 !=
	    match.captured (MATCH_SEPARATOR)
	    .compare ("j", Qt::CaseInsensitive)) {
	  // not j, therefore must be polar coord
	  double phase = imag;
	  if (match.lastCapturedIndex() >= 6) {
	    if (0 ==
		match.captured (MATCH_COORD)
		.compare ("d", Qt::CaseInsensitive)) {
	      phase *= M_PI/180.0;		// d for degrees
	      coord_type = COORD_POLAR_DEGREES;
	    }
	    else if (0 !=
		     match.captured (MATCH_COORD) 
		     .compare ("r", Qt::CaseInsensitive)) {
	      phase *= M_PI;		// not r so must be pi
	      coord_type = COORD_POLAR_PI;
	    }
	    else
	      coord_type = COORD_POLAR;
	  }
	  double mag = real;
	  real = mag * cos (phase);
	  imag = mag * sin (phase);
	}
	else
	coord_type = COORD_RECTANGULAR;
      }
    }
  }
}

double ComplexSpinBox::getReal ()
{
  return real;
}


double ComplexSpinBox::getImag ()
{
  return imag;
}


std::complex<double> ComplexSpinBox::getComplex ()
{
  std::complex<double> val (real, imag);
  return val;
}

void ComplexSpinBox::formatValue ()
{
  QString txt;
  switch(coord_type) {
  case COORD_RECTANGULAR:
    txt = QString ("%1j%2").arg (real).arg (imag);
    break;
  case COORD_POLAR:
    {
      double phase = atan2 (imag, real);
      double mag   = hypot (imag, real);
      txt = QString ("%1<%2").arg (mag).arg (phase);
    }
    break;
  case COORD_POLAR_DEGREES:
    {
      double phase = atan2 (imag, real);
      phase *= 180.0 / M_PI;
      double mag   = hypot (imag, real);
      txt = QString ("%1<%2r").arg (mag).arg (phase);
    }
    break;
  case COORD_POLAR_PI:
    {
      double phase = atan2 (imag, real);
      phase /= M_PI;
      double mag   = hypot (imag, real);
      txt = QString ("%1<%2○").arg (mag).arg (phase);
    }
    break;
  }
  string = txt;
  this->lineEdit ()->setText (txt);
}

void ComplexSpinBox::setReal (double rv)
{
  real = rv;
  formatValue ();
  //  QString txt = QString ("%1j%2").arg (real).arg (imag);
  //  this->lineEdit ()->setText (txt);
  Q_EMIT valueChanged ();
}

void ComplexSpinBox::setImag (double iv)
{
  imag = iv;
  formatValue ();
  //  QString txt = QString ("%1j%2").arg (real).arg (imag);
  //  this->lineEdit ()->setText (txt);
  Q_EMIT valueChanged ();
}

void ComplexSpinBox::setComplex (double rv, double iv)
{
  real = rv;
  imag = iv;
  formatValue ();
  //  QString txt = QString ("%1j%2").arg (real).arg (imag);
  //  this->lineEdit ()->setText (txt);
  Q_EMIT valueChanged ();
}

void ComplexSpinBox::setComplex (std::complex<double> cv)
{
  real = cv.real ();
  imag = cv.imag ();
  formatValue ();
  //  QString txt = QString ("%1j%2").arg (real).arg (imag);
  //  this->lineEdit ()->setText (txt);
  Q_EMIT valueChanged ();
}

void ComplexSpinBox::incdecValue (which_e wch, double val)
{
  if (wch == WHICH_REAL)
    real += val;
  else
    imag += val;
  formatValue ();
  //  QString txt = QString ("%1j%2").arg (real).arg (imag);
  //  this->lineEdit ()->setText (txt);
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
    mousePressActive = true;
  }
  if (handled) {
    // wait .5 sec.  if mouse button still active, repeat
    QTime dieTime= QTime::currentTime().addMSecs(500);
    while (QTime::currentTime() < dieTime)
      QCoreApplication::processEvents(QEventLoop::AllEvents,
				      MOUSE_BUTTON_DELAY);
    if (mousePressActive)
      timer->start (MOUSE_BUTTON_DELAY);
  }
  else
    QAbstractSpinBox::mousePressEvent(mouseEvent);
}

bool ComplexSpinBox::eventFilter(QObject *object, QEvent *event)
{
  if (object == this && event->type() == QEvent::MouseButtonRelease) {
    //    QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
    mousePressActive = false;
    timer->stop ();
  }
  return false;
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

ComplexSpinBox::ComplexSpinBox (QWidget *parent)
  : QAbstractSpinBox (parent)
{
  mousePressActive = false;
  real = 0.0;
  imag = 0.0;
  coord_type = COORD_RECTANGULAR;
  timer = new QTimer(this);
  connect (timer, &QTimer::timeout,
	   [=]() {
	     incdecValue (which, incr);
	   });
  validator = new QRegularExpressionValidator(rx, this);
  
  QLineEdit *line = new QLineEdit ();
  line->setValidator(validator);
  //  QString txt = QString ("%1j%2").arg (real).arg (imag);
  formatValue ();
  //  line->setText (txt);
  this->setLineEdit (line);
  this->setAccelerated (true);
  this->setAlignment (Qt::AlignHCenter);
  this->setMinimumWidth (100);
  connect (this, &QAbstractSpinBox::editingFinished,
	   [=]() {
	     QString txt = this->lineEdit ()->text ();
	     string = txt;
	     parseComplex (coord_type, real, imag, txt);
	     Q_EMIT valueChanged ();
	   });
  this->installEventFilter(this);
}

ComplexSpinBox::~ComplexSpinBox ()
{
  delete validator;
}



#include <values.h>

#include "enums.h"
#include "scidoublespinbox.h"

#ifndef toCString
#define toCString(v) ((v).toStdString ().c_str ())
#endif

#define MOUSE_BUTTON_DELAY 100

SciDoubleSpinBox::SciDoubleSpinBox (SpinType t, QWidget *parent)
  : QAbstractSpinBox (parent)
{
  stype = t;
  value = 0.0;
  timer = new QTimer(this);
  connect (timer, &QTimer::timeout,
	   [=]() {
	     value += incr;
	     this->lineEdit ()->setText (QString::number (value));
	     Q_EMIT valueChanged ();
	   });

  //  this->setFocusPolicy (Qt::StrongFocus);
  
  QLineEdit *line = new QLineEdit ();
  this->setLineEdit (line);
  this->lineEdit ()->setText (QString::number (value));
  this->setAccelerated (true);
  this->setAlignment (Qt::AlignHCenter);
  this->setMinimumWidth (100);
  connect (this, &QAbstractSpinBox::editingFinished,
	   [=]() {
	     value = (this->lineEdit ()->text ()).toFloat ();
	     Q_EMIT valueChanged ();
	   });
  this->installEventFilter(this);
}

double SciDoubleSpinBox::getValue ()
{
#if 1
  return value;
#else
  double rc;
  if (lockBox->checkState () == Qt::Unchecked)
    rc = (stype == Min) ? -MAXDOUBLE : MAXDOUBLE;
  else
    rc = value;
  return rc;
#endif
}

#if 0
double SciDoubleSpinBox::clampValue (double v)
{
  if (lockBox->checkState () == Qt::Unchecked)
    value = v;
  this->lineEdit ()->setText (QString::number (value));
  return value;
}
#endif

void SciDoubleSpinBox::keyPressEvent(QKeyEvent *keyEvent)
{
  bool handled = false;
  //  Qt::KeyboardModifiers mods = keyEvent->modifiers ();
  switch(keyEvent->key()) {
  case Qt::Key_PageUp:
    value += 10.0;
    handled = true;
    break;
  case Qt::Key_PageDown:
    value -= 10.0;
    handled = true;
    break;
  case Qt::Key_Up:
    value += 1.0;
    handled = true;
    break;
  case Qt::Key_Down:
    value -= 1.0;
    handled = true;
    break;
  }
  if (handled) {
    this->lineEdit ()->setText (QString::number (value));
    Q_EMIT valueChanged ();
    //    lockBox->setCheckState (Qt::Checked);
  }
  else
    QAbstractSpinBox::keyPressEvent(keyEvent);
}

void SciDoubleSpinBox::mousePressEvent(QMouseEvent *mouseEvent)
{
  bool handled = false;
  if (mouseEvent->button () == Qt::LeftButton) {
    int y = mouseEvent->pos ().y ();
    int hgt = this->height ();
    incr = (y < hgt / 2) ? 1.0 : -1.0;

    value += incr;
    handled = true;
    mousePressActive = true;
  }
  if (handled) {
    this->lineEdit ()->setText (QString::number (value));
    Q_EMIT valueChanged ();
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

void SciDoubleSpinBox::setValue (double v)
{
  value = v;
  this->lineEdit ()->setText (QString::number (value));
}

bool SciDoubleSpinBox::eventFilter(QObject *object, QEvent *event)
{
  if (object == this && event->type() == QEvent::MouseButtonRelease) {
    //    QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
    mousePressActive = false;
    timer->stop ();
  }
  return false;
}

SciDoubleSpinBox::~SciDoubleSpinBox ()
{
}


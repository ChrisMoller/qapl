#include <QMenuBar>
#include <QChart>
#include <QChartView>
#include <QValueAxis>
#include <QXYSeries>

#include <complex>

#include <values.h>
#include <apl/libapl.h>

#include "mainwindow.h"
#include "complexspinbox.h"
#include "plot2dwindow.h"

#define PLOTVAR "plotvarλ"
#define IDXVAR  "idxvarλ"

void Plot2DWindow::drawCurve ()
{
  QString aplExpr = aplExpression->text ();
  if (!aplExpr.isEmpty ()) {
    double realIncr   = (realFinal - realInit) / (double)resolution;
    double imagIncr   = (imagFinal - imagInit) / (double)resolution;
    QString cmd = QString ("%1←%2j%3+((⍳%4)-⎕io)×%5j%6")
      .arg (IDXVAR).arg (realInit).arg (imagInit).arg (resolution+1)
      .arg (realIncr).arg (imagIncr);
    mw->processLine (false, cmd);
    aplExpr.replace (QString ("%1"), QString (IDXVAR));
    cmd = QString ("%1←%2").arg (PLOTVAR, aplExpr);
    mw->processLine (false, cmd);
    mw->processLine (false, cmd);
    QString pv (PLOTVAR);
    APL_value result = get_var_value (pv.toUtf8 (), "drawCurve");
    int resultRank		= get_rank (result);
    int resultElementCount	= get_element_count (result);
    std::vector<std::complex<double>> resultVector;
    bool resultValid = true;
    bool isComplex   = false;
    double realMax = -MAXDOUBLE;
    double realMin =  MAXDOUBLE;
    double imagMax = -MAXDOUBLE;
    double imagMin =  MAXDOUBLE;
    for (int i = 0; i < resultElementCount; i++) {
      int type = get_type (result, i);
      switch(type) {
      case CCT_CHAR:
      case CCT_POINTER:
	resultValid = false;
	break;
      case CCT_INT:
	{
	  std::complex<double> val ((double)get_int (result, i), 0.0);
	  resultVector.push_back (val);
	  if (realMax < val.real ()) realMax = val.real ();
	  if (realMin > val.real ()) realMin = val.real ();
	}
	break;
      case CCT_FLOAT:
	{
	  std::complex<double> val (get_real (result, i), 0.0);
	  resultVector.push_back (val);
	  if (realMax < val.real ()) realMax = val.real ();
	  if (realMin > val.real ()) realMin = val.real ();
	}
	break;
      case CCT_COMPLEX:
	{
	  std::complex<double> val (get_real (result, i),
				    get_imag (result, i));
	  resultVector.push_back (val);
	  if (val.imag () != 0.0) isComplex   = true;
	  if (realMax < val.real ()) realMax = val.real ();
	  if (realMin > val.real ()) realMin = val.real ();
	  if (imagMax < val.imag ()) imagMax = val.imag ();
	  if (imagMin > val.imag ()) imagMin = val.imag ();
	}
	break;
      }
    }
    if (resultValid) {
      if (isComplex) {
	for (int i = 0; i < (int)resultVector.size (); i++)
	  fprintf (stderr, "%gj%g ",
		   resultVector[i].real (),
		   resultVector[i].imag ());
	fprintf (stderr, "\n");
      }
      else {
	for (int i = 0; i < (int)resultVector.size (); i++)
	  fprintf (stderr, "%g ", resultVector[i].real ());
	fprintf (stderr, "\n");
      }

      QXYSeries *series = new QXYSeries ();
      QValueAxis *axisX =  QValueAxis ();

      axisX->setRange(10, 20.5);
      axisX->setTickCount(10);
      axisX->setLabelFormat("%.2f");
      chartView->chart()->setAxisX(axisX, series);
      
    }
    else
      mw->printError (tr ("Invalid vactor."));
    cmd = QString (")erase %1 %2").arg (IDXVAR, PLOTVAR);
  }
  else
    mw->printError (tr ("An APL expression must be specified."));
}

void
Plot2DWindow::setResolution ()
{
  QDialog dialog (this, Qt::Dialog);
  QGridLayout *layout = new QGridLayout;
  dialog.setLayout (layout);

  int row = 0;

  QLabel *resolutionLbl = new QLabel (tr ("Resolution:"));
  layout->addWidget (resolutionLbl, row, 0);

  QSpinBox *resolutionBox = new QSpinBox ();
  resolutionBox->setRange (16, 128);
  resolutionBox->setValue (16);
  layout->addWidget (resolutionBox, row, 1);
  connect (resolutionBox,
           &QSpinBox::valueChanged,
          [=](){
	    resolution = resolutionBox->value ();
	    drawCurve ();
          });

  row++;

  QPushButton *closeButton = new QPushButton (QObject::tr ("Close"));
  closeButton->setAutoDefault (true);
  closeButton->setDefault (true);
  layout->addWidget (closeButton, row, 1);
  QObject::connect (closeButton, &QPushButton::clicked,
                    &dialog, &QDialog::accept);

  dialog.exec ();
}


void Plot2DWindow::createMenubar ()
{
  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

  // save/print graph

  QMenu *settingsMenu = menuBar()->addMenu(tr("&Settings"));

  QAction *resolutionAct =
    settingsMenu->addAction(tr("&Resolution"), this,
			    & Plot2DWindow::setResolution);
  resolutionAct->setStatusTip(tr("Set resolution"));

}

void
Plot2DWindow::closeEvent(QCloseEvent *event __attribute__((unused)))
{
  //  mw->closeHW ();
  delete this;
}

Plot2DWindow::Plot2DWindow (MainWindow *parent)
  : QMainWindow(parent)
{
  mw = parent;

  resolution	= 16.0;
  realInit	= 0.0;
  realFinal	= 1.0;
  imagInit	= 0.0;
  imagFinal	= 0.0;

  setupComplete = false;
    
  QWidget *hw = new QWidget ();
  this->setCentralWidget(hw);
  QGridLayout *layout = new QGridLayout;
  
  createMenubar ();

  int row = 0;

  chartView = new QChartView (this);
  chart = new QChart ();
  chart->setTitle ("hhhhhhhhhhhhhh");
  chartView->setChart (chart);
  layout->addWidget (chartView, row, 0, 1, 3);

  row++;
  
  aplExpression = new QLineEdit ();
  aplExpression->setPlaceholderText ("APL");
  connect (aplExpression,
           &QLineEdit::returnPressed,
          [=](){
	    if (setupComplete) drawCurve ();
          });
  layout->addWidget (aplExpression, row, 0, 1, 3);

  row++;

  int col = 0;
  
  indexVariable = new QLineEdit ();
  indexVariable->setPlaceholderText ("Index");
#if 0
  connect (indexVariable,
           &QLineEdit::returnPressed,
          [=](){
	    if (setupComplete) drawCurve ();
          });
#endif
  fprintf (stderr, "iv %d\n", col);
  layout->addWidget (indexVariable, row, col++, 1, 2);
  
  ComplexSpinBox *rangeInit = new ComplexSpinBox ();
  connect (rangeInit,
           &ComplexSpinBox::valueChanged,
          [=](){
	    realInit = rangeInit->getReal ();
	    imagInit = rangeInit->getImag ();
	    if (setupComplete) drawCurve ();
          });
  rangeInit->setComplex (0.0, 0.0);
  fprintf (stderr, "ri %d\n", col);
  layout->addWidget (rangeInit, row, col++);
  
  ComplexSpinBox *rangeFinal = new ComplexSpinBox ();
  connect (rangeFinal,
           &ComplexSpinBox::valueChanged,
          [=](){
	    realFinal = rangeFinal->getReal ();
	    imagFinal = rangeFinal->getImag ();
	    if (setupComplete) drawCurve ();
          });
  rangeFinal->setComplex (0.0, 0.0);
  fprintf (stderr, "rf %d\n", col);
  layout->addWidget (rangeFinal, row, col++);
  
  setupComplete = true;

  hw->setLayout(layout);
  this->show ();
}

Plot2DWindow::~Plot2DWindow ()
{
}

#include <QMenuBar>
#include <complex>

#include <apl/libapl.h>

#include "mainwindow.h"
#include "complexspinbox.h"
#include "plot2dwindow.h"

#define PLOTVAR "plotvarλ"

void Plot2DWindow::drawCurve ()
{
  fprintf (stderr, "drawing curve\n");

  double realIncr   = (realFinal - realInit) / (double)resolution;
  double imagIncr   = (imagFinal - imagInit) / (double)resolution;
  if (realIncr != 0.0 || imagIncr != 0.0) {
    QString varName = indexVarName->text ();
    if (!varName.isEmpty ()) {
      QString aplExpr = aplExpression->text ();
      if (!aplExpr.isEmpty ()) {
	QString cmd = QString ("%1←%2j%3+((⍳%4)-⎕io)×%5j%6")
	  .arg (varName).arg (realInit).arg (imagInit).arg (resolution+1)
	  .arg (realIncr).arg (imagIncr);
	mw->processLine (false, cmd);
	cmd = QString ("%1←%2").arg (PLOTVAR, aplExpr);
	mw->processLine (false, cmd);
	QString pv (PLOTVAR);
	APL_value result = get_var_value (pv.toUtf8 (), "drawCurve");
	int resultRank		= get_rank (result);
	int resultElementCount	= get_element_count (result);
	fprintf (stderr, "rank = %d, count = %d\n",
		 resultRank, resultElementCount);
	std::vector<std::complex<double>> resultVector;
	bool resultValid = true;
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
	    }
	    break;
	  case CCT_FLOAT:
	    {
	      std::complex<double> val (get_real (result, i), 0.0);
	      resultVector.push_back (val);
	    }
	    break;
	  case CCT_COMPLEX:
	    {
	      std::complex<double> val (get_real (result, i),
					get_imag (result, i));
	      resultVector.push_back (val);
	    }
	    break;
	  }
	}
	if (resultValid) {
	}
	else
	  mw->printError (tr ("Invalid vactor."));

	
      }
      else
	mw->printError (tr ("An APL expression must be specified."));
    }
    else
      mw->printError (tr ("Index variable name must be specified."));
  }
  else
    mw->printError (tr ("Index increment cannot be 0J0."));
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

  aplExpression = new QLineEdit ();
  aplExpression->setPlaceholderText ("APL");
  connect (aplExpression,
           &QLineEdit::returnPressed,
          [=](){
	    if (setupComplete) drawCurve ();
          });
  layout->addWidget (aplExpression, row, 0);

  row++;

  indexVarName = new QLineEdit ();
  indexVarName->setPlaceholderText ("Index");
  layout->addWidget (indexVarName, row, 0);
  
  ComplexSpinBox *rangeInit = new ComplexSpinBox ();
  connect (rangeInit,
           &ComplexSpinBox::valueChanged,
          [=](){
	    realInit = rangeInit->getReal ();
	    imagInit = rangeInit->getImag ();
	    if (setupComplete) drawCurve ();
          });
  rangeInit->setComplex (0.0, 0.0);
  layout->addWidget (rangeInit, row, 1);
  
  ComplexSpinBox *rangeFinal = new ComplexSpinBox ();
  connect (rangeFinal,
           &ComplexSpinBox::valueChanged,
          [=](){
	    realFinal = rangeFinal->getReal ();
	    imagFinal = rangeFinal->getImag ();
	    if (setupComplete) drawCurve ();
          });
  rangeFinal->setComplex (0.0, 0.0);
  layout->addWidget (rangeFinal, row, 2);
  
  setupComplete = true;

  hw->setLayout(layout);
  this->show ();
}

Plot2DWindow::~Plot2DWindow ()
{
}

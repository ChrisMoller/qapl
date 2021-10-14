#include "mainwindow.h"
#include "complexspinbox.h"
#include "plot2dwindow.h"

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
  QWidget *hw = new QWidget ();
  QGridLayout *layout = new QGridLayout;
  this->setCentralWidget(hw);

  int row = 0;

  QLineEdit *indexVarName = new QLineEdit ();
  indexVarName->setPlaceholderText ("Index");
  layout->addWidget (indexVarName, row, 0);
  
  ComplexSpinBox *rangeInit = new ComplexSpinBox ();
  connect (rangeInit,
           &ComplexSpinBox::valueChanged,
          [=](){
            double real = rangeInit->getReal ();
            double imag = rangeInit->getImag ();
	    fprintf (stderr, "init %g %g\n", real, imag);
          });
  rangeInit->setComplex (0.0, 0.0);
  layout->addWidget (rangeInit, row, 1);
  
  ComplexSpinBox *rangeFinal = new ComplexSpinBox ();
  connect (rangeFinal,
           &ComplexSpinBox::valueChanged,
          [=](){
            double real = rangeFinal->getReal ();
            double imag = rangeFinal->getImag ();
	    fprintf (stderr, "final %g %g\n", real, imag);
          });
  rangeFinal->setComplex (0.0, 0.0);
  layout->addWidget (rangeFinal, row, 2);
  
  hw->setLayout(layout);
  this->show ();
}

Plot2DWindow::~Plot2DWindow ()
{
}

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
  
  ComplexSpinBox *rangeInit = new ComplexSpinBox ();
  rangeInit->setComplex (-6000.0, -8000.0);
  layout->addWidget (rangeInit, row, 0);
  
  hw->setLayout(layout);
  this->show ();
}

Plot2DWindow::~Plot2DWindow ()
{
}

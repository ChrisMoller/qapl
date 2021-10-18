#include <QMenuBar>
#include <QChart>
#include <QChartView>
#include <QSplineSeries>
#include <QValueAxis>
//#include <QLineSeries>

#include <complex>

#include <values.h>
#include <apl/libapl.h>

#include "mainwindow.h"
#include "complexspinbox.h"
#include "plot2dwindow.h"

#define PLOTVAR "plotvarλ"
#define IDXVAR  "idxvarλ"

bool Plot2DWindow::appendSeries (double x, double y,
				 double &realMax, double &realMin)
{
  bool rc = true;
  if (realMax < y) realMax = y;
  if (realMin > y) realMin = y;
  switch (modeGroup->checkedId ()) {
  case MODE_BUTTON_SPLINE:
    if (series == nullptr) {
      series = new QSplineSeries ();
      seriesMode = MODE_BUTTON_SPLINE;
    }
    else {
      if (seriesMode != MODE_BUTTON_SPLINE) rc = false;
    }
    static_cast<QSplineSeries*>(series)->append(x, y);
    break;
  case MODE_BUTTON_LINE:
  case MODE_BUTTON_POLAR:
  case MODE_BUTTON_PIE:
  case MODE_BUTTON_SCATTER:
  case MODE_BUTTON_AREA:
  case MODE_BUTTON_BOX:
    break;
  }
  return rc;
}

void Plot2DWindow::drawCurve (QString aplExpr, aspect_e aspect)
{
  if (!aplExpr.isEmpty ()) {
    double realIncr   = (realFinal - realInit) / (double)resolution;
    double imagIncr   = (imagFinal - imagInit) / (double)resolution;
    QString idxvar = indexVariable->text ();
    bool autoIdx = false;
    if (idxvar.isEmpty ()) {
      idxvar = QString (IDXVAR);
      autoIdx = true;
    }
    QString cmd = QString ("%1←%2j%3+((⍳%4)-⎕io)×%5j%6")
      .arg (idxvar).arg (realInit).arg (imagInit).arg (resolution+1)
      .arg (realIncr).arg (imagIncr);
    mw->processLine (false, cmd);
    APL_value idxVals = get_var_value (idxvar.toUtf8 (), "drawCurve.idxVals");
    int idxElementCount	= get_element_count (idxVals);
    bool idxValid = true;
    bool idxComplex   = false;
    std::vector<double> idxVector;
    for (int i = 0; i < idxElementCount; i++) {
      int type = get_type (idxVals, i);
      switch(type) {
      case CCT_CHAR:
      case CCT_POINTER:
	idxValid = false;
	break;
      case CCT_INT:
	idxVector.push_back ((double)get_int (idxVals, i));
	break;
      case CCT_FLOAT:
	idxVector.push_back (get_real (idxVals, i));
	break;
      case CCT_COMPLEX:
	if (get_imag (idxVals, i) != 0.0) idxComplex = true;
	idxVector.push_back (get_real (idxVals, i));
      }
    }
    if (idxComplex)		// fixme
      mw->printError (tr ("Index contains imaginary components.  Using only the real components in the axis."));

    aplExpr.replace (QString ("%1"), QString (IDXVAR));
    cmd = QString ("%1←%2").arg (PLOTVAR, aplExpr);
    mw->processLine (false, cmd);
    mw->processLine (false, cmd);
    QString pv (PLOTVAR);
    APL_value result = get_var_value (pv.toUtf8 (), "drawCurve.result");
    if (result != nullptr) {
      //    int resultRank		= get_rank (result);
      int resultElementCount	= get_element_count (result);
      bool resultValid = true;
      //    bool isComplex   = false;
      double realMax = -MAXDOUBLE;
      double realMin =  MAXDOUBLE;
      series = nullptr;
      seriesMode = MODE_BUTTON_UNSET;
      bool run = true;
      for (int i = 0; run && i < resultElementCount; i++) {
	int type = get_type (result, i);
	switch(type) {
	case CCT_CHAR:
	case CCT_POINTER:
	  resultValid = false;
	  break;
	case CCT_INT:
	  if (!appendSeries (idxVector[i],
			     (double)get_int (result, i), realMax, realMin))
	    run = false;
	  break;
	case CCT_FLOAT:
	  if (!appendSeries (idxVector[i],
			     get_real (result, i), realMax, realMin))
	    run = false;
	  break;
	case CCT_COMPLEX:
	  {
	    switch (aspect) {
	    case ASPECT_REAL:
	      if (!appendSeries (idxVector[i],
				 get_real (result, i), realMax, realMin))
		run = false;
	      break;
	    case ASPECT_IMAG:
	      if (!appendSeries (idxVector[i],
				 get_imag (result, i), realMax, realMin))
		run = false;
	      break;
	    case ASPECT_MAGNITUDE:
	      {
		std::complex<double> val (get_real (result, i),
					  get_imag (result, i));
		if (!appendSeries (idxVector[i],
				   std::abs (val), realMax, realMin))
		  run = false;
	      }
	      break;
	    case ASPECT_PHASE:
	      std::complex<double> val (get_real (result, i),
					get_imag (result, i));
	      if (!appendSeries (idxVector[i],
				 std::arg (val), realMax, realMin))
		run = false;
	      break;
	    }
	  }
	  break;
	}
      }
      if (!run)
	mw->printError (tr ("Inconsistent series type."));
      if (run && resultValid) {
	if (idxElementCount == static_cast<QSplineSeries*>(series)->count ()) {
	  chartView->chart()->addSeries(series);
	  chart->createDefaultAxes ();

#if 0
	  QValueAxis *axisX =  QValueAxis ();

	  axisX->setRange(10, 20.5);
	  axisX->setTickCount(10);
	  axisX->setLabelFormat("%.2f");
	  chartView->chart()->setAxisX(axisX, series);
#endif
      
	}
	else
  mw->printError (tr ("Index and result vectors are of different lengths."));
      }
      else
	mw->printError (tr ("Invalid vector."));
    }
    else
      mw->printError (tr ("Expression evaluation error."));
    cmd = autoIdx 
      ? QString (")erase %1 %2").arg (IDXVAR, PLOTVAR)
      : QString (")erase %1").arg (PLOTVAR);
    mw->processLine (false, cmd);
  }
}


void Plot2DWindow::drawCurves ()
{
  QChart *oldChart = chartView->chart ();
  chart = new QChart ();
  chartView->setChart (chart);
  if (oldChart != nullptr)
    delete oldChart;
  
  chart->setTitle ("hhhhhhhhhhhhhh");
  
  QString aplExpr = aplExpression->text ();
  aspect_e aspect = (aspect_e) aspectGroup->checkedId ();
  drawCurve (aplExpr, aspect);
  for (int i = 0; i < plotCurves.size (); i++) {
    drawCurve (plotCurves[i]->expression (), ASPECT_REAL);
    //    drawCurve (plotCurves[i]->expression (), plotCurves[i]->aspect);
  }
}

void Plot2DWindow::pushExpression ()
{
  QString aplExpr = aplExpression->text ();
  aspect_e aspect = (aspect_e) aspectGroup->checkedId ();
  PlotCurve *plotCurve = new PlotCurve (aplExpr, aspect);
  plotCurves.append (plotCurve);
  aplExpression->clear ();
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
	    drawCurves ();
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
  chart 	= nullptr;

  setupComplete = false;
    
  QWidget *hw = new QWidget ();
  this->setCentralWidget(hw);
  QGridLayout *layout = new QGridLayout;
  
  createMenubar ();

  int row = 0;
  int col = 0;

  chartView = new QChartView (this);
  chartView->setMinimumWidth (360);
  chartView->setMinimumHeight (360);
  layout->addWidget (chartView, row, 0, 1, 3);

  row++;
  
  aplExpression = new QLineEdit ();
  aplExpression->setPlaceholderText ("APL expression");
  connect (aplExpression,
           &QLineEdit::returnPressed,
          [=](){
	    if (setupComplete) drawCurves ();
          });
  layout->addWidget (aplExpression, row, col, 1, 2);

  col += 2;
  
  QPushButton *pushExpr = new QPushButton (QObject::tr ("Push"));
  QObject::connect (pushExpr, &QPushButton::clicked,
		    [=](){
		      pushExpression ();
		      drawCurves ();
		    });
  layout->addWidget (pushExpr, row, col++);

  row++;
  col = 0;
  
  indexVariable = new QLineEdit ();
  indexVariable->setPlaceholderText ("Index var");
  indexVariable->setMaximumWidth (100);
#if 0
  connect (indexVariable,
           &QLineEdit::returnPressed,
          [=](){
	    if (setupComplete) drawCurves ();
          });
#endif
  layout->addWidget (indexVariable, row, col++, 1, 2);
  
  ComplexSpinBox *rangeInit = new ComplexSpinBox ();
  connect (rangeInit,
           &ComplexSpinBox::valueChanged,
          [=](){
	    realInit = rangeInit->getReal ();
	    imagInit = rangeInit->getImag ();
	    if (setupComplete) drawCurves ();
          });
  rangeInit->setComplex (0.0, 0.0);
  layout->addWidget (rangeInit, row, col++);
  
  ComplexSpinBox *rangeFinal = new ComplexSpinBox ();
  connect (rangeFinal,
           &ComplexSpinBox::valueChanged,
          [=](){
	    realFinal = rangeFinal->getReal ();
	    imagFinal = rangeFinal->getImag ();
	    if (setupComplete) drawCurves ();
          });
  rangeFinal->setComplex (0.0, 0.0);
  layout->addWidget (rangeFinal, row, col++);

  row++;
  col = 0;

  QGroupBox *modeBox = new QGroupBox (tr ("Presentation"));
  modeGroup = new QButtonGroup ();
  connect(modeGroup,
	  QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
	  [=](
	      QAbstractButton *button __attribute__((unused))
	      ){
	    drawCurves ();
	  });
  modeGroup->setExclusive(true);
  QGridLayout *modeLayout = new QGridLayout;
  modeBox->setLayout (modeLayout);

  int modeRow = 0;
  int modeCol = 0;

  QRadioButton *splineButton = new QRadioButton(tr ("Spines"), modeBox);
  splineButton->setChecked (true);
  modeGroup->addButton (splineButton,  MODE_BUTTON_SPLINE);
  modeLayout->addWidget (splineButton, modeRow, modeCol++);

  QRadioButton *lineButton = new QRadioButton(tr ("Lines"), modeBox);
  modeGroup->addButton (lineButton,  MODE_BUTTON_LINE);
  modeLayout->addWidget (lineButton, modeRow, modeCol++);

  QRadioButton *polarButton = new QRadioButton(tr ("Polar"), modeBox);
  modeGroup->addButton (polarButton,  MODE_BUTTON_POLAR);
  modeLayout->addWidget (polarButton, modeRow, modeCol++);

  QRadioButton *pieButton = new QRadioButton(tr ("Pie"), modeBox);
  modeGroup->addButton (pieButton,  MODE_BUTTON_PIE);
  modeLayout->addWidget (pieButton, modeRow, modeCol++);

  modeRow++;
  modeCol = 0;  

  QRadioButton *scatterButton = new QRadioButton(tr ("Scatter"), modeBox);
  modeGroup->addButton (scatterButton,  MODE_BUTTON_SCATTER);
  modeLayout->addWidget (scatterButton, modeRow, modeCol++);

  QRadioButton *areaButton = new QRadioButton(tr ("Area"), modeBox);
  modeGroup->addButton (areaButton,  MODE_BUTTON_AREA);
  modeLayout->addWidget (areaButton, modeRow, modeCol++);

  QRadioButton *boxButton = new QRadioButton(tr ("Box"), modeBox);
  modeGroup->addButton (boxButton,  MODE_BUTTON_BOX);
  modeLayout->addWidget (boxButton, modeRow, modeCol++);

  layout->addWidget (modeBox, row, col++, 1, 3);

  row++;
  col = 0;

  QGroupBox *aspectBox = new QGroupBox (tr ("Aspect"));
  aspectGroup = new QButtonGroup ();
  connect(aspectGroup,
	  QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
	  [=](
	      QAbstractButton *button __attribute__((unused))
	      ){
	    drawCurves ();
	  });
  aspectGroup->setExclusive(true);
  QHBoxLayout *aspectLayout = new QHBoxLayout;
  aspectBox->setLayout (aspectLayout);

  QRadioButton *realButton = new QRadioButton(tr ("Real"), aspectBox);
  realButton->setChecked (true);
  aspectGroup->addButton (realButton,  ASPECT_REAL);
  aspectLayout->addWidget (realButton);

  QRadioButton *imagButton = new QRadioButton(tr ("Imaginary"), aspectBox);
  aspectGroup->addButton (imagButton,  ASPECT_IMAG);
  aspectLayout->addWidget (imagButton);

  QRadioButton *magButton = new QRadioButton(tr ("Magnitude"), aspectBox);
  aspectGroup->addButton (magButton,  ASPECT_MAGNITUDE);
  aspectLayout->addWidget (magButton);

  QRadioButton *phaseButton = new QRadioButton(tr ("Phase"), aspectBox);
  aspectGroup->addButton (phaseButton,  ASPECT_PHASE);
  aspectLayout->addWidget (phaseButton);

  layout->addWidget (aspectBox, row, col++, 1, 3);
    
  setupComplete = true;

  hw->setLayout(layout);
  this->show ();
}

Plot2DWindow::~Plot2DWindow ()
{
}

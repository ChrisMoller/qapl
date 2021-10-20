#include "mainwindow.h"
#include "plot2dwindow.h"
#include "chart2dwindow.h"
#include <values.h>
#include <apl/libapl.h>

#define PLOTVAR "plotvarλ"
#define IDXVAR  "idxvarλ"

bool Chart2DWindow::appendSeries (double x, double y,
				 double &realMax, double &realMin)
{
  bool rc = true;
  if (realMax < y) realMax = y;
  if (realMin > y) realMin = y;
  switch (pw->getMode ()) {
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
  case MODE_BUTTON_UNSET:
    break;
  }
  return rc;
}

void Chart2DWindow::drawCurve (QString aplExpr, aspect_e aspect,
			       QString label, QPen pen)
{
  if (!aplExpr.isEmpty ()) {
    double realIncr   = (pw->getRealFinal () - pw->getRealInit ()) /
      (double)(pw->getResolution ());
    double imagIncr   = (pw->getImagFinal () - pw->getImagInit ()) /
      (double)(pw->getResolution ());
    QString realIncrString =
      QString::number (realIncr).replace (QString ("-"), QString ("¯"));
    QString imagIncrString =
      QString::number (imagIncr).replace (QString ("-"), QString ("¯"));
    QString realInitString =
      QString::number (pw->getRealInit ()).replace (QString ("-"),
						    QString ("¯"));
    QString imagInitString =
      QString::number (pw->getImagInit ()).replace (QString ("-"),
						    QString ("¯"));
    QString idxvar = pw->getIndexVariable ();
    bool autoIdx = false;
    if (idxvar.isEmpty ()) {
      idxvar = QString (IDXVAR);
      autoIdx = true;
    }
    QString cmd = QString ("%1←%2j%3+((⍳%4)-⎕io)×%5j%6")
      .arg (idxvar).arg (realInitString).arg (imagInitString)
      .arg (1+pw->getResolution ()).arg (realIncrString).arg (imagIncrString);
    mw->processLine (false, cmd);
    APL_value idxVals = get_var_value (idxvar.toUtf8 (), "drawCurve.idxVals");
    if (idxVals != nullptr) {
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
	  if (idxElementCount ==
	      static_cast<QSplineSeries*>(series)->count ()) {
	    static_cast<QSplineSeries*>(series)->setPen (pen);
	    series->setName (label);
	    chartView->chart()->addSeries(series);
	    chart->createDefaultAxes ();
	    QList<QAbstractAxis *>haxes = chart->axes (Qt::Horizontal);
	    if (haxes.size () > 0)
	      haxes.first ()->setTitleText (pw->getXTitle ());
	    QList<QAbstractAxis *>vaxes = chart->axes (Qt::Vertical);
	    if (vaxes.size () > 0)
	      vaxes.first ()->setTitleText (pw->getYTitle ());
	    chart->setTitle (pw->getChartTitle ());
	    chart->setTheme (pw->getTheme ());

	    // fixme -- assumes left/down are min
	    double dx = 0.075 * (idxVector.back () - idxVector.front ());
	    double dy = 0.075 * (realMax - realMin);
	    chart->axes (Qt::Vertical).first()
	      ->setRange(realMin - dy, realMax + dy);
	    chart->axes (Qt::Horizontal).first()
	      ->setRange(idxVector.front () - dx, idxVector.back () + dx);

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
    }
    else
      mw->printError (tr ("Index evaluation error."));
    cmd = autoIdx 
      ? QString (")erase %1 %2").arg (IDXVAR, PLOTVAR)
      : QString (")erase %1").arg (PLOTVAR);
    mw->processLine (false, cmd);
  }
}

void Chart2DWindow::drawCurves ()
{
  QChart *oldChart = chartView->chart ();
  chart = new QChart ();
  chartView->setChart (chart);
  if (oldChart != nullptr)
    delete oldChart;
  
  QString aplExpr = pw->getAplExpression ();
  aspect_e aspect = pw->getAspect ();
  QPen pen = pw->getPen ();
  QString label = pw->getCurveTitle ();
  drawCurve (aplExpr, aspect, label, pen);
  for (int i = 0; i < pw->getPlotCurves ().size (); i++) {
    drawCurve (pw->getPlotCurves ()[i]->expression (),
	       pw->getPlotCurves ()[i]->aspect (),
	       pw->getPlotCurves ()[i]->label (),
	       pw->getPlotCurves ()[i]->pen ());
  }
}

void
Chart2DWindow::exportImage ()
{
  // https://forum.qt.io/topic/76684/qpaintdevice-cannot-destroy-paint-device-that-is-being-painted/3
#if 0
  const auto dpr = chartView->devicePixelRatioF();
  QPixmap buffer(chartView->width() * dpr, chartView->height() * dpr);
  buffer.setDevicePixelRatio(dpr);
  buffer.fill(Qt::transparent);
#else
  int cvw = chartView->width ();
  int cvh = chartView->height ();
  chartView->setMinimumSize (1000, 1000);
  QPixmap buffer(chartView->width(), chartView->height());
  buffer.fill(Qt::white);
#endif

  QPainter *paint = new QPainter(&buffer);
  // see void QPainter::setBackgroundMode(Qt::BGMode mode)
  //  paint->setPen(*(new QColor(255,34,255,255)));
  QColor colour(255,34,255,255);
  chartView->render(paint);
  drawCurves ();
  paint->end ();
  QFile file("image.png");
  file.open(QIODevice::WriteOnly);
  // bool QPixmap::save(const QString &fileName, const char *format = nullptr, int quality = -1) const
  //https://doc.qt.io/qt-5/qtimageformats-index.html
  buffer.save(&file, "PNG");
  file.close ();
  chartView->setMinimumSize (cvw, cvh);
}

void Chart2DWindow::createMenubar ()
{
  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
  
  QAction *exportAct =
    fileMenu->addAction(tr("&Export..."), this,
			    &Chart2DWindow::exportImage);
  exportAct->setStatusTip(tr("Export chart"));
}

Chart2DWindow::Chart2DWindow (Plot2DWindow *parent, MainWindow *mainWin)
  : QMainWindow(parent)
{
  this->setWindowTitle ("qapl 2D Plot");
  pw = parent;
  mw = mainWin;

  QWidget *hw = new QWidget ();
  this->setCentralWidget(hw);
  QGridLayout *layout = new QGridLayout;

  createMenubar ();

  chartView = new QChartView (this);
  chartView->setMinimumWidth (360);
  chartView->setMinimumHeight (360);

  layout->addWidget (chartView, 0, 0, 1, 3);




  hw->setLayout(layout);
  this->show ();
}

Chart2DWindow::~Chart2DWindow ()
{
}

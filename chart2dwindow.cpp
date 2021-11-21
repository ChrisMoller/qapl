// https://doc.qt.io/qt-5/qtcharts-customchart-example.html
//
//  https://stackoverflow.com/questions/42941508/display-text-on-qtcharts
//

#include <QScatterSeries>
#include <QCategoryAxis>
#include <QPolarChart>

#include "textitem.h"

#include "mainwindow.h"
#include "plot2dwindow.h"
#include "chart2dwindow.h"
#include <values.h>
#include <apl/libapl.h>

#define PLOTVARX "plotvarxλ"
#define PLOTVARY "plotvaryλ"
#define IDXVAR  "idxvarλ"
#define PARMVAR  "parmvarλ"

bool Chart2DWindow::appendSeries (double x, double y, series_mode_e mode,
				  double &realXMax, double &realXMin,
				  double &realYMax, double &realYMin)
{
  bool rc = true;
  if (realXMax < x) realXMax = x;
  if (realXMin > x) realXMin = x;
  if (realYMax < y) realYMax = y;
  if (realYMin > y) realYMin = y;
  switch (mode) {
  case MODE_BUTTON_SPLINE:
    static_cast<QSplineSeries*>(series)->append(x, y);
    break;
  case MODE_BUTTON_LINE:
    static_cast<QLineSeries*>(series)->append(x, y);
    break;
  case MODE_BUTTON_SCATTER:
    static_cast<QScatterSeries*>(series)->append(x, y);
    break;
  case MODE_BUTTON_POLAR:
  case MODE_BUTTON_PIE:
  case MODE_BUTTON_AREA:
  case MODE_BUTTON_BOX:
  case MODE_BUTTON_UNSET:
    break;
  }
  return rc;
}

void Chart2DWindow::drawCurve (QString aplXExpr, QString aplYExpr,
			       aspect_e aspect, QString label, QPen pen,
			       series_mode_e mode,
			       double &realXMax, double &realXMin,
			       double &realYMax, double &realYMin,
			       std::vector<double> idxVector,
			       double markerSize)
{
  if (!aplYExpr.isEmpty ()) {
    switch (mode) {
    case MODE_BUTTON_SPLINE:
      series = new QSplineSeries ();
      break;
    case MODE_BUTTON_LINE:
      series = new QLineSeries ();
      break;
    case MODE_BUTTON_SCATTER:
      series = new QScatterSeries ();
      static_cast<QScatterSeries*>(series)->setMarkerSize (markerSize);
      break;
    default:
      series = nullptr;
      break;
    }
    
    if (avgScale != 1.0) {
      //      fprintf (stderr, "setting pw %g\n",
      //	       10.0 * fontScale * (double)pen.width ());
      pen.setWidth ((int)(10.0 * avgScale * (double)pen.width ()));
    }
  
    QString cmd = QString ("%1←%2").arg (PLOTVARY, aplYExpr);
    mw->processLine (false, cmd);
    QString pv (PLOTVARY);
    APL_value resulty = get_var_value (pv.toUtf8 (), "drawCurve.result");

    APL_value resultx = nullptr;
    if (!aplXExpr.isEmpty ()) {
      QString cmd = QString ("%1←%2").arg (PLOTVARX, aplXExpr);
      mw->processLine (false, cmd);
      QString pv (PLOTVARX);
      resultx = get_var_value (pv.toUtf8 (), "drawCurve.result");
    }

    if (resulty != nullptr) {
      int resultElementCount = get_element_count (resulty);
      bool resultValid = true;
      bool run = true;
      if (resultx != nullptr) {
	int rec = get_element_count (resultx);
	if (rec != resultElementCount) {
	  mw->printError (tr ("X and Y vectors are of different length."));
	  run = false;
	}
      }

      for (int i = 0; run && i < resultElementCount; i++) {
	double xVal = 0.0;
	if (resultx != nullptr) {
	  int type = get_type (resultx, i);
	  switch(type) {
	  case CCT_CHAR:
	  case CCT_POINTER:
	    resultValid = false;
	    break;
	  case CCT_INT:
	    xVal = (double)get_int (resultx, i);
	    break;
	  case CCT_FLOAT:
	    xVal = get_real (resultx, i);
	    break;
	  case CCT_COMPLEX:
	    switch (aspect) {
	    case ASPECT_REAL:
	      xVal = get_real (resultx, i);
	      break;
	    case ASPECT_IMAG:
	      xVal = get_imag (resultx, i);
	      break;
	    case ASPECT_MAGNITUDE:
	      {
		std::complex<double> val (get_real (resultx, i),
					  get_imag (resultx, i));
		xVal = std::abs (val);
	      }
	      break;
	    case ASPECT_PHASE:
	      {
		std::complex<double> val (get_real (resultx, i),
					  get_imag (resultx, i));
		xVal = std::arg (val);
	      }
	      break;
	    }
	    break;
	  }
	}
	else xVal = idxVector[i];
	
	int type = get_type (resulty, i);
	switch(type) {
	case CCT_CHAR:
	case CCT_POINTER:
	  resultValid = false;
	  break;
	case CCT_INT:
	  if (!appendSeries (xVal,
			     (double)get_int (resulty, i), mode,
			     realXMax, realXMin, realYMax, realYMin))
	    run = false;
	  break;
	case CCT_FLOAT:
	  if (!appendSeries (xVal, get_real (resulty, i), mode,
			     realXMax, realXMin, realYMax, realYMin))
	    run = false;
	  break;
	case CCT_COMPLEX:
	  {
	    switch (aspect) {
	    case ASPECT_REAL:
	      if (!appendSeries (xVal, get_real (resulty, i), mode,
				 realXMax, realXMin, realYMax, realYMin))
		run = false;
	      break;
	    case ASPECT_IMAG:
	      if (!appendSeries (xVal, get_imag (resulty, i), mode,
				 realXMax, realXMin, realYMax, realYMin))
		run = false;
	      break;
	    case ASPECT_MAGNITUDE:
	      {
		std::complex<double> val (get_real (resulty, i),
					  get_imag (resulty, i));
		if (!appendSeries (xVal, std::abs (val), mode,
				   realXMax, realXMin, realYMax, realYMin))
		  run = false;
	      }
	      break;
	    case ASPECT_PHASE:
	      std::complex<double> val (get_real (resulty, i),
					get_imag (resulty, i));
	      if (!appendSeries (xVal, std::arg (val), mode,
				 realXMax, realXMin, realYMax, realYMin))
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
	int seriesCount = -1;
	switch (pw->getMode ()) {
	case MODE_BUTTON_SPLINE:
	  seriesCount = (series == nullptr) ? 0
	    : static_cast<QSplineSeries*>(series)->count ();
	  static_cast<QSplineSeries*>(series)->setPen (pen);
	  break;
	case MODE_BUTTON_LINE:
	  seriesCount = (series == nullptr) ? 0
	    : static_cast<QLineSeries*>(series)->count ();
	  static_cast<QLineSeries*>(series)->setPen (pen);
	  break;
	case MODE_BUTTON_SCATTER:
	  seriesCount = (series == nullptr) ? 0
	    : static_cast<QScatterSeries*>(series)->count ();
	  static_cast<QScatterSeries*>(series)->setPen (pen);
	  break;
	default:
	  break;
	}
	  
	if (seriesCount > 0 && idxVector.size () ==
	    (long unsigned int)seriesCount) {
	  series->setName (label);
	  // fixme -- scale pen width
	  //	    series->setWidth(2 * fontScale);

	  chartView->chart()->addSeries(series);
	}
	else {
	  QString msg = QString ("Index and result vectors \
are of different lengths: %1 %2").arg (seriesCount).arg (idxVector.size ());
	  mw->printError (msg);
	}
      }
      else
	mw->printError (tr ("Expression evaluation error."));
    }
    else
      mw->printError (tr ("Expression evaluation error."));

    cmd = QString (")erase %1").arg (PLOTVARY);
    mw->processLine (false, cmd);
  }
}

void Chart2DWindow::doSubstitutions (QString &aplXExpr, QString &aplYExpr)
{
  aplXExpr.replace (QString ("%1"), QString (IDXVAR));
  aplYExpr.replace (QString ("%1"), QString (IDXVAR));
    
  for (int i = 0; i < pw->getPlotParameters ().size (); i++) {
    double  real  = pw->getPlotParameters ().at (i)->real ();
    double  imag  = pw->getPlotParameters ().at (i)->imag ();
    QString vname = pw->getPlotParameters ().at (i)->vname ();
    if (vname.isEmpty ()) {
      vname = QString ("%1%2").arg (PARMVAR).arg (i);
      QString tgt = QString ("$%1").arg (i);
      aplXExpr.replace (tgt, QString (vname));
      aplYExpr.replace (tgt, QString (vname));
    }
    QString cmd = QString ("%1←%2j%3").arg (vname).arg (real).arg (imag);
    mw->processLine (false, cmd);
  }
}

QFont Chart2DWindow::scaleFont (double fontScale, QFont &font)
{
  QString family = font.family ();
  int pointSize  =
    (int) (2.0 * fontScale * font.pointSizeF ());
  int weight     = font.weight ();
  bool italic    = font.italic ();
  return QFont(family, pointSize, weight, italic);
}

void Chart2DWindow::drawCurves ()
{
  series = nullptr;
  QChart *oldChart = chartView->chart ();
  chart = pw->getPolar () ? new QPolarChart () : new QChart ();
  chartView->setChart (chart);
  if (oldChart != nullptr)
    delete oldChart;

  
  chart->setTheme (pw->getTheme ());
    
  {
    chart->setTitleFont(pw->getChartTitleFont ());
    chart->setTitleBrush(QBrush(pw->getChartTitleColour ()));
    chart->setTitle (pw->getChartTitle ());
  }

  if (!pw->getBGFile ().isEmpty ()) {
    //    QString fn = chartControls->getChartData ()->getBGFile ();
    //    QString fn("./images/bgimage.png");
    QImage gep (pw->getBGFile ());
    gep = gep.scaled (chartView->width (), chartView->height (),
		      // Qt::IgnoreAspectRatio,
		      // Qt::KeepAspectRatio,
		      //Qt::KeepAspectRatioByExpanding,
		      pw->getAspectMode (),
		      Qt::SmoothTransformation);
    QBrush  brush (gep);
    chart->setBackgroundBrush (brush);
  }
  
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
  APL_value idxVals = get_var_value (idxvar.toUtf8 (), "drawCurves.idxVals");
  if (idxVals != nullptr) {
    int idxElementCount = get_element_count (idxVals);
    //      bool idxValid = true;
    bool idxComplex   = false;
    std::vector<double> idxVector;
    for (int i = 0; i < idxElementCount; i++) {
      int type = get_type (idxVals, i);
      switch(type) {
      case CCT_CHAR:
      case CCT_POINTER:
	//	  idxValid = false;
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
      mw->printError (tr ("Index contains imaginary components.  \
Using only the real components in the axis."));

    QString aplXExpr = pw->getAplXExpression ();
    QString aplYExpr = pw->getAplYExpression ();
    doSubstitutions (aplXExpr, aplYExpr);
    
    aspect_e aspect = pw->getAspect ();
    QString label = pw->getCurveTitle ();
    QPen pen = *(pw->getPen ());
    double markerSize = pw->getMarkerSize ();
    series_mode_e mode = pw->getMode ();
    double realXMax = -MAXDOUBLE;
    double realXMin =  MAXDOUBLE;
    double realYMax = -MAXDOUBLE;
    double realYMin =  MAXDOUBLE;
    drawCurve (aplXExpr, aplYExpr, aspect, label, pen, mode,
	       realXMax, realXMin, realYMax, realYMin,
	       idxVector, markerSize);
    for (int i = 0; i < pw->getPlotCurves ().size (); i++) {
      pen = *(pw->getPlotCurves ().at (i)->pen ());
      QString aplXExpr = pw->getPlotCurves ().at (i)->Xexpression ();
      QString aplYExpr = pw->getPlotCurves ().at (i)->Yexpression ();
      doSubstitutions (aplXExpr, aplYExpr);
      
      drawCurve (aplXExpr,
		 aplYExpr,
		 pw->getPlotCurves ().at (i)->aspect (),
		 pw->getPlotCurves ().at (i)->title (),
		 pen,
		 pw->getPlotCurves ().at (i)->mode (),
		 realXMax, realXMin,
		 realYMax, realYMin,
		 idxVector,
		 pw->getPlotCurves ().at (i)->markerSize ()
		 );
    }

    if (series != nullptr) {
      // was in drawCurves()
    
      QValueAxis *axisX = new QValueAxis();
      QValueAxis *axisY = new QValueAxis();
      axisX->setTickCount(10);
      axisY->setTickCount(10);
      //	    axisX->setLabelFormat("%.2f");

      {	
	axisX->setTitleBrush(QBrush(pw->getAxisTitleColour ()));
	axisY->setTitleBrush(QBrush(pw->getAxisTitleColour ()));
	axisX->setLabelsBrush(QBrush(pw->getAxisLabelColour ()));
	axisY->setLabelsBrush(QBrush(pw->getAxisLabelColour ()));
	

	/***
	    double dpi =
	    QGuiApplication::primaryScreen()->physicalDotsPerInch();
	    fprintf (stderr, "dpi %g\n", dpi);
	  
	    pixels = dpi * point / 72
	    
	***/
      
#if 1
	if (fontScale != 1.0) {
	  QFont ufont =  pw->getAxisTitleFont ();
	  QFont tfont = scaleFont (fontScale, ufont);
	  axisX->setTitleFont (tfont);
	  axisY->setTitleFont (tfont);
	}
	else {
	  axisX->setTitleFont ((pw->getAxisTitleFont ()));
	  axisY->setTitleFont ((pw->getAxisTitleFont ()));
	}
#else
	// fprintf (stderr, "title before %g\n", tfont.pointSizeF ());
	double psf = fontScale * tfont.pointSizeF ();
	tfont.setPointSizeF (psf);
	// fprintf (stderr, "title after %g\n", tfont.pointSizeF ());
	axisX->setTitleFont(tfont);
	axisY->setTitleFont(tfont);
#endif
	axisX->setTitleText (pw->getXTitle ());
	axisY->setTitleText (pw->getYTitle ());
	      
#if 1
	if (fontScale != 1.0) {
	  QString family = pw->getAxisLabelFont ().family ();
	  int pointSize  =
	    (int) (2.0 * fontScale *
		   pw->getAxisTitleFont ().pointSizeF ());
	  int weight     = pw->getAxisTitleFont ().weight ();
	  bool italic    = pw->getAxisTitleFont ().italic ();
	  QFont tfont = QFont(family, pointSize, weight, italic);
	  axisX->setLabelsFont (tfont);
	  axisY->setLabelsFont (tfont);
	}
	else {
	  axisX->setLabelsFont ((pw->getAxisLabelFont ()));
	  axisY->setLabelsFont ((pw->getAxisLabelFont ()));
	}
#else
	QFont ufont (pw->getAxisLabelFont ());
	// fprintf (stderr, "label before %g\n", ufont.pointSizeF ());
	psf = fontScale * ufont.pointSizeF ();
	ufont.setPointSizeF (psf);
	// fprintf (stderr, "label after %g\n", ufont.pointSizeF ());
	axisX->setLabelsFont(ufont);
	axisY->setLabelsFont(ufont);
#endif	
      }

      {
	QPen axisPen(QColor(pw->getAxisColour ()));
	axisPen.setWidth(2 * avgScale);
	axisX->setLinePen(axisPen);
	axisY->setLinePen(axisPen);
      
	axisX->setGridLineVisible(true);
	axisY->setGridLineVisible(true);
	// axisY->setShadesPen(Qt::NoPen);
	// axisY->setShadesBrush(QBrush(QColor(0x99, 0xcc, 0xcc, 0x55)));
	// axisY->setShadesVisible(true);
      
	//	double dx = 0.075 * (idxVector.back () - idxVector.front ());
	if (!pw->getyMinLocked ()) {
	  pw->setYmin (realYMin);
	  if (pw->getYminBox ()) pw->getYminBox ()->setValue (realYMin);
	}
	else realYMin = pw->getYmin ();
	if (!pw->getyMaxLocked ()) {
	  pw->setYmax (realYMax);
	  if (pw->getYmaxBox ()) pw->getYmaxBox ()->setValue (realYMax);
	}
	else realYMax = pw->getYmax ();
	double dx = 0.075 * (realXMax - realXMin);
	double dy = 0.075 * (realYMax - realYMin);
	axisX->setRange(realXMin - dx, realXMax + dx);
	axisY->setRange(realYMin - dy, realYMax + dy);
	// axisX->setRange(idxVector.front () - dx, idxVector.back () + dx);
      
	chart->addAxis(axisX, Qt::AlignBottom);
	chart->addAxis(axisY, Qt::AlignLeft);
	QList<QAbstractSeries *> seriesList = chart->series ();
	for (int i = 0; i < seriesList.size (); i++) {
	  seriesList[i]->attachAxis(axisX);
	  seriesList[i]->attachAxis(axisY);
	  if (i == 0) {
	    PlotLabel  *al = pw->getActiveLabel ();
	    if (al && !al->getLabel ().isEmpty ()) {
	      TextItem *ti = new TextItem (chart, seriesList[i]);
#if 1
	      ti->setFontP (fontScale, al->getFont ());
#else
	      if (fontScale != 1.0) {
		QFont ufont = al->getFont ();
		QFont tfont = scaleFont (fontScale, ufont);
		ti->setFont (tfont);
	      }
	      else {
		ti->setFont (al->getFont ());
	      }
#endif
	      ti->setColour (al->getColour ());
	      ti->setAngle (al->getAngle ());
	      ti->setAlignment (al->getHorizontalAlignment (),
				al->getVerticalAlignment ());
	      ti->setText (al->getLabel (), al->getPosition (),
			   al->getWorldCoordinates ());
	    }
	    
	    QList<PlotLabel *> pq = pw->getPlotLabels ();
	    for (int j = 0; j < pq.size ();  j++) {
	      PlotLabel  *al = pw->getPlotLabels ().at (j);
	      if (al->getEnable ()) {
		  TextItem *ti = new TextItem (chart, seriesList[i]);
#if 1
		  ti->setFontP (fontScale, al->getFont ());
#else
		  if (fontScale != 1.0) {
		    QFont ufont = al->getFont ();
		    QFont tfont = scaleFont (fontScale, ufont);
		    ti->setFontP (tfont);
		  }
		  else {
		    ti->setFontP (al->getFont ());
		  }
#endif
		  ti->setColour (al->getColour ());
		  ti->setAngle (al->getAngle ());
		  ti->setAlignment (al->getHorizontalAlignment (),
				    al->getVerticalAlignment ());
		  ti->setText (al->getLabel (), al->getPosition (),
			       al->getWorldCoordinates ());
		}
	    }
	  }
	}
      }
    }
  }
  else
    mw->printError (tr ("Index evaluation error."));


  if (autoIdx) {
    cmd = QString (")erase %1").arg (IDXVAR);
    mw->processLine (false, cmd);
  }

  for (int i = 0; i < pw->getPlotParameters ().size (); i++) {
    QString vname = pw->getPlotParameters ().at(i)->vname ();
    if (vname.isEmpty ()) {
      vname = QString ("%1%2").arg (PARMVAR).arg (i);
      cmd = QString (")erase %1").arg (vname);
      mw->processLine (false, cmd);
    }
  }
}

enum {
  UNITS_PIXELS,
  UNITS_CM,
  UNITS_INCHES,
  UNITS_PERCENT
};

static QComboBox *
unitsComboBox ()
{
  QComboBox *unitsCombo = new QComboBox ();
  unitsCombo->addItem ("Pixels",	QVariant(UNITS_PIXELS));
  unitsCombo->addItem ("Centimetres",	QVariant(UNITS_CM));
  unitsCombo->addItem ("Inches",	QVariant(UNITS_INCHES));
  unitsCombo->addItem ("Percent",	QVariant(UNITS_PERCENT));
  return unitsCombo;
}

bool Chart2DWindow::showPreview (QPixmap plotPixmap)
{
  QDialog dialog (this, Qt::Dialog);
  dialog.setWindowTitle ("qapl preview");
  QGridLayout *layout = new QGridLayout;
  dialog.setLayout (layout);

  QLabel *image = new QLabel();
  image->setPixmap (plotPixmap);

  int row = 0;
  int col = 0;
  
  layout->addWidget (image, row, col++, 1, 2);

  row++;
  col = 0;

  QPushButton *closeButton = new QPushButton (QObject::tr ("Close"));
  layout->addWidget (closeButton, row, col++);
  QObject::connect (closeButton, &QPushButton::clicked,
                    &dialog, &QDialog::reject);

  QPushButton *exportButton = new QPushButton (QObject::tr ("Export"));
  exportButton->setAutoDefault (true);
  exportButton->setDefault (true);
  layout->addWidget (exportButton, row, col++);
  QObject::connect (exportButton, &QPushButton::clicked,
                    &dialog, &QDialog::accept);
  bool rc = true;
  int res = dialog.exec ();
  if (res != QDialog::Accepted) rc = false;
  return rc;
}

void
Chart2DWindow::exportAsImage ()
{
  QFileDialog dialog (this, "Export As...", ".",
		      tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
  
  dialog.setOption (QFileDialog::DontUseNativeDialog);
  QLayout *layout = dialog.layout ();
  QGroupBox *gbox = new QGroupBox ();
  QGridLayout *btnlayout = new QGridLayout ();
  gbox->setLayout (btnlayout);

  int row = 0;
  int col = 0;

  QLabel widthLbl(tr ("Width"));
  btnlayout->addWidget (&widthLbl, row, col++);

  //  static double currentWidth = 640.0;
  QDoubleSpinBox *widthBox = new QDoubleSpinBox ();
  widthBox->setDecimals (4);
  widthBox->setMinimum (16.0);
  widthBox->setMaximum (5000.0);
  widthBox->setValue (currentWidth);
  btnlayout->addWidget (widthBox, row, col++);

  QComboBox *widthUnits = unitsComboBox ();
  btnlayout->addWidget (widthUnits, row, col++);
  
  QCheckBox *previewButton = new QCheckBox (tr ("Preview"));
  previewButton->setCheckState (Qt::Unchecked);
  btnlayout->addWidget (previewButton, row, col++);

  row++;
  col = 0;

  QLabel heightLbl(tr ("Height"));
  btnlayout->addWidget (&heightLbl, row, col++);

  //  static double currentHeight = 640.0;
  QDoubleSpinBox *heightBox = new QDoubleSpinBox ();
  heightBox->setDecimals (4);
  heightBox->setMinimum (16.0);
  heightBox->setMaximum (5000.0);
  heightBox->setValue (currentHeight);
  btnlayout->addWidget (heightBox, row, col++);

  QComboBox *heightUnits = unitsComboBox ();
  btnlayout->addWidget (heightUnits, row, col++);

  connect (widthUnits,
	   QOverload<int>::of(&QComboBox::activated),
	   [=](int index){
	     if (index == UNITS_PERCENT)
	       widthBox->setValue (100.0);
	     else
	       widthBox->setValue (currentWidth);
	   });

  connect (heightUnits,
	   QOverload<int>::of(&QComboBox::activated),
	   [=](int index){
	     if (index == UNITS_PERCENT)
	       heightBox->setValue (100.0);
	     else
	       heightBox->setValue (currentHeight);
	   });


  row++;
  col = 0;  

  QLabel ppuLbl(tr ("Pixels per unit"));
  btnlayout->addWidget (&ppuLbl, row, col++);

  QDoubleSpinBox *ppuBox = new QDoubleSpinBox ();
  ppuBox->setMinimum (16.0);
  btnlayout->addWidget (ppuBox, row, col++);


  layout->addWidget (gbox);
  
  dialog.setWindowModality(Qt::WindowModal);
  dialog.setAcceptMode(QFileDialog::AcceptSave);

  int drc = dialog.exec();
  
  if (drc == QDialog::Accepted) {

    double ppu = ppuBox->value ();

    QVariant widthSel = widthUnits->currentData ();
    int widthUnits = widthSel.toInt ();
    double widthDim = widthBox->value ();
    //    currentWidth = widthDim;
    switch(widthUnits) {	// fixme
    case UNITS_PIXELS:
      break;
    case UNITS_CM:
      widthDim *= ppu;
      break;
    case UNITS_INCHES:
      widthDim *= ppu;
      break;
    case UNITS_PERCENT:
      widthDim *= currentWidth / 100.0;
      break;
    }

    int cvw = chartView->width ();
    int cvh = chartView->height ();

    QVariant heightSel = heightUnits->currentData ();
    int heightUnits = heightSel.toInt ();
    double heightDim = heightBox->value ();
    currentHeight = heightDim;
    switch(heightUnits) {	// fixme
    case UNITS_PIXELS:
      fontScale  = heightDim / (double)cvh;
      break;
    case UNITS_CM:
      heightDim *= ppu;
      fontScale  = heightDim / (double)cvh;
      break;
    case UNITS_INCHES:
      heightDim *= ppu;
      fontScale  = heightDim / (double)cvh;
      break;
    case UNITS_PERCENT:
      fontScale = heightDim / 100.0;
      heightDim *= currentHeight / 100.0;
      break;
    }
    
    // https://forum.qt.io/topic/76684/qpaintdevice-cannot-destroy-paint-device-that-is-being-painted/3

    heightScale = heightDim / (double)cvh;
    widthScale  = widthDim / (double)cvw;
    avgScale = (heightScale + widthScale) / 2.0;
#if 0
#if 1
    fontScale  = heightDim / (double)cvh;
#else
    double diagInit   = hypot ((double)cvw, (double)cvh);
    double diagExport = hypot (widthDim, heightDim);
    fontScale  = diagExport / diagInit;
#endif
#endif


    /**** adjust title font size ****/
    
    QFont font = chart->titleFont ();
    double psf = font.pointSizeF ();
    bool fontChanged = false;
    if (psf < 0.0) {
      psf = (double) font.pixelSize ();
      if (psf > 0.0) {
	psf *= fontScale;
	QFont newFont = QFont (font);
	newFont.setPointSizeF (psf);
	chart->setTitleFont (newFont);
	fontChanged = true;
      }
    }
    else {
      psf *= fontScale;
      QFont newFont = QFont (font);
      newFont.setPointSizeF (psf);
      chart->setTitleFont (newFont);
      fontChanged = true;
    }
    

    /**** adjust legend font size ****/

    QLegend *legend = chart->legend ();
    //    QFont legendFont = legend->font ();
    psf = font.pointSizeF ();
    //    bool legendFontChanged = false;
    if (psf < 0.0) {
      psf = (double) font.pixelSize ();
      if (psf > 0.0) {
	psf *= fontScale;
	QFont newFont = QFont (font);
	newFont.setPointSizeF (psf);
	legend->setFont (newFont);
	//	legendFontChanged = true;
      }
    }
    else {
      psf *= fontScale;
      QFont newFont = QFont (font);
      newFont.setPointSizeF (psf);
      legend->setFont (newFont);
      //      legendFontChanged = true;
    }

    chartView->setMinimumSize ((int)widthDim, (int)heightDim);
    QPixmap plotPixmap (chartView->width(), chartView->height());
    plotPixmap.fill(Qt::white);

    QPainter *paint = new QPainter(&plotPixmap);
    // see void QPainter::setBackgroundMode(Qt::BGMode mode)
    //  paint->setPen(*(new QColor(255,34,255,255)));
    //QColor colour(255,34,255,255);
    chartView->render(paint);

    drawCurves ();

    paint->end ();

    bool doit = true;
    if (Qt::Checked == previewButton->checkState ())
      doit = showPreview (plotPixmap);

    if (doit) {
      currentImageExportFile = dialog.selectedFiles().at(0);
      QFile file(currentImageExportFile);
      file.open(QIODevice::WriteOnly);
      // bool QPixmap::save(const QString &fileName,
      //   const char *format = nullptr, int quality = -1) const
      //https://doc.qt.io/qt-5/qtimageformats-index.html
      /****
	   BMP	Windows Bitmap				Read/write
	   JPG	Joint Photographic Experts Group	Read/write
	   JPEG	Joint Photographic Experts Group	Read/write
	   PNG	Portable Network Graphics		Read/write
	   PPM	Portable Pixmap				Read/write
	   XBM	X11 Bitmap				Read/write
	   XPM	X11 Pixmap				Read/write
    ****/
      bool rc = plotPixmap.save(&file);
      file.close ();

      if (!rc) {
	QMessageBox msgBox;
	msgBox.setIcon (QMessageBox::Critical);
	msgBox.setText (tr ("Export failed."));
	msgBox.setInformativeText(tr ("Might be due to an unsupport format"));
	msgBox.exec();
      }
    }
    
    fontScale = 1.0;
    heightScale = 1.0;
    widthScale = 1.0;
    avgScale = 1.0;
    chartView->setMinimumSize (cvw, cvh);

    if (fontChanged)
      chart->setTitleFont (font);
    //    if (legendFontChanged)
    //      legend->setFont (legendFont);
    drawCurves ();

  }
}

void Chart2DWindow::createMenubar ()
{
  readout = new QLabel ();
  readout->setAlignment (Qt::AlignLeft);
  menuBar()->setCornerWidget (readout);
  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
  
  QAction *exportAct =
    fileMenu->addAction(tr("&Export..."), this,
			    &Chart2DWindow::exportAsImage);
  exportAct->setStatusTip(tr("Export chart"));
}

QaplChartView::QaplChartView (Chart2DWindow *parent)
  : QChartView (parent)
{
  pc = parent;
  rubberBand = nullptr;
}

QPointF QaplChartView::coordinateTransform (QPoint d)
{
  auto const scenePos1 = mapToScene(d);
  auto const chartItemPos1 = chart()->mapFromScene(scenePos1); 
  return chart()->mapToValue(chartItemPos1);
}

void QaplChartView::wheelEvent(QWheelEvent *event)
{
  Qt::KeyboardModifiers mods = event->modifiers ();
  bool ctl = (0 == (mods & Qt::ControlModifier));
  QPoint angle = event->angleDelta ();
#if 0
  fprintf (stderr, "wheel mods 0x%08x %d %d\n",
	   (int)mods, angle.x (), angle.y ());
#endif

  double direction = (angle.y () > 0.0) ? -1.0 : 1.0;
  
  QPointF initP  = coordinateTransform (origin);
  QPointF finalP = coordinateTransform (currentPoint);
  double ipx = initP.x ();
  double fpx = finalP.x ();
  if (ipx > fpx) {double tmp = ipx; ipx = fpx; fpx = tmp; }

  double initReal  = pc->pw->getRealInit ();
  double initImag  = pc->pw->getImagInit ();
  double finalReal = pc->pw->getRealFinal ();
  double finalImag = pc->pw->getImagFinal ();

  ComplexSpinBox *ibox =  pc->pw->getRangeInit ();
  ComplexSpinBox *fbox =  pc->pw->getRangeFinal ();
  
#define RANGE_STEP 0.005
  
  double deltaReal = direction * (finalReal -initReal) * RANGE_STEP;
  double deltaImag = direction * (finalImag -initImag) * RANGE_STEP;

  if (ctl) {
    /*** motion ***/
    
    ibox->setComplex (initReal  + deltaReal, initImag  + deltaImag);
    fbox->setComplex (finalReal + deltaReal, finalImag + deltaImag);
  }
  else {
    /*** zoom ***/

    ibox->setComplex (initReal  - deltaReal, initImag  - deltaImag);
    fbox->setComplex (finalReal + deltaReal, finalImag + deltaImag);
  }

  pc->pw->drawCurves ();
  event->accept ();
}

bool QaplChartView::chartLabel (QPoint screenPoint, bool editMode)
{
  bool rc = false;
  QDialog dialog (this, Qt::Dialog);
  dialog.setModal (false);
  dialog.setWindowTitle ("Chart labels");
  QGridLayout *layout = new QGridLayout;
  dialog.setLayout (layout);

  PlotLabel *activeLabel = pc->pw->getActiveLabel ();

  QPointF worldCoords;

  bool isWorld = activeLabel->getWorldCoordinates ();
  if (editMode) {
    worldCoords = activeLabel->getPosition ();
    screenPoint = QPoint ((int)(worldCoords.x ()),
			  (int)(worldCoords.y ()));
  }
  else	
    worldCoords = coordinateTransform (screenPoint);

  int row = 0;
  int col = 0;
  int colMax = 0;
  
  QLabel *labelLabel = new QLabel (tr ("Label:"));
  layout->addWidget (labelLabel, row, col++);

  QLineEdit *labelBox = new QLineEdit ();
  labelBox->setText (activeLabel->getLabel ());
  connect (labelBox,
           &QLineEdit::editingFinished,
          [=](){
	    activeLabel->setLabel (labelBox->text ());
	    pc->pw->drawCurves ();
          });
  layout->addWidget (labelBox, row, col++, 1, 2);

  row++;
  if (colMax < col) colMax = col;
  col = 0;

  
  QLabel *positonLabel = new QLabel (tr ("Position:"));
  layout->addWidget (positonLabel, row, col++);
  
  SciDoubleSpinBox *positionXBox =
    new SciDoubleSpinBox (SciDoubleSpinBox::Unset);
  connect(positionXBox,
	  &SciDoubleSpinBox::valueChanged,
	  [=](){
	    double val = positionXBox->getValue ();
	    if (isWorld)
	      activeLabel->setPositionX (val);
	    else
	      activeLabel->setPositionX (val);
	    pc->pw->drawCurves ();
	  });
  layout->addWidget (positionXBox, row, col++);
  
  SciDoubleSpinBox *positionYBox = 
    new SciDoubleSpinBox (SciDoubleSpinBox::Unset);
  connect(positionYBox,
	  &SciDoubleSpinBox::valueChanged,
	  [=](){
	    double val = positionYBox->getValue ();
	    if (isWorld)
	      activeLabel->setPositionY (val);
	    else
	      activeLabel->setPositionY (val);
	    pc->pw->drawCurves ();
	  });
  layout->addWidget (positionYBox, row, col++);

  
  if (isWorld) {
    positionXBox->setValue (worldCoords.x ());
    positionYBox->setValue (worldCoords.y ());
  }
  else {
    positionXBox->setValue (QPointF (screenPoint).x ());
    positionYBox->setValue (QPointF (screenPoint).y ());
  }
  
  row++;
  if (colMax < col) colMax = col;
  col = 0;

  col++;
  QPushButton *labelFontButton =
    new QPushButton (QObject::tr ("Label font"));
  layout->addWidget (labelFontButton, row, col++);
  QObject::connect (labelFontButton, &QPushButton::clicked,
		    [=](){
		      bool ok;

		      QFont font = QFontDialog::getFont(&ok,
							activeLabel->getFont (),
							this,
							"Label font");
		      if (ok) {
			activeLabel->setFont (font);
			pc->pw->drawCurves ();
		      } 
		    });

  QPushButton *labelColourButton =
    new QPushButton (QObject::tr ("Label colour"));
  layout->addWidget (labelColourButton, row, col++);
  QObject::connect (labelColourButton, &QPushButton::clicked,
		    [=](){
		      QColor colour
			= QColorDialog::getColor (activeLabel->getColour (),
						  this,
						  "Label colour",
					  QColorDialog::ShowAlphaChannel);
		      if (colour.isValid ()) {
			activeLabel->setColour (colour);
			pc->pw->drawCurves ();
		      }
		    });
  
  row++;
  if (colMax < col) colMax = col;
  col = 0;

  QLabel *angleLabel = new QLabel (tr ("Angle:"));
  layout->addWidget (angleLabel, row, col++);

  QDoubleSpinBox *angleBox = new QDoubleSpinBox ();
  angleBox->setRange (-200.0, 200.0);
  angleBox->setAccelerated (true);
  angleBox->setValue (activeLabel->getAngle ());
  connect(angleBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
	  [=](double d){
	    bool dChanged = false;
	    if (d < -180.0) {
	      d += 360.0;
	      dChanged = true;
	    }
	    else if (d > 180.0) {
	      d -= 360.0;
	      dChanged = true;
	    }
	    if (dChanged) angleBox->setValue (d);
	    activeLabel->setAngle (d);
	    pc->pw->drawCurves ();
	  });
  layout->addWidget (angleBox, row, col++);
  
  row++;
  if (colMax < col) colMax = col;
  col = 0;

  QLabel *alignmentLabel = new QLabel (tr ("Alignment:"));
  layout->addWidget (alignmentLabel, row, col++);
  
  QComboBox *verticalAlignment = new QComboBox ();
  verticalAlignment->addItem ("Top",      QVariant(Qt::AlignTop));
  verticalAlignment->addItem ("Bottom",   QVariant(Qt::AlignBottom));
  verticalAlignment->addItem ("Centre",   QVariant(Qt::AlignVCenter));
  verticalAlignment->addItem ("Baseline", QVariant(Qt::AlignBaseline));
  int vi =
    verticalAlignment->findData (QVariant (activeLabel->getVerticalAlignment ()));
  if (vi >= 0) verticalAlignment->setCurrentIndex (vi);
  connect (verticalAlignment, QOverload<int>::of(&QComboBox::activated),
          [=](int index __attribute__((unused)))
          {
	    activeLabel->setVerticalAlignment (verticalAlignment->currentData ().toInt ());
	    pc->pw->drawCurves ();
	  });
  layout->addWidget (verticalAlignment, row, col++);

  QComboBox *horizontalAlignment = new QComboBox ();
  horizontalAlignment->addItem ("Left",    QVariant(Qt::AlignLeft));
  horizontalAlignment->addItem ("Right",   QVariant(Qt::AlignRight));
  horizontalAlignment->addItem ("Centre",  QVariant(Qt::AlignHCenter));
  horizontalAlignment->addItem ("Justify", QVariant(Qt::AlignJustify));
  int hi =
    horizontalAlignment->findData (QVariant (activeLabel->getHorizontalAlignment ()));
  if (hi >= 0) horizontalAlignment->setCurrentIndex (hi);
  connect (horizontalAlignment, QOverload<int>::of(&QComboBox::activated),
          [=](int index __attribute__((unused)))
          {
	    activeLabel->setHorizontalAlignment (horizontalAlignment->currentData ().toInt ());
	    pc->pw->drawCurves ();
	  });
  layout->addWidget (horizontalAlignment, row, col++);
  

  row++;
  if (colMax < col) colMax = col;
  col = 0;

  col++;
  QCheckBox *worldButton = new QCheckBox (tr ("World coordinates"));
  worldButton->setEnabled (!editMode);
  if (isWorld) {
    worldButton->setCheckState (Qt::Checked);
    activeLabel->setWorldCoordinates (true);
    activeLabel->setPosition (worldCoords);
    positionXBox->setValue (worldCoords.x ());
    positionYBox->setValue (worldCoords.y ());
  }
  else {
    worldButton->setCheckState (Qt::Unchecked);
    activeLabel->setWorldCoordinates (true);
    activeLabel->setPosition (QPointF (screenPoint));
  }
  connect (worldButton,
	   &QCheckBox::stateChanged,
	   [=](int index)
	   {
	     if (index == Qt::Checked) {
	       activeLabel->setPosition (worldCoords);
	       activeLabel->setWorldCoordinates (true);
	       positionXBox->setValue (worldCoords.x ());
	       positionYBox->setValue (worldCoords.y ());
	     }
	     else {
	       activeLabel->setPosition (QPointF (screenPoint));
	       activeLabel->setWorldCoordinates (false);
	       positionXBox->setValue (QPointF (screenPoint).x ());
	       positionYBox->setValue (QPointF (screenPoint).y ());
	     }
	     pc->pw->drawCurves ();
	   });
  layout->addWidget (worldButton, row, col++);

  row++;
  if (colMax < col) colMax = col;
  col = 0;

  QPushButton *showButton = new QPushButton (QObject::tr ("Show"));
  layout->addWidget (showButton, row, colMax-3);
  connect (showButton, &QPushButton::clicked,
	   [=](){
	     pc->pw->drawCurves ();
	   });

  QPushButton *saveButton = new QPushButton (QObject::tr ("Save"));
  saveButton->setAutoDefault (true);
  saveButton->setDefault (true);
  layout->addWidget (saveButton, row, colMax-2);
    QObject::connect (saveButton, &QPushButton::clicked,
		      &dialog, &QDialog::accept);

  QPushButton *closeButton = new QPushButton (QObject::tr ("Cancel"));
  layout->addWidget (closeButton, row, colMax-1);
  QObject::connect (closeButton, &QPushButton::clicked,
                    &dialog, &QDialog::reject);
  

  int drc = dialog.exec ();
  if (drc == QDialog::Accepted) {
    if (editMode)
      rc = true;
    else {
      if (!activeLabel->getLabel ().isEmpty ()) {
	PlotLabel *copy = new PlotLabel (*(pc->pw->getActiveLabel ()));
	pc->pw->appendPlotLabels (copy);
	//      pc->pw->deleteActiveLabel ();
      }
    }
  }
  // activeLabel->clearLabel ();
  pc->pw->drawCurves ();
  return rc;
}

void QaplChartView::mousePressEvent(QMouseEvent *event)
{
  Qt::MouseButton button = event->button();
  if (button == Qt::RightButton) {	// pop up label stuff
    pc->pw->newActiveLabel ();
    chartLabel (currentPoint, false);
    event->accept ();
  }
  else {
    origin = event->pos();
    origin.setY (0);
    currentPoint = event->pos();
    if (!rubberBand)
      rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    rubberBand->setGeometry(QRect(origin, QSize()));
    rubberBand->show();
    event->accept ();
  }
}

void QaplChartView::mouseMoveEvent(QMouseEvent *event)
{
  currentPoint = event->pos();
  QPointF pt = coordinateTransform (currentPoint);
  QString pts = QString ("%1, %2       ")
    .arg (QString::number (pt.x (), 'g', 3))
    .arg (QString::number (pt.y (), 'g', 3));
  pc->readout->setText (pts);
  
  if (rubberBand && !rubberBand->isHidden ()) {
    currentPoint.setY (this->height ());
    rubberBand->setGeometry(QRect(origin, currentPoint).normalized());
    event->accept ();
  }
  else event->ignore ();
}

void QaplChartView::mouseReleaseEvent(QMouseEvent *event)
{
  Qt::MouseButton button = event->button();
  if (button == Qt::RightButton) {	// do nothing--for lbl popup
    event->accept ();
  }
  else
    if (rubberBand && !rubberBand->isHidden () && (origin != currentPoint)) {
      rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
      rubberBand->hide();

      QPointF initP  = coordinateTransform (origin);
      QPointF finalP = coordinateTransform (currentPoint);

      double initReal  = pc->pw->getRealInit ();
      double initImag  = pc->pw->getImagInit ();
      double finalReal = pc->pw->getRealFinal ();
      double finalImag = pc->pw->getImagFinal ();

      double ipx = initP.x ();
      double fpx = finalP.x ();
      if (ipx > fpx) {double tmp = ipx; ipx = fpx; fpx = tmp; }
      double fracIx = (initP.x ()  - initReal) / (finalReal - initReal);
      double fracFx = (finalP.x () - initReal) / (finalReal - initReal);
      double imagIx = initImag + fracIx * (finalImag - initImag);
      double imagFx = initImag + fracFx * (finalImag - initImag);
      double realIx = initP.x ();
      double realFx = finalP.x ();
      
      ComplexSpinBox *ibox =  pc->pw->getRangeInit ();
      ComplexSpinBox *fbox =  pc->pw->getRangeFinal ();
      ibox->setComplex (realIx, imagIx);
      fbox->setComplex (realFx, imagFx);

      pc->pw->drawCurves ();

      // determine selection, for example using QRect::intersects()
      // and QRect::contains().
      event->accept ();
    }
    else event->ignore ();
}

void Chart2DWindow::closeEvent(QCloseEvent *event)
{
  Q_UNUSED (event);
  Q_EMIT destroyed ();
}

Chart2DWindow::Chart2DWindow (Plot2DWindow *parent, MainWindow *mainWin)
  : QMainWindow(parent)
{
  this->setWindowTitle ("qapl 2D Plot");
  pw = parent;
  mw = mainWin;
  fontScale = 1.0;
  heightScale = 1.0;
  widthScale = 1.0;
  avgScale = 1.0;
  currentWidth = 640.0;
  currentHeight = 640.0;

  QWidget *hw = new QWidget ();
  this->setCentralWidget(hw);
  QGridLayout *layout = new QGridLayout;

  createMenubar ();

  // https://stackoverflow.com/questions/44067831/get-mouse-coordinates-in-qchartviews-axis-system
  
  //  chartView = new QChartView (this);
  chartView = new QaplChartView (this);
  chartView->setMinimumWidth (650);
  chartView->setMinimumHeight (420);

  layout->addWidget (chartView, 0, 0, 1, 3);




  hw->setLayout(layout);
  this->show ();
}

Chart2DWindow::~Chart2DWindow ()
{
}

// https://doc.qt.io/qt-5/qtcharts-customchart-example.html

#include <QScatterSeries>
#include <QCategoryAxis>
#include <QPolarChart>

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
    if (fontScale != 1.0) {
      //      fprintf (stderr, "setting pw %g\n",
      //	       10.0 * fontScale * (double)pen.width ());
      pen.setWidth ((int)(10.0 * fontScale * (double)pen.width ()));
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
    aplXExpr.replace (QString ("%1"), QString (IDXVAR));
    QString aplYExpr = pw->getAplYExpression ();
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
      drawCurve (pw->getPlotCurves ().at (i)->Xexpression (),
		 pw->getPlotCurves ().at (i)->Yexpression (),
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
	  QString family = pw->getAxisTitleFont ().family ();
	  int pointSize  =
	    (int) (2.0 * fontScale *
		   pw->getAxisTitleFont ().pointSizeF ());
	  int weight     = pw->getAxisTitleFont ().weight ();
	  bool italic    = pw->getAxisTitleFont ().italic ();
	  QFont tfont = QFont(family, pointSize, weight, italic);
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
	axisPen.setWidth(2 * fontScale);
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
  UNITS_INCHES
};

static QComboBox *
unitsComboBox ()
{
  QComboBox *unitsCombo = new QComboBox ();
  unitsCombo->addItem ("Pixels",	QVariant(UNITS_PIXELS));
  unitsCombo->addItem ("Centimetres",	QVariant(UNITS_CM));
  unitsCombo->addItem ("Inches",	QVariant(UNITS_INCHES));
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
    currentWidth = widthDim;
    switch(widthUnits) {	// fixme
    case UNITS_PIXELS:
      break;
    case UNITS_CM:
      widthDim *= ppu;
      break;
    case UNITS_INCHES:
      widthDim *= ppu;
      break;
    }

    QVariant heightSel = heightUnits->currentData ();
    int heightUnits = heightSel.toInt ();
    double heightDim = heightBox->value ();
    currentHeight = heightDim;
    switch(heightUnits) {	// fixme
    case UNITS_PIXELS:
      break;
    case UNITS_CM:
      heightDim *= ppu;
      break;
    case UNITS_INCHES:
      heightDim *= ppu;
      break;
    }
    
    // https://forum.qt.io/topic/76684/qpaintdevice-cannot-destroy-paint-device-that-is-being-painted/3
    int cvw = chartView->width ();
    int cvh = chartView->height ();

#if 1
    fontScale  = heightDim / (double)cvh;
#else
    double diagInit   = hypot ((double)cvw, (double)cvh);
    double diagExport = hypot (widthDim, heightDim);
    fontScale  = diagExport / diagInit;
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
    
    fontScale  = 1.0;
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
  readout = new QLabel ("           ");
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

void QaplChartView::mousePressEvent(QMouseEvent *event)
{
  origin = event->pos();
  origin.setY (0);
  currentPoint = event->pos();
  if (!rubberBand)
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

  rubberBand->setGeometry(QRect(origin, QSize()));
  rubberBand->show();
}

void QaplChartView::mouseMoveEvent(QMouseEvent *event)
{
  currentPoint = event->pos();
  QPointF pt = coordinateTransform (currentPoint);
  QString pts = QString ("%1, %2")
    .arg (QString::number (pt.x (), 'g', 3))
    .arg (QString::number (pt.y (), 'g', 3));
  pc->readout->setText (pts);
  currentPoint.setY (this->height ());
  if (rubberBand) {
    rubberBand->setGeometry(QRect(origin, currentPoint).normalized());
  }
}

void QaplChartView::mouseReleaseEvent(QMouseEvent *event)
{
  if (origin != currentPoint) {
    rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
    rubberBand->hide();

    QPointF initP  = coordinateTransform (origin);
    QPointF finalP = coordinateTransform (currentPoint);

    double initReal  = pc->pw->getRealInit ();
    double initImag  = pc->pw->getImagInit ();
    double finalReal = pc->pw->getRealFinal ();
    double finalImag = pc->pw->getImagFinal ();

#if 0
    double ymin = pc->pw->getYmin ();
    double ymax = pc->pw->getYmax ();
#endif

    /****
                    p1x
		     |
		     |		             v
                     |                      p2x
		     |			     |
		     v		             v
	 |----------------------------------------------|
      initR                                          finalR

      fracI = (p1x - initR) / (finalR - initR)
      fracF = (p2x - initR) / (finalR - initR)
      initR'  = p1x
      finalR' = p2x
      initI'  = initI + fracI * (finalI - initI)
      finalI' = initI + fracF * (finalI - initI)
      
     ****/

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

#if 0
    double ipy = initP.y ();
    double fpy = finalP.y ();
    if (ipy > fpy) {double tmp = ipy; ipy = fpy; fpy = tmp; }
    fprintf (stderr, "ipy fpy: %g %g\n", ipy, fpy);
    
    fprintf (stderr, "ymin ymax: %g %g\n", ymin, ymax);
    double fracMinY = (ipy - ymin) / (ymax - ymin);
    double fracMaxY = (fpy - ymin) / (ymax - ymin);
    fprintf (stderr, "frac %g %g\n", fracMinY, fracMaxY);
#endif
    
    pc->pw->drawCurves ();

    // determine selection, for example using QRect::intersects()
    // and QRect::contains().
  }
}

Chart2DWindow::Chart2DWindow (Plot2DWindow *parent, MainWindow *mainWin)
  : QMainWindow(parent)
{
  this->setWindowTitle ("qapl 2D Plot");
  pw = parent;
  mw = mainWin;
  fontScale = 1.0;
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

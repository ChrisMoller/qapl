// https://doc.qt.io/qt-5/qtcharts-customchart-example.html

#include <QScatterSeries>
#include <QCategoryAxis>

#include "mainwindow.h"
#include "plot2dwindow.h"
#include "chart2dwindow.h"
#include <values.h>
#include <apl/libapl.h>

#define PLOTVAR "plotvarλ"
#define IDXVAR  "idxvarλ"

bool Chart2DWindow::appendSeries (double x, double y, series_mode_e mode,
				 double &realMax, double &realMin)
{
  bool rc = true;
  if (realMax < y) realMax = y;
  if (realMin > y) realMin = y;
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

void Chart2DWindow::drawCurve (QString aplExpr, aspect_e aspect,
			       QString label, QPen pen, series_mode_e mode)
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

	switch (mode) {
	case MODE_BUTTON_SPLINE:
	  series = new QSplineSeries ();
	  break;
	case MODE_BUTTON_LINE:
	  series = new QLineSeries ();
	  break;
	case MODE_BUTTON_SCATTER:
	  series = new QScatterSeries ();
	  break;
	default:
	  break;
	}
	
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
			       (double)get_int (result, i), mode,
			       realMax, realMin))
	      run = false;
	    break;
	  case CCT_FLOAT:
	    if (!appendSeries (idxVector[i], get_real (result, i), mode,
			       realMax, realMin))
	      run = false;
	    break;
	  case CCT_COMPLEX:
	    {
	      switch (aspect) {
		case ASPECT_REAL:
		  if (!appendSeries (idxVector[i], get_real (result, i), mode,
				     realMax, realMin))
		    run = false;
		  break;
	      case ASPECT_IMAG:
		if (!appendSeries (idxVector[i], get_imag (result, i), mode,
				   realMax, realMin))
		  run = false;
		break;
	      case ASPECT_MAGNITUDE:
		{
		  std::complex<double> val (get_real (result, i),
					    get_imag (result, i));
		  if (!appendSeries (idxVector[i], std::abs (val), mode,
				     realMax, realMin))
		    run = false;
		}
		break;
	      case ASPECT_PHASE:
		std::complex<double> val (get_real (result, i),
					  get_imag (result, i));
		if (!appendSeries (idxVector[i], std::arg (val), mode,
				   realMax, realMin))
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
	    seriesCount = static_cast<QSplineSeries*>(series)->count ();
	    static_cast<QSplineSeries*>(series)->setPen (pen);
	    break;
	  case MODE_BUTTON_LINE:
	    seriesCount = static_cast<QLineSeries*>(series)->count ();
	    static_cast<QLineSeries*>(series)->setPen (pen);
	    break;
	  case MODE_BUTTON_SCATTER:
	    seriesCount = static_cast<QScatterSeries*>(series)->count ();
	    static_cast<QScatterSeries*>(series)->setPen (pen);
	    //	    https://doc.qt.io/qt-5/qscatterseries.html#markerShape-prop
	    break;
	  default:
	    break;
	  }
	  
	  if (seriesCount > 0 && idxElementCount == seriesCount) {
	    series->setName (label);
	    // fixme -- scale pen width
	    //	    series->setWidth(2 * fontScale);

	    chartView->chart()->addSeries(series);

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

	      fprintf (stderr, "atc %s\n",
		       toCString (pw->getAxisTitleColour ().name ()));
	      fprintf (stderr, "alc %s\n",
		       toCString (pw->getAxisLabelColour ().name ()));


	      QFont tfont (pw->getAxisTitleFont ());
	      fprintf (stderr, "title before %g\n", tfont.pointSizeF ());
	      double psf = fontScale * tfont.pointSizeF ();
	      tfont.setPointSizeF (psf);
	      fprintf (stderr, "title after %g\n", tfont.pointSizeF ());
	      axisX->setTitleFont(tfont);
	      axisY->setTitleFont(tfont);
	      axisX->setTitleText (pw->getXTitle ());
	      axisY->setTitleText (pw->getYTitle ());
	      
	      QFont ufont (pw->getAxisLabelFont ());
	      fprintf (stderr, "label before %g\n", ufont.pointSizeF ());
	      psf = fontScale * ufont.pointSizeF ();
	      ufont.setPointSizeF (psf);
	      fprintf (stderr, "label after %g\n", ufont.pointSizeF ());
	      axisX->setLabelsFont(ufont);
	      axisY->setLabelsFont(ufont);
	    }

	    {
	      QPen axisPen(QColor("yellow"));
	      axisPen.setWidth(2 * fontScale);
	      axisX->setLinePen(axisPen);
	      axisY->setLinePen(axisPen);

	      axisX->setGridLineVisible(true);
	      axisY->setGridLineVisible(true);
	      // axisY->setShadesPen(Qt::NoPen);
	      // axisY->setShadesBrush(QBrush(QColor(0x99, 0xcc, 0xcc, 0x55)));
	      // axisY->setShadesVisible(true);

	      double dx = 0.075 * (idxVector.back () - idxVector.front ());
	      double dy = 0.075 * (realMax - realMin);
	      axisY->setRange(realMin - dy, realMax + dy);
	      axisX->setRange(idxVector.front () - dx, idxVector.back () + dx);

	      chart->addAxis(axisX, Qt::AlignBottom);
	      chart->addAxis(axisY, Qt::AlignLeft);
	      series->attachAxis(axisX);
	      series->attachAxis(axisY);
	    }

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

  chart->setTheme (pw->getTheme ());

  {
    chart->setTitleFont(pw->getChartTitleFont ());
    chart->setTitleBrush(QBrush(pw->getChartTitleColour ()));
    chart->setTitle (pw->getChartTitle ());
  }
  
  QString aplExpr = pw->getAplExpression ();
  aspect_e aspect = pw->getAspect ();
  QPen pen = pw->getPen ();
  QString label = pw->getCurveTitle ();
  series_mode_e mode = pw->getMode ();
  
  drawCurve (aplExpr, aspect, label, pen, mode);
  for (int i = 0; i < pw->getPlotCurves ().size (); i++) {
    drawCurve (pw->getPlotCurves ()[i]->expression (),
	       pw->getPlotCurves ()[i]->aspect (),
	       pw->getPlotCurves ()[i]->label (),
	       pw->getPlotCurves ()[i]->pen (),
	       pw->getPlotCurves ()[i]->mode ());
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
Chart2DWindow::exportImage ()
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

  QDoubleSpinBox *widthBox = new QDoubleSpinBox ();
  widthBox->setDecimals (4);
  widthBox->setMinimum (16.0);
  widthBox->setMaximum (5000.0);
  widthBox->setValue (512.0);
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

  QDoubleSpinBox *heightBox = new QDoubleSpinBox ();
  heightBox->setDecimals (4);
  heightBox->setMinimum (16.0);
  heightBox->setMaximum (5000.0);
  heightBox->setValue (512.0);
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

    double diagInit   = hypot ((double)cvw, (double)cvh);
    double diagExport = hypot (widthDim, heightDim);
    fontScale  = diagExport / diagInit;


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
    QFont legendFont = legend->font ();
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
    QColor colour(255,34,255,255);
    chartView->render(paint);

    drawCurves ();

    paint->end ();

    bool doit = true;
    if (Qt::Checked == previewButton->checkState ())
      doit = showPreview (plotPixmap);

    if (doit) {
      currentFile = dialog.selectedFiles().first();
      QFile file(currentFile);
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
  fontScale = 1.0;

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

#include <QMenuBar>
#include <QChart>
#include <QChartView>
#include <QColorDialog>
#include <QSplineSeries>
#include <QValueAxis>

#include <complex>

#include <values.h>
#include <apl/libapl.h>

#include "mainwindow.h"
#include "complexspinbox.h"
#include "plot2dwindow.h"
#include "plot2ddata.h"
#include "chart2dwindow.h"
#include "greeklineedit.h"

#define STYLE_NO_PEN		""
#define STYLE_SOLID_LINE	"Solid Line"
#define STYLE_DASH_LINE		"Dash Line"
#define STYLE_DOT_LINE		"Dot Line"
#define STYLE_DASH_DOT_LINE	"Dash Dot Line"
#define STYLE_DASH_DOT_DOT_LINE "Dash Dot Dot Line"

#define STRING_REAL		tr ("Real")
#define STRING_IMAGINARY	tr ("Imaginary")
#define STRING_MAGNITUDE	tr ("Magnitude")
#define STRING_PHASE		tr ("Phase")

#define STRING_SPLINES		tr ("Splines")
#define STRING_LINES		tr ("Lines")
#define STRING_SCATTER		tr ("Scatter")

const char *styleStrings[] = {
  STYLE_NO_PEN,
  STYLE_SOLID_LINE,
  STYLE_DASH_LINE,
  STYLE_DOT_LINE,
  STYLE_DASH_DOT_LINE,
  STYLE_DASH_DOT_DOT_LINE
};

void Plot2DWindow::showPlot2dData (Plot2dData *data)
{
  fprintf (stderr, "resolution:\t\t%d\n",	getResolution ());
  fprintf (stderr, "theme:\t\t\t%d\n",      	getTheme ());

  fprintf (stderr, "chart title font:\t%s\n",
	   toCString (getChartTitleFont ().toString ()));
  fprintf (stderr, "chart title colour:\t%s\n",
	   toCString (getChartTitleColour ().name (QColor::HexArgb)));
  fprintf (stderr, "axis colour:\t\t%s\n",
	   toCString (getAxisColour ().name (QColor::HexArgb)));
  fprintf (stderr, "axis label font:\t%s\n",
	   toCString (getAxisLabelFont ().toString ()));
  fprintf (stderr, "axis label colour:\t%s\n",
	   toCString (getAxisLabelColour ().name (QColor::HexArgb)));
  fprintf (stderr, "axis title font:\t%s\n",
	   toCString (getAxisTitleFont ().toString ()));
  fprintf (stderr, "axis title colour:\t%s\n",
	   toCString (getAxisTitleColour ().name (QColor::HexArgb)));
  
  fprintf (stderr, "chart title:\t\t%s\n",
	   toCString (getChartTitle ()));
  fprintf (stderr, "index variable:\t\t%s\n",
	   toCString (getIndexVariable ()));
  fprintf (stderr, "x title:\t\t%s\n",
	   toCString (getXTitle ()));
  fprintf (stderr, "y title:\t\t%s\n",
	   toCString (getYTitle ()));
  fprintf (stderr, "initial:\t\t%gj%g\n",
	   getRealInit (), getImagInit ());
  fprintf (stderr, "final:\t\t\t%gj%g\n",
	   getRealFinal (), getImagFinal ());

  
  fprintf (stderr, "active aspect:\t\t%d\n",	data->activeCurve.aspect ());
  fprintf (stderr, "active mode:\t\t%d\n",	data->activeCurve.mode ());
  fprintf (stderr, "active X expression:\t%s\n",
	   toCString (data->activeCurve.Xexpression ()));
  fprintf (stderr, "active Y expression:\t%s\n",
	   toCString (data->activeCurve.Yexpression ()));
  fprintf (stderr, "active label:\t\t%s\n",
	   toCString (data->activeCurve.title ()));
  QPen *pen = data->activeCurve.pen ();
  QString colour = pen->brush ().color ().name (QColor::HexArgb);
  fprintf (stderr, "active pen:\t\t%s %i %g\n",
	   toCString (colour), pen->style (), pen->widthF ());

  
}

void Plot2DWindow::drawCurves ()
{
  chart2DWindow->drawCurves ();
}

void Plot2DWindow::pushExpression ()
{
  QString aplXExpr = getAplXExpression ();
  QString aplYExpr = getAplYExpression ();
  QString label = getCurveTitle ();
  QVariant sel = aspectCombo->currentData ();
  aspect_e aspect = (aspect_e)sel.toInt ();
  QPen pen = *getPen ();
  double markerSize = getMarkerSize ();
  sel = modeCombo->currentData ();
  series_mode_e mode = (series_mode_e)sel.toInt ();
  PlotCurve *plotCurve =
    new PlotCurve (aplXExpr, aplYExpr, aspect, label, pen, mode, markerSize);
  plot2DData->plotCurves.append(plotCurve);
}

static QComboBox *
lineStyleCombo (Qt::PenStyle sel)
{
  QComboBox *linestyle_combo = new QComboBox ();
  linestyle_combo->addItem (STYLE_SOLID_LINE,
                            QVariant((int)Qt::SolidLine));
  linestyle_combo->addItem (STYLE_DASH_LINE,
                            QVariant((int)Qt::DashLine));
  linestyle_combo->addItem (STYLE_DOT_LINE,
                            QVariant((int)Qt::DotLine));
  linestyle_combo->addItem (STYLE_DASH_DOT_LINE,
                            QVariant((int)Qt::DashDotLine));
  linestyle_combo->addItem (STYLE_DASH_DOT_DOT_LINE,
                            QVariant((int)Qt::DashDotDotLine));
  int found = linestyle_combo->findData (QVariant ((int)sel));
  if (found != -1)
    linestyle_combo->setCurrentIndex (found);

  return linestyle_combo;
}

/***
    These next two things are here because c++ was giving me
    mysterious errors about consts.  I don't know what they
    were all about or why this works.
***/

static void
doColour (QColor colour, QPen *pen, QPushButton *setColourButton)
{
  pen->setColor (colour);
  QString cmd = QString("background-color: %1;").arg(pen->color().name());
  setColourButton->setStyleSheet(cmd);
}

static void
doWidth (double width, QPen *pen)
{
  pen->setWidthF (width);
}

#if 0
static void
doMarkerSize (double width, QPen *pen)
{
  //  pen->setWidthF (width);
}
#endif

static void
doStyle (int style, QPen *pen)
{
  pen->setStyle (static_cast<Qt::PenStyle>(style));
}

void
Plot2DWindow::updatePen (QPen *pen)
{
  QDialog dialog (this, Qt::Dialog);
  QGridLayout *layout = new QGridLayout;
  dialog.setLayout (layout);

  int row = 0;
  int col = 0;

  QPushButton *setColourButton = new QPushButton (QObject::tr ("Pen colour"));
  {
    QColor bg = getPen ()->brush ().color ();
    QColor fg = QColor (bg);
    float hue;
    float saturation;
    float lightness;
    bg.getHslF (&hue, &saturation, &lightness);
    lightness = (lightness > 0.5) ? 0.0 : 1.0;
    //    saturation = 1.0;
    fg.setHslF (hue, saturation, lightness);
    QString cmd = QString("color: %1; background-color: %2;")
      .arg (fg.name(QColor::HexArgb)). arg (bg.name(QColor::HexArgb));
    setColourButton->setStyleSheet(cmd);
  }
  connect (setColourButton, &QPushButton::clicked,
	   [=](){
	     doColour (QColorDialog::getColor(pen->color (), nullptr),
		       pen, setColourButton);
	   });
  layout->addWidget (setColourButton, row, 1);

  row++;
  col = 0;

  QLabel lbll ("Linestyle");
  layout->addWidget (&lbll, row, col++);

  QComboBox *lineStyle =  lineStyleCombo (pen->style ());
  connect (lineStyle, QOverload<int>::of(&QComboBox::activated),
          [=](int index)
          {
	    QVariant sel = lineStyle->itemData (index);
	    doStyle (sel.toInt (), getPen ());
	  });
  layout->addWidget (lineStyle, row, col++);
  
  row++;
  col = 0;

  QLabel wlbl ("Pen width");
  layout->addWidget (&wlbl, row, col++);
  
  QDoubleSpinBox *widthBox = new QDoubleSpinBox ();
  widthBox->setDecimals (4);
  widthBox->setMinimum (1.0);
  widthBox->setMaximum (64.0);
  widthBox->setValue (pen->widthF ());
  connect(widthBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
	  [=](double d){
	    doWidth (d, pen);
	  });
  layout->addWidget (widthBox, row, col++);
  
  row++;
  col = 0;

  QLabel mlbl ("Marker size");
  layout->addWidget (&mlbl, row, col++);
  
  QDoubleSpinBox *markerSizeBox = new QDoubleSpinBox ();
  markerSizeBox->setDecimals (4);
  markerSizeBox->setMinimum (1.0);
  markerSizeBox->setMaximum (64.0);
  markerSizeBox->setValue (getMarkerSize ());
  connect(markerSizeBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
	  [=](double d){
	    setMarkerSize (d);
	  });
  layout->addWidget (markerSizeBox, row, col++);

  row++;
  col = 0;

  QPushButton *closeButton = new QPushButton (QObject::tr ("Close"));
  closeButton->setAutoDefault (true);
  closeButton->setDefault (true);
  layout->addWidget (closeButton, row, 1);
  QObject::connect (closeButton, &QPushButton::clicked,
                    &dialog, &QDialog::accept);
  
  dialog.exec ();
}

void
Plot2DWindow::updateAspect (PlotCurve *pc)
{
  QDialog dialog (this, Qt::Dialog);
  QGridLayout *layout = new QGridLayout;
  dialog.setLayout (layout);

  int row = 0;
  int col = 0;
  
  QLabel lbla (tr ("Aspect"));
  layout->addWidget (&lbla, row, col++);
  
  row++;
  
  QComboBox *aspectCombo = new QComboBox ();
  aspectCombo->addItem (STRING_REAL,		QVariant(ASPECT_REAL));
  aspectCombo->addItem (STRING_IMAGINARY,	QVariant(ASPECT_IMAG));
  aspectCombo->addItem (STRING_MAGNITUDE,	QVariant(ASPECT_MAGNITUDE));
  aspectCombo->addItem (STRING_PHASE,		QVariant(ASPECT_PHASE));
  int ap = (int)pc->aspect ();
  aspectCombo->setCurrentIndex (ap);
  connect(aspectCombo,
	  QOverload<int>::of(&QComboBox::activated),
	  [=](int index){
	    QVariant sel = aspectCombo->itemData (index);
	    pc->setAspect ((aspect_e)sel.toInt ());
	  });
  layout->addWidget (aspectCombo, row, col++);

  row++;
  col = 0;

  QPushButton *closeButton = new QPushButton (QObject::tr ("Close"));
  closeButton->setAutoDefault (true);
  closeButton->setDefault (true);
  layout->addWidget (closeButton, row, 1);
  QObject::connect (closeButton, &QPushButton::clicked,
                    &dialog, &QDialog::accept);
  
  dialog.exec ();
}

void
Plot2DWindow::updateMode (PlotCurve *pc)
{
  QDialog dialog (this, Qt::Dialog);
  QGridLayout *layout = new QGridLayout;
  dialog.setLayout (layout);

  int row = 0;
  int col = 0;
  
  QLabel lbla (tr ("Mode"));
  layout->addWidget (&lbla, row, col++);
  
  row++;
  
  QComboBox *modeCombo = new QComboBox ();
  connect(modeCombo,
	  QOverload<int>::of(&QComboBox::activated),
	  [=](int index){
	    QVariant sel = modeCombo->itemData (index);
	    pc->setMode ((series_mode_e)sel.toInt ());
	  });
  modeCombo->addItem (STRING_SPLINES,	QVariant(MODE_BUTTON_SPLINE));
  modeCombo->addItem (STRING_LINES,	QVariant(MODE_BUTTON_LINE));
  modeCombo->addItem (STRING_SCATTER,	QVariant(MODE_BUTTON_SCATTER));
  int idx =  modeCombo->findData (QVariant (getMode ())); 
  modeCombo->setCurrentIndex (idx);
  layout->addWidget (modeCombo, row, col++);

  row++;
  col = 0;

  QPushButton *closeButton = new QPushButton (QObject::tr ("Close"));
  closeButton->setAutoDefault (true);
  closeButton->setDefault (true);
  layout->addWidget (closeButton, row, 1);
  QObject::connect (closeButton, &QPushButton::clicked,
                    &dialog, &QDialog::accept);
  
  dialog.exec ();
}

void Plot2DWindow:: deleteStackEntry (int row)
{
  QMessageBox msgBox;
  msgBox.setText(tr ("Are you sure?"));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Cancel);
  int ret = msgBox.exec();
  if (ret == QMessageBox::Yes) {
    fprintf (stderr, "got a yes\n");
    if (row >= 0 && row < getPlotCurves ().size ()) {
      fprintf (stderr, "removing row %d\n", row);
      plot2DData->plotCurves.removeAt (row);
      //      getPlotCurves ().removeAt (row);
    }
  }
}

QString Plot2DWindow::getModeString (int idx)
{
  QString modeString;
  switch(getPlotCurves ()[idx]->mode ()) {
  case MODE_BUTTON_SPLINE:
    modeString = QString (STRING_SPLINES);
    break;
  case MODE_BUTTON_LINE:
    modeString = QString (STRING_LINES);
    break;
  case MODE_BUTTON_SCATTER:
    modeString = QString (STRING_SCATTER);
    break;
  default:
    break;
  }
  return modeString;
}

QString Plot2DWindow::getAspectString (int idx)
{
  QString aspectString;
  switch(getPlotCurves ()[idx]->aspect ()) {
  case ASPECT_REAL:
    aspectString = QString (STRING_REAL);
    break;
  case ASPECT_IMAG:
    aspectString = QString (STRING_IMAGINARY);
    break;
  case ASPECT_MAGNITUDE:
    aspectString = QString (STRING_MAGNITUDE);
    break;
  case ASPECT_PHASE:
    aspectString = QString (STRING_PHASE);
    break;
  }
  return aspectString;
}

enum {
  CURVES_COLUMN_LABEL,
  CURVES_COLUMN_X_EXPRESSION,
  CURVES_COLUMN_Y_EXPRESSION,
  CURVES_COLUMN_ASPECT,
  CURVES_COLUMN_PEN,
  CURVES_COLUMN_MODE,
  CURVES_COLUMN_DELETE,
  CURVES_COLUMN_LAST
};

void Plot2DWindow::fillTable ( QTableWidget *curvesTable)
{
  for (int i = 0; i < getPlotCurves ().size (); i++) {
    QString labelString = getPlotCurves ()[i]->title ().isEmpty () ?
      QString ("---") : getPlotCurves ()[i]->title ();
    QTableWidgetItem *labelItem = new QTableWidgetItem (labelString);
    curvesTable->setItem (i, CURVES_COLUMN_LABEL, labelItem);
    
    QTableWidgetItem *XexprItem
      = new QTableWidgetItem (getPlotCurves ()[i]->Xexpression ());
    curvesTable->setItem (i, CURVES_COLUMN_X_EXPRESSION, XexprItem);
    
    QTableWidgetItem *YexprItem
      = new QTableWidgetItem (getPlotCurves ()[i]->Yexpression ());
    curvesTable->setItem (i, CURVES_COLUMN_Y_EXPRESSION, YexprItem);

    QString aspectString = getAspectString (i);
    QTableWidgetItem *aspectItem = new QTableWidgetItem (aspectString);
    Qt::ItemFlags aspectFlags = aspectItem->flags ();
    aspectFlags &= ~Qt::ItemIsEditable;
    aspectItem->setFlags (aspectFlags);
    curvesTable->setItem (i, CURVES_COLUMN_ASPECT, aspectItem);

    unsigned long int ls = getPlotCurves ()[i]->pen ()->style();
    QString lbl;
    if (ls < sizeof(styleStrings)/sizeof(char *))
      lbl = QString (styleStrings[ls]);
    QTableWidgetItem *penItem = new QTableWidgetItem (lbl);
    QBrush penBrush (getPlotCurves ()[i]->pen ()->color());
    penItem->setBackground (penBrush);
    curvesTable->setItem (i, CURVES_COLUMN_PEN, penItem);

    QString modeString = getModeString (i);
    QTableWidgetItem *modeItem = new QTableWidgetItem (modeString);
    Qt::ItemFlags modeFlags = modeItem->flags ();
    modeFlags &= ~Qt::ItemIsEditable;
    modeItem->setFlags (modeFlags);
    curvesTable->setItem (i, CURVES_COLUMN_MODE, modeItem);

    QTableWidgetItem *deleteItem = new QTableWidgetItem ("Delete");
    Qt::ItemFlags deleteFlags = deleteItem->flags ();
    deleteFlags &= ~Qt::ItemIsEditable;
    deleteItem->setFlags (deleteFlags);
    QBrush deleteBrush ("red");
    deleteItem->setBackground (deleteBrush);
    curvesTable->setItem (i, CURVES_COLUMN_DELETE, deleteItem);
  }
}

void Plot2DWindow::setBGImage ()
{
  QFileDialog dialog (this, "Background image", ".",
                      tr("Image Files (*.png *.jpg *.jpeg *.bmp *.gif)"));

  dialog.setOption (QFileDialog::DontUseNativeDialog);
  QLayout *layout = dialog.layout ();
  QGroupBox *gbox = new QGroupBox ();
  QGridLayout *btnlayout = new QGridLayout ();
  gbox->setLayout (btnlayout);

  QComboBox *aspectComboBox = new QComboBox ();
  connect (aspectComboBox, QOverload<int>::of(&QComboBox::activated),
          [=](int index __attribute__((unused)))
          {
	    QVariant aspectSel = aspectComboBox->currentData ();
	    setAspectMode ((Qt::AspectRatioMode)aspectSel.toInt ());
	     fprintf (stderr, "setting aspect\n");
	    drawCurves ();
	  });
  aspectComboBox->addItem ("Ignore Aspect Ratio",
			   QVariant(Qt::IgnoreAspectRatio));
  aspectComboBox->addItem ("Keep Aspect Ratio",
			   QVariant(Qt::KeepAspectRatio));
  aspectComboBox->addItem ("Keep Aspect Ratio By Expanding",
			   QVariant(Qt::KeepAspectRatioByExpanding));
  int initialIndex = aspectComboBox->findData (QVariant (getAspectMode ()));
  if (initialIndex != -1) aspectComboBox->setCurrentIndex (initialIndex);
  btnlayout->addWidget (aspectComboBox, 0, 0);

  QPushButton *clearImageButton
    = new QPushButton (QObject::tr ("Clear Image"));
  connect (clearImageButton, &QPushButton::clicked,
	   [=](){
	     getBGFile ().clear ();
	     fprintf (stderr, "clearing image\n");
	     drawCurves ();
	   });
  btnlayout->addWidget (clearImageButton, 0, 1);

  layout->addWidget (gbox);

  dialog.setWindowModality(Qt::WindowModal);
  dialog.setAcceptMode(QFileDialog::AcceptOpen);

  int drc = dialog.exec();

  if (drc == QDialog::Accepted) 
    setBGFile (dialog.selectedFiles().first());

}

void Plot2DWindow::setFonts ()
{
  QDialog dialog (this, Qt::Dialog);
  dialog.setWindowTitle ("qapl font controls");
  QGridLayout *layout = new QGridLayout;
  dialog.setLayout (layout);

  int row = 0;
  int col = 0;

  QPushButton *axisLabelFontButton =
    new QPushButton (QObject::tr ("Axis Label Font"));
  layout->addWidget (axisLabelFontButton, row, col++);
  QObject::connect (axisLabelFontButton, &QPushButton::clicked,
		    [=](){
		      bool ok;

		      QFont font = QFontDialog::getFont(&ok,
							getAxisLabelFont (),
							this,
							"Axis label font");
		      if (ok) {
			setAxisLabelFont (font);
			mw->getSettings ()->setValue (SETTINGS_AXIS_LABEL_FONT,
						      font.toString ());
			drawCurves ();
		      } 
		    });

  QPushButton *axisLabelColourButton =
    new QPushButton (QObject::tr ("Axis Label Colour"));
  layout->addWidget (axisLabelColourButton, row, col++);
  QObject::connect (axisLabelColourButton, &QPushButton::clicked,
		    [=](){
		      QColor colour
			= QColorDialog::getColor (getAxisLabelColour (),
						  this,
						  "Axis label colour",
					  QColorDialog::ShowAlphaChannel);
		      if (colour.isValid ()) {
			setAxisLabelColour (colour);
		mw->getSettings ()->setValue (SETTINGS_AXIS_LABEL_COLOUR,
		      getAxisLabelColour ().name (QColor::HexArgb));
		drawCurves ();
		      }
		      });

  row++;
  col = 0;


  QPushButton *axisTitleFontButton =
    new QPushButton (QObject::tr ("Axis Title Font"));
  layout->addWidget (axisTitleFontButton, row, col++);
  QObject::connect (axisTitleFontButton, &QPushButton::clicked,
		    [=](){
		      bool ok;

		      QFont font = QFontDialog::getFont(&ok,
							getAxisTitleFont (),
							this,
							"Axis title font");
		      if (ok) {
			setAxisTitleFont (font);
			mw->getSettings ()->setValue (SETTINGS_AXIS_TITLE_FONT,
						      font.toString ());
			drawCurves ();
		      } 
		    });

  QPushButton *axisTitleColourButton =
    new QPushButton (QObject::tr ("Axis Title Colour"));
  layout->addWidget (axisTitleColourButton, row, col++);
  QObject::connect (axisTitleColourButton, &QPushButton::clicked,
		    [=](){
		      QColor colour
			= QColorDialog::getColor (getAxisTitleColour (),
						  this,
						  "Axis title colour",
					  QColorDialog::ShowAlphaChannel);
		       if (colour.isValid ()) {
			 setAxisTitleColour (colour);
		      mw->getSettings ()->setValue (SETTINGS_AXIS_TITLE_COLOUR,
			    getAxisTitleColour ().name (QColor::HexArgb));
		      drawCurves ();
		       }
		    });

  
  row++;
  col = 0;

  QPushButton *chartTitleFontButton =
    new QPushButton (QObject::tr ("Chart Title Font"));
  layout->addWidget (chartTitleFontButton, row, col++);
  QObject::connect (chartTitleFontButton, &QPushButton::clicked,
		    [=](){
		      bool ok;

		      QFont font = QFontDialog::getFont(&ok,
							getChartTitleFont (),
							this,
							"Chart title font");
		      if (ok) {
			setChartTitleFont (font);
			mw->getSettings ()->setValue (SETTINGS_CHART_TITLE_FONT,
						      font.toString ());
			drawCurves ();
		      } 
		    });

  QPushButton *chartTitleColourButton =
    new QPushButton (QObject::tr ("Chart Title Colour"));
  layout->addWidget (chartTitleColourButton, row, col++);
  QObject::connect (chartTitleColourButton, &QPushButton::clicked,
		    [=](){
		      QColor colour
			= QColorDialog::getColor (getChartTitleColour (),
						  this,
						  "Chart title colour",
					  QColorDialog::ShowAlphaChannel);
		       if (colour.isValid ()) {
			 setChartTitleColour (colour);
		      mw->getSettings ()->setValue (SETTINGS_CHART_TITLE_COLOUR,
			    getChartTitleColour ().name (QColor::HexArgb));
		      drawCurves ();
		       }
		    });
  

  
  row++;
  col = 0;


  QPushButton *axisColourButton =
    new QPushButton (QObject::tr ("Axis Colour"));
  layout->addWidget (axisColourButton, row, 1);
  QObject::connect (axisColourButton, &QPushButton::clicked,
		    [=](){
		      QColor colour
			= QColorDialog::getColor (getAxisColour (),
						  this,
						  "Axis colour",
					  QColorDialog::ShowAlphaChannel);
		      if (colour.isValid ()) {
			setAxisColour (colour);
		      mw->getSettings ()->setValue (SETTINGS_AXIS_COLOUR,
			    getAxisColour ().name (QColor::HexArgb));
		      drawCurves ();
		      }
		    });

  row++;
  col = 0;


  QLabel themeLbl ("Theme");
  layout->addWidget (&themeLbl, row, 0, Qt::AlignRight);
  
  QComboBox *themebox = new QComboBox ();
  themebox->addItem ("Light", QChart::ChartThemeLight);
  themebox->addItem ("Blue Cerulean", QChart::ChartThemeBlueCerulean);
  themebox->addItem ("Dark", QChart::ChartThemeDark);
  themebox->addItem ("Brown Sand", QChart::ChartThemeBrownSand);
  themebox->addItem ("Blue Ncs", QChart::ChartThemeBlueNcs);
  themebox->addItem ("High Contrast", QChart::ChartThemeHighContrast);
  themebox->addItem ("Blue Icy", QChart::ChartThemeBlueIcy);
  themebox->addItem ("Qt", QChart::ChartThemeQt);
  themebox->setCurrentIndex ((int)getTheme ());
  connect (themebox, QOverload<int>::of(&QComboBox::activated),
          [=](int index)
          {
	    QVariant sel = themebox->itemData (index);
	    setTheme (sel.toInt ());
	    mw->getSettings ()->setValue (SETTINGS_PLOT_THEME, getTheme ());
	    drawCurves ();
	  });
  layout->addWidget(themebox, row, 1);

  row++;
  col = 0;


  QPushButton *chartBGImageButton =
    new QPushButton (QObject::tr ("Background Image"));
  layout->addWidget (chartBGImageButton, row, 1);
  QObject::connect (chartBGImageButton, &QPushButton::clicked,
		    [=](){
		      setBGImage ();
		      //drawCurves ();
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
  

void Plot2DWindow::setDecorations ()
{
  QDialog dialog (this, Qt::Dialog);
  dialog.setMinimumWidth (640);
  dialog.setWindowTitle ("qapl controls");
  QGridLayout *layout = new QGridLayout;
  dialog.setLayout (layout);

  int row = 0;
  int col = 0;
  
  QLabel chartTitileLbl ("Title");
  layout->addWidget (&chartTitileLbl, row, col++);

  GreekLineEdit *chartTitleBox = new GreekLineEdit ();
  chartTitleBox->setText (getChartTitle ());
  connect (chartTitleBox,
           &QLineEdit::editingFinished,
          [=](){
	    setChartTitle (chartTitleBox->text ());
	    drawCurves ();
          });
  chartTitleBox->setPlaceholderText ("ChartTitle");
  layout->addWidget (chartTitleBox, row, col++, 1, 2);

  row++;
  col = 0;
  
  QLabel xLbl ("Axes Labels");
  layout->addWidget (&xLbl, row, col++);

  GreekLineEdit *xTitleBox = new GreekLineEdit ();
  xTitleBox->setText (getXTitle ());
  connect (xTitleBox,
           &QLineEdit::editingFinished,
          [=](){
	    setXTitle (xTitleBox->text ());
	    drawCurves ();
          });
  xTitleBox->setPlaceholderText ("X label");
  layout->addWidget (xTitleBox, row, col++);

  GreekLineEdit *yTitleBox = new GreekLineEdit ();
  yTitleBox->setText (getYTitle ());
  connect (yTitleBox,
           &QLineEdit::editingFinished,
          [=](){
	    setYTitle (yTitleBox->text ());
	    drawCurves ();
          });
  yTitleBox->setPlaceholderText ("Y Label");
  layout->addWidget (yTitleBox, row, col++);

  row++;
  col = 0;

  QTableWidget *curvesTable = new QTableWidget (getPlotCurves ().size (),
						CURVES_COLUMN_LAST, this);
  connect (curvesTable,
	   QOverload<int, int>::of(&QTableWidget::cellDoubleClicked),
	   [=](int row, int column)
	   {
	     if (column == CURVES_COLUMN_DELETE) {
	       deleteStackEntry (row);
	       int rc = curvesTable->rowCount ();
	       curvesTable->setRowCount (0);
	       curvesTable->setRowCount (rc - 1);
	       fillTable (curvesTable);
	       drawCurves ();
	     }
	   });
  connect (curvesTable,
	   QOverload<int, int>::of(&QTableWidget::cellChanged),
	   [=](int row, int column)
	   {
	     if (column == CURVES_COLUMN_LABEL) {
	       QTableWidgetItem *labelItem = curvesTable->item (row, column);
	       QString newLabel = labelItem->text ();
	       getPlotCurves ()[row]->setTitle (newLabel);
	       drawCurves ();
	     }
	     else if (column == CURVES_COLUMN_X_EXPRESSION) {
	       QTableWidgetItem *XexpItem = curvesTable->item (row, column);
	       QString newExp = XexpItem->text ();
	       plot2DData->plotCurves[row]->setXExpression (newExp);
	       drawCurves ();
	     }
	     else if (column == CURVES_COLUMN_Y_EXPRESSION) {
	       QTableWidgetItem *YexpItem = curvesTable->item (row, column);
	       QString newExp = YexpItem->text ();
	       plot2DData->plotCurves[row]->setYExpression (newExp);
	       drawCurves ();
	     }
	   });
  connect (curvesTable,
	   QOverload<int, int>::of(&QTableWidget::cellClicked),
	   [=](int row, int column)
	   {
	     if (column == CURVES_COLUMN_ASPECT) {
	       updateAspect (plot2DData->plotCurves[row]);
	       QString aspectString = getAspectString (row);
	       QTableWidgetItem *aspectItem = curvesTable->item (row, column);
	       aspectItem->setText (aspectString);
	       drawCurves ();
	     }
	     else if (column == CURVES_COLUMN_MODE) {
	       updateMode (plot2DData->plotCurves[row]);
	       QString modeString = getModeString (row);
	       QTableWidgetItem *modeItem = curvesTable->item (row, column);
	       modeItem->setText (modeString);
	       drawCurves ();
	     }
	     else if (column == CURVES_COLUMN_PEN) {
	       QPen pen = *plot2DData->plotCurves[row]->pen ();
	       updatePen (&pen);
	       plot2DData->plotCurves[row]->setPen (pen);
	       QTableWidgetItem *penItem = curvesTable->item (row, column);
	       unsigned long int ls = (unsigned long int)pen.style();
	       QString lbl;
	       if (ls < sizeof(styleStrings)/sizeof(char *))
		 lbl = QString (styleStrings[ls]);
	       penItem->setText (lbl);
	       QBrush penBrush (pen.color());
	       penItem->setBackground (penBrush);
	       drawCurves ();
	     }
	   });

  QStringList headers = {
    "Label",
    "X Expression",
    "Y Expression",
    "Aspect",
    "Pen",
    "Mode",
    "Delete"
  };
  curvesTable->setHorizontalHeaderLabels (headers);

  fillTable (curvesTable);

  layout->addWidget (curvesTable, row, col++, 1, 4);

  row++;

  QPushButton *closeButton = new QPushButton (QObject::tr ("Close"));
  closeButton->setAutoDefault (true);
  closeButton->setDefault (true);
  layout->addWidget (closeButton, row, 3);
  QObject::connect (closeButton, &QPushButton::clicked,
                    &dialog, &QDialog::accept);


  dialog.exec ();
}

void
Plot2DWindow::setGranularity ()
{
  QDialog dialog (this, Qt::Dialog);
  QGridLayout *layout = new QGridLayout;
  dialog.setLayout (layout);

  int row = 0;

  QLabel *resolutionLbl = new QLabel (tr ("Resolution:"));
  layout->addWidget (resolutionLbl, row, 0);

  QSpinBox *resolutionBox = new QSpinBox ();
  resolutionBox->setRange (4, 128);
  resolutionBox->setValue (getResolution ());
  layout->addWidget (resolutionBox, row, 1);
  connect (resolutionBox,
           &QSpinBox::valueChanged,
          [=](){
	    setResolution (resolutionBox->value ());
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

void Plot2DWindow::exportAsChart ()
{
  QFileDialog dialog (this, "Export As...", ".",
		      tr("Plot Files (*.plot)"));

  dialog.setOption (QFileDialog::DontUseNativeDialog);
  dialog.setAcceptMode (QFileDialog::AcceptSave);

  int drc = dialog.exec();
  
  if (drc == QDialog::Accepted) {
    QString cf =  dialog.selectedFiles().first();
    dumpXML (cf);
  }
}

void Plot2DWindow::exportChart ()
{
  if (plot2DData->currentPlotFile.isEmpty ()) exportAsChart ();
  else dumpXML (plot2DData->currentPlotFile);
}

void Plot2DWindow::importChart ()
{
  QFileDialog dialog (this, "Export As...", ".",
		      tr("Plot Files (*.plot)"));

  dialog.setOption (QFileDialog::DontUseNativeDialog);
  dialog.setAcceptMode (QFileDialog::AcceptOpen);

   int drc = dialog.exec();
  
  if (drc == QDialog::Accepted) {
    QString cf = dialog.selectedFiles().first();
    readXML (cf, mw, false);
  }
}


void Plot2DWindow::createMenubar ()
{
  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
  
  QAction *importAct =
    fileMenu->addAction(tr("&Import"), this,
			    & Plot2DWindow::importChart);
  importAct->setStatusTip(tr("Import chart"));
  
  QAction *exportAsAct =
    fileMenu->addAction(tr("&Export As"), this,
			    & Plot2DWindow::exportAsChart);
  exportAsAct->setStatusTip(tr("Export chart to new file"));
  
  QAction *exportAct =
    fileMenu->addAction(tr("&Export"), this,
			    & Plot2DWindow::exportChart);
  exportAct->setStatusTip(tr("Export chart to current file"));

  QMenu *settingsMenu = menuBar()->addMenu(tr("&Settings"));

  QAction *resolutionAct =
    settingsMenu->addAction(tr("&Resolution"), this,
			    & Plot2DWindow::setGranularity);
  resolutionAct->setStatusTip(tr("Set resolution"));

  QAction *decorationsAct =
    settingsMenu->addAction(tr("&Controls"), this,
			    & Plot2DWindow::setDecorations);
  decorationsAct->setStatusTip(tr("Set plot decorations"));

  QAction *fontsAct =
    settingsMenu->addAction(tr("&Appearance"), this,
			    & Plot2DWindow::setFonts);
  {
    QFont font = fontsAct->font ();
    int pixelSize = font.pixelSize ();
    if (pixelSize != -1) {
      font.setPixelSize ((2 * pixelSize) / 3);
      fontsAct->setFont (font);
    }
    else {
      double pointSize = font.pointSizeF ();
      if (pointSize != -1.0) {
	font.setPointSizeF (pointSize * 0.667);
	fontsAct->setFont (font);
      }
    }
  }
  fontsAct->setStatusTip(tr("Set fonts"));

}

void
Plot2DWindow::closeEvent(QCloseEvent *event __attribute__((unused)))
{
  delete this;
}

//https://doc.qt.io/qt-5/qlineedit.html#contextMenuEvent


Plot2DWindow::Plot2DWindow (MainWindow *parent, Plot2dData *data)
  : QMainWindow(parent)
{
  this->setWindowTitle ("qapl 2D Plot Controls");
  mw = parent;

  plot2DData   = (data != nullptr) ? data : new Plot2dData (mw);
  chart 	= nullptr;
  chart2DWindow = new Chart2DWindow (this, mw);

  setAspectMode (Qt::KeepAspectRatio);

  setupComplete = false;
    
  QWidget *hw = new QWidget ();
  this->setCentralWidget(hw);
  QGridLayout *layout = new QGridLayout;
  
  createMenubar ();

  int row = 0;
  int col = 0;

  QLineEdit *aplXExpression = new QLineEdit ();
  aplXExpression->setPlaceholderText (tr ("APL X expression"));
  aplXExpression->setText (getAplXExpression ());
  connect (aplXExpression,
           &QLineEdit::editingFinished,
          [=](){
	    setAplXExpression (aplXExpression->text ());
	    if (setupComplete) drawCurves ();
          });
  layout->addWidget (aplXExpression, row, col, 1, 3);

  row++;
  col = 0;
  
  QLineEdit *aplYExpression = new QLineEdit ();
  aplYExpression->setPlaceholderText (tr ("APL Y expression"));
  aplYExpression->setText (getAplYExpression ());
  connect (aplYExpression,
           &QLineEdit::editingFinished,
          [=](){
	    setAplYExpression (aplYExpression->text ());
	    if (setupComplete) drawCurves ();
          });
  layout->addWidget (aplYExpression, row, col, 1, 3);

  row++;
  col = 0;

  GreekLineEdit *curveTitle = new GreekLineEdit ();
  curveTitle->setPlaceholderText (tr ("Curve label"));
  curveTitle->setText (plot2DData->activeCurve.title ());
  connect (curveTitle,
           &QLineEdit::editingFinished,
          [=](){
	    setCurveTitle (curveTitle->text ());
	    if (setupComplete) drawCurves ();
          });
  layout->addWidget (curveTitle, row, col++);


  QPushButton *setPenButton = new QPushButton (QObject::tr ("Pen"));
  {
    QColor bg = getPen ()->brush ().color ();
    QColor fg = QColor (bg);
    float hue;
    float saturation;
    float lightness;
    bg.getHslF (&hue, &saturation, &lightness);
    lightness = (lightness > 0.5) ? 0.0 : 1.0;
    //    saturation = 1.0;
    fg.setHslF (hue, saturation, lightness);
    QString cmd = QString("color: %1; background-color: %2;")
      .arg (fg.name(QColor::HexArgb)). arg (bg.name(QColor::HexArgb));
    setPenButton->setStyleSheet(cmd);
  }
  QObject::connect (setPenButton, &QPushButton::clicked,
		    [=](){
		      updatePen (getPen ());
		      drawCurves ();
		    });
  layout->addWidget (setPenButton, row, col++);
  
  QPushButton *pushExpr = new QPushButton (QObject::tr ("Push"));
  QObject::connect (pushExpr, &QPushButton::clicked,
		    [=](){
		      pushExpression ();
		      drawCurves ();
		    });
  layout->addWidget (pushExpr, row, col++);

  row++;
  col = 0;
  
  QLineEdit *indexVariable = new QLineEdit ();
  indexVariable->setPlaceholderText ("Index var");
  indexVariable->setMaximumWidth (100);
  indexVariable->setText (getIndexVariable ());
  connect (indexVariable,
           &QLineEdit::editingFinished,
          [=](){
	    fprintf (stderr, "idx = %s\n",
		     toCString (indexVariable->text ()));
	    setIndexVariable (indexVariable->text ());
	    if (setupComplete) drawCurves ();
          });
  layout->addWidget (indexVariable, row, col++, 1, 2);
  
  rangeInit = new ComplexSpinBox ();
  rangeInit->setComplex (getRealInit (), getImagInit ());
  connect (rangeInit,
           &ComplexSpinBox::valueChanged,
          [=](){
	    plot2DData->rangeInit = rangeInit->getComplex ();
	    if (setupComplete) drawCurves ();
          });
  layout->addWidget (rangeInit, row, col++);
  
  rangeFinal = new ComplexSpinBox ();
  rangeFinal->setComplex (getRealFinal (), getImagFinal ());
  connect (rangeFinal,
           &ComplexSpinBox::valueChanged,
          [=](){
	    plot2DData->rangeFinal = rangeFinal->getComplex ();
	    if (setupComplete) drawCurves ();
          });
  layout->addWidget (rangeFinal, row, col++);



  row++;
  col = 0;

  aspectCombo = new QComboBox ();
  aspectCombo->addItem (STRING_REAL,		QVariant(ASPECT_REAL));
  aspectCombo->addItem (STRING_IMAGINARY,	QVariant(ASPECT_IMAG));
  aspectCombo->addItem (STRING_MAGNITUDE,	QVariant(ASPECT_MAGNITUDE));
  aspectCombo->addItem (STRING_PHASE,		QVariant(ASPECT_PHASE));
  int ac = (int)(plot2DData->activeCurve.aspect ());
  aspectCombo->setCurrentIndex (ac);
  connect (aspectCombo,
	   QOverload<int>::of(&QComboBox::activated),
          [=](int index)
          {
	    plot2DData->activeCurve.setAspect ((aspect_e)index);
	    drawCurves ();
	  });

  layout->addWidget (aspectCombo, row, col++);
  
  

  modeCombo = new QComboBox ();
  modeCombo->addItem (STRING_SPLINES,	QVariant(MODE_BUTTON_SPLINE));
  modeCombo->addItem (STRING_LINES,	QVariant(MODE_BUTTON_LINE));
  modeCombo->addItem (STRING_SCATTER,	QVariant(MODE_BUTTON_SCATTER));
  int ap =  modeCombo->findData (QVariant (getMode ())); 
  modeCombo->setCurrentIndex (ap);
  connect (modeCombo,
	   QOverload<int>::of(&QComboBox::activated),
          [=](int index)
          {
	    QVariant sel = modeCombo->itemData (index);
	    plot2DData->activeCurve.setMode ((series_mode_e)sel.toInt ());
	    drawCurves ();
	  });

  layout->addWidget (modeCombo, row, col++);


  
  QCheckBox *polarCheck = new QCheckBox (tr ("Polar"));
  polarCheck->setCheckState (plot2DData->doPolar ? Qt::Checked : Qt::Unchecked);
  connect (polarCheck,
           &QCheckBox::stateChanged,
          [=](int index)
          {
	    plot2DData->doPolar = (index == Qt::Checked) ? true : false;
	    drawCurves ();
	  });
  layout->addWidget (polarCheck, row, col++);
  
  setupComplete = true;

  hw->setLayout(layout);
  this->show ();
}

Plot2DWindow::~Plot2DWindow ()
{
}

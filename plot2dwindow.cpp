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
#include "chart2dwindow.h"

#ifdef OLD_CODE
#define PLOTVAR "plotvarλ"
#define IDXVAR  "idxvarλ"
#endif

void Plot2DWindow::drawCurves ()
{
  chart2DWindow->drawCurves ();
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
Plot2DWindow::setPen ()
{
  QDialog dialog (this, Qt::Dialog);
  QGridLayout *layout = new QGridLayout;
  dialog.setLayout (layout);

  int row = 0;
  int col = 0;
  
  QPushButton *setColourButton = new QPushButton (QObject::tr ("Pen colour"));
  QObject::connect (setColourButton, &QPushButton::clicked,
		    [=](){
		      QColor colour = QColorDialog::getColor ();
		    });
  layout->addWidget (setColourButton, row, col++);


  dialog.exec ();
}


void
Plot2DWindow::setDecorations ()
{
  QDialog dialog (this, Qt::Dialog);
  QGridLayout *layout = new QGridLayout;
  dialog.setLayout (layout);

  int row = 0;
  int col = 0;
  
  QLabel chartTitileLbl ("Title");
  layout->addWidget (&chartTitileLbl, row, col++);

  QLineEdit *chartTitleBox = new QLineEdit ();
  chartTitleBox->setText (chartTitle);
  connect (chartTitleBox,
           &QLineEdit::editingFinished,
          [=](){
	    chartTitle = chartTitleBox->text ();
	    drawCurves ();
          });
  chartTitleBox->setPlaceholderText ("ChartTitle");
  layout->addWidget (chartTitleBox, row, col++, 1, 2);

  row++;
  col = 0;
  
  QLabel xLbl ("Axes Labels");
  layout->addWidget (&xLbl, row, col++);

  QLineEdit *xTitleBox = new QLineEdit ();
  xTitleBox->setText (xTitle);
  connect (xTitleBox,
           &QLineEdit::editingFinished,
          [=](){
	    xTitle = xTitleBox->text ();
	    drawCurves ();
          });
  xTitleBox->setPlaceholderText ("X label");
  layout->addWidget (xTitleBox, row, col++);

  QLineEdit *yTitleBox = new QLineEdit ();
  yTitleBox->setText (yTitle);
  connect (yTitleBox,
           &QLineEdit::editingFinished,
          [=](){
	    yTitle = yTitleBox->text ();
	    drawCurves ();
          });
  yTitleBox->setPlaceholderText ("Y Label");
  layout->addWidget (yTitleBox, row, col++);

  row++;
  col = 0;
#if 0
  QLabel curvesLbl ("Axes Labels");
  layout->addWidget (&curvesLbl, row, col++);
#endif

  enum {
    CURVES_COLUMN_LABEL,
    CURVES_COLUMN_EXPRESSION,
    CURVES_COLUMN_ASPECT,
    CURVES_COLUMN_PEN
  };

  QTableWidget *curvesTable = new QTableWidget (plotCurves.size (), 4, this);
  connect (curvesTable,
	   QOverload<int, int>::of(&QTableWidget::cellChanged),
	   [=](int row, int column)
	   {
	     if (column == CURVES_COLUMN_LABEL) {
	       QTableWidgetItem *labelItem = curvesTable->item (row, column);
	       QString newLabel = labelItem->text ();
	       plotCurves[row]->setLabel (newLabel);
	       drawCurves ();
	     }
	     else if (column == CURVES_COLUMN_EXPRESSION) {
	       QTableWidgetItem *expItem = curvesTable->item (row, column);
	       QString newExp = expItem->text ();
	       plotCurves[row]->setExpression (newExp);
	       drawCurves ();
	     }
	   });
  connect (curvesTable,
	   QOverload<int, int>::of(&QTableWidget::cellDoubleClicked),
	   [=](int row, int column)
	   {
	     if (column == CURVES_COLUMN_ASPECT) {
	       fprintf (stderr, "dc%d\n", row);
	     }
	   });

  QStringList headers = {"Label", "Expression", "Aspect", "Pen"};
  curvesTable->setHorizontalHeaderLabels (headers);
  
  for (int i = 0; i < plotCurves.size (); i++) {
    QString labelString = plotCurves[i]->label ().isEmpty () ?
      QString ("---") : plotCurves[i]->label ();
    QTableWidgetItem *labelItem = new QTableWidgetItem (labelString);
    curvesTable->setItem (i, CURVES_COLUMN_LABEL, labelItem);
    
    QTableWidgetItem *exprItem
      = new QTableWidgetItem (plotCurves[i]->expression ());
    curvesTable->setItem (i, CURVES_COLUMN_EXPRESSION, exprItem);

    QString aspectString;
    switch(plotCurves[i]->aspect ()) {
    case ASPECT_REAL:
      aspectString = QString ("Real");
      break;
    case ASPECT_IMAG:
      aspectString = QString ("Imaginary");
      break;
    case ASPECT_MAGNITUDE:
      aspectString = QString ("Magnitude");
      break;
    case ASPECT_PHASE:
      aspectString = QString ("Phase");
      break;
    }
    QTableWidgetItem *aspectItem = new QTableWidgetItem (aspectString);
    Qt::ItemFlags aspectFlags = aspectItem->flags ();
    aspectFlags &= ~Qt::ItemIsEditable;
    aspectItem->setFlags (aspectFlags);
    curvesTable->setItem (i, CURVES_COLUMN_ASPECT, aspectItem);
  }

  layout->addWidget (curvesTable, row, col++, 1, 4);

  row++;
  col = 0;

  QLabel themeLbl ("Theme");
  layout->addWidget (&themeLbl, row, 0);
  
  QComboBox *themebox = new QComboBox ();
  themebox->addItem ("Light", QChart::ChartThemeLight);
  themebox->addItem ("Blue Cerulean", QChart::ChartThemeBlueCerulean);
  themebox->addItem ("Dark", QChart::ChartThemeDark);
  themebox->addItem ("Brown Sand", QChart::ChartThemeBrownSand);
  themebox->addItem ("Blue Ncs", QChart::ChartThemeBlueNcs);
  themebox->addItem ("High Contrast", QChart::ChartThemeHighContrast);
  themebox->addItem ("Blue Icy", QChart::ChartThemeBlueIcy);
  themebox->addItem ("Qt", QChart::ChartThemeQt);
  themebox->setCurrentIndex ((int)theme);
  connect (themebox, QOverload<int>::of(&QComboBox::activated),
          [=](int index)
          {
	    QVariant sel = themebox->itemData (index);
	    theme = sel.toInt ();
	    drawCurves ();
	  });
  layout->addWidget(themebox, row, 1);

  row++;

  QPushButton *closeButton = new QPushButton (QObject::tr ("Close"));
  closeButton->setAutoDefault (true);
  closeButton->setDefault (true);
  layout->addWidget (closeButton, row, 1);
  QObject::connect (closeButton, &QPushButton::clicked,
                    &dialog, &QDialog::accept);


  dialog.exec ();
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
  resolutionBox->setValue (resolution);
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

  QAction *decorationsAct =
    settingsMenu->addAction(tr("&Decor"), this,
			    & Plot2DWindow::setDecorations);
  decorationsAct->setStatusTip(tr("Set plot decorations"));

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

  chart2DWindow = new Chart2DWindow (this, mw);

  setupComplete = false;
    
  QWidget *hw = new QWidget ();
  this->setCentralWidget(hw);
  QGridLayout *layout = new QGridLayout;
  
  createMenubar ();

  int row = 0;
  int col = 0;
  
  aplExpression = new QLineEdit ();
  aplExpression->setPlaceholderText (tr ("APL expression"));
  connect (aplExpression,
           &QLineEdit::returnPressed,
          [=](){
	    if (setupComplete) drawCurves ();
          });
  layout->addWidget (aplExpression, row, col, 1, 3);

  row++;
  col = 0;

  curveTitle = new QLineEdit ();
  curveTitle->setPlaceholderText (tr ("Curve title"));
  connect (curveTitle,
           &QLineEdit::returnPressed,
          [=](){
	    if (setupComplete) drawCurves ();
          });
  layout->addWidget (curveTitle, row, col++);
  
  QPushButton *setPenButton = new QPushButton (QObject::tr ("Pen"));
  QObject::connect (setPenButton, &QPushButton::clicked,
		    [=](){
		      setPen ();
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
    
  setupComplete = true;

  hw->setLayout(layout);
  this->show ();
}

Plot2DWindow::~Plot2DWindow ()
{
}

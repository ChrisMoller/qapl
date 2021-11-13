#ifndef PLOT2DWINDOW_H
#define PLOT2DWINDOW_H

#include <QtWidgets>
#include <QAbstractSeries>
#include <QChart>
#include <QChartView>

#include "mainwindow.h"
#include "chart2dwindow.h"
#include "complexspinbox.h"
#include "scidoublespinbox.h"
#include "plot2ddata.h"

class Chart2DWindow;

#include "enums.h"

class MainWindow;
class Plot2DWindow;

class Plot2DWindow : public QMainWindow
{
  Q_OBJECT

public:
  Plot2DWindow (MainWindow *parent, Plot2dData *data);
  ~Plot2DWindow ();
  QString  getAplXExpression () {
    return plot2DData->activeCurve.Xexpression ();
  }
  void     setAplXExpression (QString expression) {
    plot2DData->activeCurve.setXExpression (expression);
  }
  QString  getAplYExpression () {
    return plot2DData->activeCurve.Yexpression ();
  }
  void     setAplYExpression (QString expression) {
    plot2DData->activeCurve.setYExpression (expression);
  }
  double getMarkerSize () { return plot2DData->activeCurve.markerSize (); }
  void   setMarkerSize (double ms) {
    plot2DData->activeCurve.setMarkerSize (ms);
  }
  ComplexSpinBox *getRangeInit ()  { return rangeInit; }
  ComplexSpinBox *getRangeFinal ()  { return rangeFinal; }
  double   getRealInit ()  { return plot2DData->rangeInit.real (); }
  double   getRealFinal () { return plot2DData->rangeFinal.real (); }
  double   getImagInit ()  { return plot2DData->rangeFinal.imag (); }
  double   getImagFinal () { return plot2DData->rangeInit.imag (); }
  QString  getBGFile () { return plot2DData->bgFile; }
  void     setBGFile (QString file) { plot2DData->bgFile = file; }
  Qt::AspectRatioMode getAspectMode () { return plot2DData->aspectMode; }
  void	   setAspectMode (Qt::AspectRatioMode mode) {
    plot2DData->aspectMode = mode;
  }
  int      getResolution () {return plot2DData->resolution; }
  void     setResolution (int res) {plot2DData->resolution = res; }
  QFont	   getAxisLabelFont () { return plot2DData->axisLabelFont; }
  void     setAxisLabelFont (QFont font) { plot2DData->axisLabelFont = font; }
  QColor   getAxisLabelColour () { return plot2DData->axisLabelColour; }
  void     setAxisLabelColour (QColor colour) {
    plot2DData->axisLabelColour = colour;
  }
  QFont    getAxisTitleFont () { return plot2DData->axisTitleFont; }
  void     setAxisTitleFont (QFont font) { plot2DData->axisTitleFont = font; }
  QColor   getAxisTitleColour () { return plot2DData->axisTitleColour; }
  void     setAxisTitleColour (QColor colour) {
    plot2DData->axisTitleColour = colour;
  }
  QColor   getAxisColour () { return plot2DData-> axisColour; }
  void     setAxisColour (QColor colour) { plot2DData->axisColour = colour; }
  QFont    getChartTitleFont () { return plot2DData->chartTitleFont; }
  void     setChartTitleFont (QFont font) { plot2DData->chartTitleFont = font; }
  QColor   getChartTitleColour () { return plot2DData->chartTitleColour; }
  void     setChartTitleColour (QColor colour) {
    plot2DData->chartTitleColour = colour;
  }
  QString  getIndexVariable () { return plot2DData->indexVariable; }
  void     setIndexVariable (QString iv) { plot2DData->indexVariable = iv; }
  QString  getXTitle () { return plot2DData->xTitle; }
  void     setXTitle (QString xtitle) { plot2DData->xTitle = xtitle; }
  QString  getYTitle () { return plot2DData->yTitle; }
  void     setYTitle (QString ytitle) { plot2DData->yTitle = ytitle; }
  QString  getChartTitle () { return plot2DData->chartTitle; }
  void     setChartTitle (QString title) { plot2DData->chartTitle = title; }
  QPen    *getPen () { return plot2DData->activeCurve.pen (); }
  void     setPen (QPen pen) { plot2DData->activeCurve.setPen (pen); }
  bool     getPolar () { return plot2DData->doPolar; }
  void     setPolar (bool polar) { plot2DData->doPolar = polar; }
  QString  getCurveTitle () { return plot2DData->activeCurve.title (); }
  void     setYmin (double v) {yMin = v;}
  void     setYmax (double v) {yMax = v;}
  double   getYmin () { return yMin; }
  double   getYmax () { return yMax; }
  SciDoubleSpinBox *getYminBox () { return minBox; }
  SciDoubleSpinBox *getYmaxBox () { return maxBox; }
  bool     getyMinLocked () { return yMinLocked; }
  bool     getyMaxLocked () { return yMaxLocked; }
  void     setCurveTitle (QString title) {
    plot2DData->activeCurve.setTitle (title);
  }
  QChart::ChartTheme getTheme () {
    return (QChart::ChartTheme)plot2DData->theme;
  }
  void setTheme (int theme) { plot2DData->theme = theme; }
  QList<PlotCurve *> getPlotCurves () { return plot2DData->plotCurves; }
  QList<PlotParameter *> getPlotParameters ()
  {
    return plot2DData->plotParameters;
  }
#if 1
  aspect_e  getAspect () { return plot2DData->activeCurve.aspect (); }
  series_mode_e  getMode () { return plot2DData->activeCurve.mode (); }
#else
  aspect_e getAspect () {
    QVariant sel = aspectCombo->currentData ();
    return (aspect_e)sel.toInt ();
  }
  series_mode_e getMode () {
    QVariant sel = modeCombo->currentData ();
    fprintf (stderr, "getting mode %d\n", sel.toInt ());
    return (series_mode_e)sel.toInt ();
  }
#endif

  void showPlot2dData (Plot2dData *data);
  void drawCurves ();

  static void showError (QXmlStreamReader &stream);
  static void readXML (QString &fileName, MainWindow *mw, bool trace);
  static int parseQapl (QXmlStreamReader &stream, Plot2dData *plot2DData,
			 bool trace);
  static int parseChart (QXmlStreamReader &stream, Plot2dData *plot2DData,
			  bool trace);
  static int parseAxes (QXmlStreamReader &stream, Plot2dData *plot2DData,
			 bool trace);
  static int parseAxesLabel (QXmlStreamReader &stream, Plot2dData *plot2DData,
			      bool trace);
  static int parseAxesTitle (QXmlStreamReader &stream, Plot2dData *plot2DData,
			      bool trace);
  static int parseRange (QXmlStreamReader &stream, Plot2dData *plot2DData,
			  bool trace);
  static int parseActive (QXmlStreamReader &stream, Plot2dData *plot2DData,
			   bool trace);
  static int parseStack (QXmlStreamReader &stream, Plot2dData *plot2DData,
			  bool trace);
  static int parsePen (QXmlStreamReader &stream, Plot2dData *plot2DData,
			bool trace);
  static int parseStackPen (QXmlStreamReader &stream, PlotCurve *plotCurve,
			     bool trace, int index);
  static int parseParameter (QXmlStreamReader &stream, Plot2dData *plot2DData,
			     bool trace);

private:
  MainWindow *mw;
  void createMenubar ();
  void setGranularity ();
  void setControls ();
  void setParameters ();
  void pushExpression ();
  void updatePen (QPen *pen);
  void updatParmValue (int idx, QTableWidget *parametersTable);
  void updateAspect (PlotCurve *pc);
  void updateMode (PlotCurve *pc);
  QString getAspectString (int idx);
  QString getModeString (int idx);
  void deleteStackEntry (int row);
  void deleteParameterEntry (int row);
  void fillTable (QTableWidget *curvesTable);
  void fillParametersTable (QTableWidget *parametersTable);
  void setFonts ();
  void exportChart ();
  void exportAsChart ();
  void importChart ();
  void dumpXML (QString fileName);
  void setBGImage ();
  double yMin;
  double yMax;
  bool yMinLocked;
  bool yMaxLocked;

  bool setupComplete;
  QChartView *chartView;
  QChart *chart;
  QComboBox *modeCombo;
  QComboBox *aspectCombo;
  Chart2DWindow *chart2DWindow;
  ComplexSpinBox *rangeInit;
  ComplexSpinBox *rangeFinal;
  Plot2dData *plot2DData;
  SciDoubleSpinBox *minBox;
  SciDoubleSpinBox *maxBox;
  
protected:
  void closeEvent(QCloseEvent *event) override;
};

#endif // PLOT2DWINDOW_H

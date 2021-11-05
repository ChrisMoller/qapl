#ifndef PLOT2DWINDOW_H
#define PLOT2DWINDOW_H

#include <QtWidgets>
#include <QAbstractSeries>
#include <QChart>
#include <QChartView>

#include "mainwindow.h"
#include "chart2dwindow.h"
#include "complexspinbox.h"
#include "plot2ddata.h"

class Chart2DWindow;

#include "enums.h"

class MainWindow;

class Plot2DWindow : public QMainWindow
{
  Q_OBJECT

public:
  Plot2DWindow (MainWindow *parent, Plot2dData *data);
  ~Plot2DWindow ();
  QString  getAplExpression () { return plot2DData->activeCurve.expression (); }
  void     setAplExpression (QString expression) {
    plot2DData->activeCurve.setExpression (expression);
  }
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
  QString  getCurveTitle () { return plot2DData->activeCurve.title (); }
  void     setCurveTitle (QString title) {
    plot2DData->activeCurve.setTitle (title);
  }
  QChart::ChartTheme getTheme () {
    return (QChart::ChartTheme)plot2DData->theme;
  }
  void setTheme (int theme) { plot2DData->theme = theme; }
  QList<PlotCurve *> getPlotCurves () { return plot2DData->plotCurves; }
  aspect_e getAspect () {
    QVariant sel = aspectCombo->currentData ();
    return (aspect_e)sel.toInt ();
  }
  series_mode_e getMode () {
    QVariant sel = modeCombo->currentData ();
    return (series_mode_e)sel.toInt ();
  }

  void showPlot2dData (Plot2dData *data);
  
  static void readXML (QString &fileName, MainWindow *mw);
  static bool parseQapl (QXmlStreamReader &stream, Plot2dData *plot2DData);
  static bool parseChart (QXmlStreamReader &stream, Plot2dData *plot2DData);
  static bool parseAxes (QXmlStreamReader &stream, Plot2dData *plot2DData);
  static bool parseAxesLabel (QXmlStreamReader &stream, Plot2dData *plot2DData);
  static bool parseAxesTitle (QXmlStreamReader &stream, Plot2dData *plot2DData);
  static bool parseRange (QXmlStreamReader &stream, Plot2dData *plot2DData);
  static bool parseActive (QXmlStreamReader &stream, Plot2dData *plot2DData);
  static bool parsePen (QXmlStreamReader &stream, Plot2dData *plot2DData);
  QString currentPlotFile;

private:
  MainWindow *mw;
  void drawCurves ();
  void createMenubar ();
  void setGranularity ();
  void setDecorations ();
  void pushExpression ();
  void updatePen (QPen *pen);
  void updateAspect (PlotCurve *pc);
  void updateMode (PlotCurve *pc);
  QString getAspectString (int idx);
  QString getModeString (int idx);
  void deleteStackEntry (int row);
  void fillTable (QTableWidget *curvesTable);
  void setFonts ();
  void exportChart ();
  void importChart ();
  void dumpXML (QString fileName);
  void setBGImage ();

  bool setupComplete;
  QChartView *chartView;
  QChart *chart;
  QComboBox *modeCombo;
  QComboBox *aspectCombo;
  Chart2DWindow *chart2DWindow;
  ComplexSpinBox *rangeInit;
  ComplexSpinBox *rangeFinal;
  Plot2dData *plot2DData;

  // QLineEdit *aplExpression;
  // QLineEdit *indexVariable;
  // QLineEdit *curveTitle;
  // int resolution;
  // double realInit;
  // double realFinal;
  // double imagInit;
  // double imagFinal;
  // QList<PlotCurve *> plotCurves;
  // QString chartTitle;
  // QString xTitle;
  // QString yTitle;
  // int theme;
  // QPen activePen;
  // QFont  axisLabelFont;
  // QColor axisLabelColour;
  // QFont  axisTitleFont;
  // QColor axisTitleColour;
  // QColor axisColour;
  // QFont  chartTitleFont;
  // QColor chartTitleColour;
  
protected:
  void closeEvent(QCloseEvent *event) override;
};

#endif // PLOT2DWINDOW_H

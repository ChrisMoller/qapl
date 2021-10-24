#ifndef PLOT2DWINDOW_H
#define PLOT2DWINDOW_H

#include <QtWidgets>
#include <QAbstractSeries>
#include <QChart>
#include <QChartView>

#include "mainwindow.h"
#include "chart2dwindow.h"
#include "complexspinbox.h"

class Chart2DWindow;

#include "enums.h"

class MainWindow;

class PlotCurve {
public:
  PlotCurve (QString &e,  aspect_e &a, QString l, QPen p, series_mode_e m) {
    a_expression = e;
    a_aspect = a;
    a_pen = p;
    a_label = l;
    a_mode = m;
  }
  QString label () { return a_label; }
  void setLabel (QString label) { a_label = label; }
  QString expression () { return a_expression; }
  QPen pen () { return a_pen; }
  void setPen (QPen pen) { a_pen = pen; }
  void setExpression (QString expression) { a_expression = expression; }
  aspect_e aspect () { return a_aspect; }
  void setAspect (aspect_e aspect) { a_aspect = aspect; }
  series_mode_e mode () { return a_mode; }
  void setMode (series_mode_e mode) { a_mode = mode; }
  
private:
  QString	a_label;
  QString	a_expression;
  aspect_e	a_aspect;
  QPen		a_pen;
  series_mode_e a_mode;
};

class Plot2DWindow : public QMainWindow
{
  Q_OBJECT

public:
  Plot2DWindow (MainWindow *parent = nullptr);
  ~Plot2DWindow ();
  QString  getAplExpression () { return aplExpression->text (); }
  aspect_e getAspect () {
    QVariant sel = aspectCombo->currentData ();
    return (aspect_e)sel.toInt ();
  }
  series_mode_e getMode () {
    QVariant sel = modeCombo->currentData ();
    return (series_mode_e)sel.toInt ();
  }
  double   getRealFinal () { return realFinal; }
  double   getRealInit  () { return realInit; }
  double   getImagFinal () { return imagFinal; }
  double   getImagInit  () { return imagInit; }
  int      getResolution () {return resolution; }
  QString  getIndexVariable () { return indexVariable->text (); }
  QString  getXTitle () { return xTitle; }
  QString  getYTitle () { return yTitle; }
  QString  getChartTitle () { return chartTitle; }
  QPen     getPen () { return activePen; }
  QString  getCurveTitle () { return curveTitle->text (); }
  QChart::ChartTheme getTheme () { return (QChart::ChartTheme)theme; }
  QList<PlotCurve *> getPlotCurves () { return plotCurves; }
  QFont  getAxisLabelFont () { return axisLabelFont; }
  QColor getAxisLabelColour () { return axisLabelColour; }
  QFont  getAxisTitleFont () { return axisTitleFont; }
  QColor getAxisTitleColour () { return axisTitleColour; }
  QColor getAxisColour () { return axisColour; }
  QFont  getChartTitleFont () { return chartTitleFont; }
  QColor getChartTitleColour () { return chartTitleColour; }

private:
  MainWindow *mw;
  void drawCurves ();
  void createMenubar ();
  void setResolution ();
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
  void dumpXML (QString fileName);

  QLineEdit *aplExpression;
  QLineEdit *indexVariable;
  QLineEdit *curveTitle;
  int resolution;
  double realInit;
  double realFinal;
  double imagInit;
  double imagFinal;
  bool setupComplete;
  QChartView *chartView;
  QChart *chart;
  QComboBox *modeCombo;
  QComboBox *aspectCombo;
  QList<PlotCurve *> plotCurves;
  QString chartTitle;
  QString xTitle;
  QString yTitle;
  QString currentFile;
  int theme;
  Chart2DWindow *chart2DWindow;
  QPen activePen;
  QFont  axisLabelFont;
  QColor axisLabelColour;
  QFont  axisTitleFont;
  QColor axisTitleColour;
  QColor axisColour;
  QFont  chartTitleFont;
  QColor chartTitleColour;
  
protected:
  void closeEvent(QCloseEvent *event) override;
};

#endif // PLOT2DWINDOW_H

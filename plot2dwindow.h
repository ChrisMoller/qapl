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
  PlotCurve (QString &e,  aspect_e &a, QPen p) {
    a_expression = e;
    a_aspect = a;
    a_pen = p;
  }
  QString label () { return a_label; }
  void setLabel (QString label) { a_label = label; }
  QString expression () { return a_expression; }
  QPen pen () { return a_pen; }
  void setPen (QPen pen) { a_pen = pen; }
  void setExpression (QString expression) { a_expression = expression; }
  aspect_e aspect () { return a_aspect; }
  void setAspect (aspect_e aspect) { a_aspect = aspect; }
  
private:
  QString	a_label;
  QString	a_expression;
  aspect_e	a_aspect;
  QPen		a_pen;
};

class Plot2DWindow : public QMainWindow
{
  Q_OBJECT

public:
  Plot2DWindow (MainWindow *parent = nullptr);
  ~Plot2DWindow ();
  QString  getAplExpression () { return aplExpression->text (); }
  aspect_e getAspect () {return (aspect_e) aspectGroup->checkedId (); }
  series_mode_e getMode () {return (series_mode_e) modeGroup->checkedId (); }
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

private:
  MainWindow *mw;
  void drawCurves ();
  void createMenubar ();
  void setResolution ();
  void setDecorations ();
  void pushExpression ();
  void updatePen (QPen *pen);

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
  QButtonGroup *modeGroup;
  QButtonGroup *aspectGroup;
  QList<PlotCurve *> plotCurves;
  QString chartTitle;
  QString xTitle;
  QString yTitle;
  int theme;
  Chart2DWindow *chart2DWindow;
  QPen activePen;
  
protected:
  void closeEvent(QCloseEvent *event) override;
};

#endif // PLOT2DWINDOW_H

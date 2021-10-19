#ifndef PLOT2DWINDOW_H
#define PLOT2DWINDOW_H

#include <QtWidgets>
#include <QAbstractSeries>
#include <QChart>
#include <QChartView>

#include "mainwindow.h"
#include "complexspinbox.h"

typedef enum {
  MODE_BUTTON_UNSET,
  MODE_BUTTON_SPLINE,
  MODE_BUTTON_LINE,
  MODE_BUTTON_POLAR,
  MODE_BUTTON_PIE,
  MODE_BUTTON_SCATTER,
  MODE_BUTTON_AREA,
  MODE_BUTTON_BOX
} series_mode_e;

typedef enum {
  ASPECT_REAL,
  ASPECT_IMAG,
  ASPECT_MAGNITUDE,
  ASPECT_PHASE
} aspect_e;

class MainWindow;

class PlotCurve {
public:
  PlotCurve (QString &e,  aspect_e &a) { a_expression = e; a_aspect = a; }
  QString label () { return a_label; }
  void setLabel (QString label) { a_label = label; }
  QString expression () { return a_expression; }
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

private:
  MainWindow *mw;
#if 0
  QLineEdit *indexVarName;
#endif
  void drawCurves ();
  void drawCurve (QString aplExpr, aspect_e aspect);
  void createMenubar ();
  void setResolution ();
  void setDecorations ();
  bool appendSeries (double x, double y,
		     double &realMax, double &realMin);
  void pushExpression ();

  QLineEdit *aplExpression;
  QLineEdit *indexVariable;
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
  QAbstractSeries *series;
  series_mode_e seriesMode;
  QList<PlotCurve *> plotCurves;
  QString chartTitle;
  QString xTitle;
  QString yTitle;
  int theme;
  
protected:
  void closeEvent(QCloseEvent *event) override;
};

#endif // PLOT2DWINDOW_H

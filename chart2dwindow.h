#ifndef CHART2DWINDOW_H
#define CHART2DWINDOW_H

#include <QtWidgets>
#include <QAbstractSeries>
#include <QChart>
#include <QChartView>
#include <QSplineSeries>

#include "mainwindow.h"
#include "plot2dwindow.h"
#include "chart2dwindow.h"

#include "enums.h"

class Plot2DWindow;

class Chart2DWindow : public QMainWindow
{
  Q_OBJECT

public:
  Chart2DWindow (Plot2DWindow *parent, MainWindow *mainWin);
  ~Chart2DWindow ();
  void drawCurves ();
  void drawCurve (QString aplExpr, aspect_e aspect, QPen pen);
  
 private:

  Plot2DWindow *pw;
  MainWindow *mw;
  void createMenubar ();
  QChartView *chartView;
  QChart *chart;
  QAbstractSeries *series;
  series_mode_e seriesMode;
  bool appendSeries (double x, double y,
		     double &realMax, double &realMin);

};

#endif // CHART2DWINDOW_H
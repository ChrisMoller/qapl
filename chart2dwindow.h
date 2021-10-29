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
  void drawCurve (QString aplExpr, aspect_e aspect,
		  QString label, QPen pen, series_mode_e mode,
		  double &realMax, double &realMin,
		  std::vector<double> idxVector);
  void exportImage ();
  
 private:

  Plot2DWindow *pw;
  MainWindow *mw;
  void createMenubar ();
#if 0
  void setAxesFont (QAbstractAxis *axis);
#endif
  QChartView *chartView;
  QChart *chart;
  QAbstractSeries *series;
  bool appendSeries (double x, double y, series_mode_e mode,
		     double &realMax, double &realMin);
  QString currentFile;
  bool showPreview (QPixmap plotPixmap);
  double fontScale;

};

#endif // CHART2DWINDOW_H

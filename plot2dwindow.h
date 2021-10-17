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

enum {
  ASPECT_REAL,
  ASPECT_IMAG,
  ASPECT_MAGNITUDE,
  ASPECT_PHASE
};

class MainWindow;

class Plot2DWindow : public QMainWindow
{
  Q_OBJECT

public:
  Plot2DWindow (MainWindow *parent = nullptr);
  ~Plot2DWindow ();
  void drawCurve ();
  void createMenubar ();
  void setResolution ();
  bool appendSeries (double x, double y,
		     double &realMax, double &realMin);

private:
  MainWindow *mw;
#if 0
  QLineEdit *indexVarName;
#endif
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
  
  
protected:
  void closeEvent(QCloseEvent *event) override;
};

#endif // PLOT2DWINDOW_H

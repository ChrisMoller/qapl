#ifndef CHART2DWINDOW_H
#define CHART2DWINDOW_H

#include <QtWidgets>
#include <QAbstractSeries>
#include <QChart>
#include <QChartView>
#include <QSplineSeries>
#include <QRubberBand>

#include "mainwindow.h"
#include "plot2dwindow.h"
//#include "chart2dwindow.h"

#include "enums.h"

class Plot2DWindow;
class Chart2DWindow;

// https://doc.qt.io/qt-5/qabstractscrollarea.html

#if 0
// https://doc.qt.io/qt-5/qabstractscrollarea.html
virtual void	mouseDoubleClickEvent(QMouseEvent *e) override
virtual void	mouseMoveEvent(QMouseEvent *e) override
virtual void	mousePressEvent(QMouseEvent *e) override
virtual void	mouseReleaseEvent(QMouseEvent *e) override
virtual void	wheelEvent(QWheelEvent *e) override
#endif

class QaplChartView : public QChartView
{
  Q_OBJECT
  
public:
  QaplChartView(Chart2DWindow *parent);
  ~QaplChartView () {}
  QPointF coordinateTransform (QPoint d);

protected:
  void	mouseMoveEvent(QMouseEvent *e) override;
  void	mousePressEvent(QMouseEvent *e) override;
  void	mouseReleaseEvent(QMouseEvent *e) override;
  void	wheelEvent(QWheelEvent *event) override;

private:
  void chartLabel ();
  QPoint origin;
  QPoint currentPoint;
  QRubberBand *rubberBand;
  Chart2DWindow *pc;
};

class Chart2DWindow : public QMainWindow
{
  Q_OBJECT

public:
  Chart2DWindow (Plot2DWindow *parent, MainWindow *mainWin);
  ~Chart2DWindow ();
  void drawCurves ();
  void drawCurve (QString aplXExpr, QString aplYExpr,aspect_e aspect,
		  QString label, QPen pen, series_mode_e mode,
		  double &realXMax, double &realXMin,
		  double &realYMax, double &realYMin,
		  std::vector<double> idxVector, double markerSize);
  //  void exportImage ();
  void exportAsImage ();
  
  Plot2DWindow *pw;
  QLabel *readout;

private:

  MainWindow *mw;
  void createMenubar ();
#if 0
  void setAxesFont (QAbstractAxis *axis);
#endif
  QChartView *chartView;
  QChart *chart;
  QAbstractSeries *series;
  bool appendSeries (double x, double y, series_mode_e mode,
		     double &realXMax, double &realXMin,
		     double &realYMax, double &realYMin);
  QString currentImageExportFile;
  bool showPreview (QPixmap plotPixmap);
  double fontScale;
  double currentWidth;
  double currentHeight;
};

#endif // CHART2DWINDOW_H

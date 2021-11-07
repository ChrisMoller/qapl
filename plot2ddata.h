#ifndef PLOT2DDATA_H
#define PLOT2DDATA_H

#include "mainwindow.h"
#include "chart2dwindow.h"
#include "complexspinbox.h"

class PlotCurve {
public:
  PlotCurve () {
    a_mode	= MODE_BUTTON_SPLINE;
    a_aspect	= ASPECT_REAL;
    a_markerSize = 15.0;
  }
  PlotCurve (QString &e,  aspect_e &a, QString l, QPen p,
	     series_mode_e m, double ms) {
    a_expression = e;
    a_aspect = a;
    a_pen = p;
    a_title = l;
    a_mode = m;
    a_markerSize = ms;
  }
  QString title () { return a_title; }
  void setTitle (QString title) { a_title = title; }
  QString expression () { return a_expression; }
  void setExpression (QString expression) { a_expression = expression; }
  QPen *pen () { return &a_pen; }
  void setPen (QPen pen) { a_pen = pen; }
  aspect_e aspect () { return a_aspect; }
  void setAspect (aspect_e aspect) { a_aspect = aspect; }
  series_mode_e mode () { return a_mode; }
  void setMode (series_mode_e mode) { a_mode = mode; }
  double markerSize () { return a_markerSize; }
  void setMarkerSize (double ms) { a_markerSize = ms; }
  
private:
  QString	a_title;
  QString	a_expression;
  aspect_e	a_aspect;
  QPen		a_pen;
  series_mode_e a_mode;
  double	a_markerSize;
};

class Plot2dData {
public:
  Plot2dData (MainWindow *parent) {
    mw = parent;
    resolution	= 16.0;
    theme = mw->getSettings ()->value (SETTINGS_PLOT_THEME).toInt ();
    
    QVariant fontVariant;
    QVariant colourVariant;
  
    fontVariant = QVariant (QFont ("Times", 10));
    axisLabelFont = mw->getSettings ()->value (SETTINGS_AXIS_LABEL_FONT,
					       fontVariant).toString ();

    fontVariant = QFont ("Times", 12);
    axisTitleFont = mw->getSettings ()->value (SETTINGS_AXIS_TITLE_FONT,
					       fontVariant).toString ();

    fontVariant = QFont ("Times", 18);
    chartTitleFont = mw->getSettings ()->value (SETTINGS_CHART_TITLE_FONT,
						fontVariant).toString ();

    colourVariant = QVariant ("white");
    axisLabelColour = QColor (mw->getSettings ()
			      ->value (SETTINGS_AXIS_LABEL_COLOUR,
				       colourVariant).toString ());

    axisColour = QColor (mw->getSettings ()
			 ->value (SETTINGS_AXIS_COLOUR,
				  colourVariant).toString ());

    axisTitleColour = QColor (mw->getSettings ()
			      ->value (SETTINGS_AXIS_TITLE_COLOUR,
				       colourVariant).toString ());

    chartTitleColour = QColor (mw->getSettings ()
			       ->value (SETTINGS_CHART_TITLE_COLOUR,
					colourVariant).toString ());
  };
  ~Plot2dData () {};
  void setCurrentPlotFile (QString fn) { currentPlotFile = fn; }

  // private:
  int		resolution;
  int 		theme;
  QString 	bgFile;
  Qt::AspectRatioMode aspectMode;
  QColor	axisColour;
  QFont  	chartTitleFont;
  QColor 	chartTitleColour;
  QFont  	axisLabelFont;
  QColor 	axisLabelColour;
  QFont  	axisTitleFont;
  QColor 	axisTitleColour;
  QString 	xTitle;
  QString 	yTitle;
  QString 	chartTitle;
  QString 	indexVariable;
  PlotCurve	activeCurve;
  QList<PlotCurve *> plotCurves;
  std::complex<double> rangeInit;
  std::complex<double> rangeFinal;
  MainWindow   *mw;
  QString 	currentPlotFile;
};

#endif // PLOT2DDATA_H

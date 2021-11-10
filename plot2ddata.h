#ifndef PLOT2DDATA_H
#define PLOT2DDATA_H

#include "mainwindow.h"
#include "chart2dwindow.h"
#include "complexspinbox.h"

class PlotParameter {
public:
  PlotParameter () {
  }

  PlotParameter (QString vn, double r, double i) {
    a_vname = vn;
    a_real = r;
    a_imag = i;
  }
  QString vname () { return a_vname; }
  void setVname (QString v) { a_vname = v; }
  double  real ()  { return a_real; }
  void setReal (double r) { a_real = r; }
  double  imag ()  { return a_imag; }
  void setImag (double i) { a_imag = i; }
  
private:
  QString a_vname;
  double  a_real;
  double  a_imag;
};

class PlotCurve {
public:
  PlotCurve () {
    a_mode	= MODE_BUTTON_SPLINE;
    a_aspect	= ASPECT_REAL;
    a_markerSize = 15.0;
  }
  PlotCurve (QString &ex,  QString &ey, aspect_e &a, QString l, QPen p,
	     series_mode_e m, double ms) {
    x_expression = ex;
    y_expression = ey;
    a_aspect = a;
    a_pen = p;
    a_title = l;
    a_mode = m;
    a_markerSize = ms;
  }
  QString title () { return a_title; }
  void setTitle (QString title) { a_title = title; }
  QString Xexpression () { return x_expression; }
  void setXExpression (QString expression) { x_expression = expression; }
  QString Yexpression () { return y_expression; }
  void setYExpression (QString expression) { y_expression = expression; }
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
  QString	x_expression;
  QString	y_expression;
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
  bool		doPolar;
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
  QList<PlotParameter *> plotParameters;
  std::complex<double> rangeInit;
  std::complex<double> rangeFinal;
  MainWindow   *mw;
  QString 	currentPlotFile;
};

#endif // PLOT2DDATA_H

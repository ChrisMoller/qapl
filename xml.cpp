#include "mainwindow.h"
#include "plot2dwindow.h"

#include "enums.h"

/***
                         // SETTINGS_PLOT_THEME
    <qapl resolution="..." theme="..." polar="...">
	                 // SETTINGS_CHART_TITLE_FONT
	                 // SETTINGS_CHART_TITLE_COLOUR
      <chart font="..." colour="...">
        <axes colour="...">	// (SETTINGS_AXIS_COLOUR
	                 // SETTINGS_AXIS_LABEL_FONT
	                 // SETTINGS_AXIS_LABEL_COLOUR
	  <label font="..." colour="..." >
	                 // SETTINGS_AXIS_TITLE_FONT
	                 // SETTINGS_AXIS_TITLE_COLOUR
	  <title font="..." colour="..." >
        </axes>
      </chart>
      <title>....</title>
      <index>....</index>
      <xlabel>....</xlabel>
      <ylabel>....</ylabel>
      <range>
        <initial>...</initial
        <final>...</final>
      </range>
      <active aspect="..." mode="..." marker="...">
        <Xexpression>....</Xexpression>
        <Yexpression>....</Yexpression>
        <label>....</label>
        <pen colour="..." style="..." width="..."/>
      </active>
      <stack index="..." aspect="..." mode="..." marker="...">
        <Xexpression>....</Xexpression>
        <Yexpression>....</Yexpression>
        <label>....</label>
        <pen colour="..." style="..." width="..."/>
      </stack>

      repeat stack...

      <parameter index="..." real="..." imag="...">...</parameter>

      repeat parameter...
      
      <label index="..." angle="..." halign="..." valign="..." world="..."
      xpos="..." ypos="..." font="..." colour="...">...</label>

      repeat label...
      
    </qapl>
 ***/

typedef struct {
  QString tag;
  int idx;
  int value;
} xml_tag_s;

#include "xml.h"

#define xml_def(v, l) #v, XML_ ## v, l
xml_tag_s xml_tags[] = {
#include "XMLtags.def"
};

static QHash<const QString, int> xmlhash;

int Plot2DWindow::parseAxesLabel (QXmlStreamReader &stream,
				   Plot2dData *plot2DData, bool trace)
{
  if (trace) fprintf (stderr, "parseAxesLabel\n");
  int rc = XML_OK;
  bool run = true;
  while (run && rc == XML_OK) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      plot2DData->axisLabelColour =
	QColor (attrs.value (xml_tags[XML_colour].tag));
      if (trace)
	fprintf (stderr, "axisLabelColour %s\n",
	  toCString (plot2DData->axisLabelColour.name (QColor::HexArgb)));
      {
	QString fs (attrs.value (xml_tags[XML_font].tag).toString ());
	QFont ft;
	ft.fromString (fs);
	plot2DData->axisLabelFont = QFont (ft);
	if (trace)
	  fprintf (stderr, "axisLabelefont %s\n",
		   toCString (fs));
      }
    }
    QXmlStreamReader::TokenType tt = stream.readNext ();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      break;
    case QXmlStreamReader::EndDocument:
      run = false;
      break;
    case QXmlStreamReader::EndElement:
      run = false;
      break;
    case QXmlStreamReader::Invalid:
      run = false;
      rc = XML_pen;
      showError (stream);
      break;
    default:
      break;
    }
  }
  
  return rc;
}

int Plot2DWindow::parseAxesTitle (QXmlStreamReader &stream,
				   Plot2dData *plot2DData, bool trace)
{
  if (trace) fprintf (stderr, "parseAxesTitle\n");
  int rc = XML_OK;
  bool run = true;
  while (run && rc == XML_OK) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      plot2DData->axisTitleColour =
	QColor (attrs.value (xml_tags[XML_colour].tag));
      if (trace)
	fprintf (stderr, "axisColour %s\n",
	  toCString (plot2DData->axisTitleColour.name (QColor::HexArgb)));
      {
	QString fs (attrs.value (xml_tags[XML_font].tag).toString ());
	QFont ft;
	ft.fromString (fs);
	plot2DData->axisTitleFont = QFont (ft);
	if (trace)
	  fprintf (stderr, "axisTitlefont %s\n",
		   toCString (fs));
      }
    }
    QXmlStreamReader::TokenType tt = stream.readNext ();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      break;
    case QXmlStreamReader::EndDocument:
      run = false;
      break;
    case QXmlStreamReader::EndElement:
      run = false;
      break;
    case QXmlStreamReader::Invalid:
      run = false;
      rc = XML_pen;
      showError (stream);
      break;
    default:
      break;
    }
  }
  
  return rc;
}

int Plot2DWindow::parsePen (QXmlStreamReader &stream,
			     Plot2dData *plot2DData, bool trace)
{
  if (trace) fprintf (stderr, "parsePen\n");
  int rc = XML_OK;
  bool run = true;
  while (run && rc == XML_OK) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      QBrush brush (QColor (attrs.value (xml_tags[XML_colour].tag)));
      Qt::PenStyle style = (Qt::PenStyle)attrs.value (xml_tags[XML_style].tag).toInt ();
      double width = attrs.value (xml_tags[XML_width].tag).toFloat ();
      QPen pen (brush, width, style);
      plot2DData->activeCurve.setPen (pen);
      if (trace)
	fprintf (stderr, "active pen colour %s, style %d, width %g\n",
		 toCString (brush.color ().name (QColor::HexArgb)),
		 (int)style, width);
    }
    QXmlStreamReader::TokenType tt = stream.readNext ();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      break;
    case QXmlStreamReader::EndDocument:
      run = false;
      break;
    case QXmlStreamReader::EndElement:
      run = false;
      break;
    case QXmlStreamReader::Invalid:
      run = false;
      rc = XML_pen;
      showError (stream);
      break;
    default:
      break;
    }
  }
  
  return rc;
}

int Plot2DWindow::parseStackPen (QXmlStreamReader &stream,
				  PlotCurve *plotCurve, bool trace, int index)
{
  if (trace) fprintf (stderr, "parseStackPenl\n");
  int rc = XML_OK;
  bool run = true;
  while (run && rc == XML_OK) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      QBrush brush (QColor (attrs.value (xml_tags[XML_colour].tag)));
      Qt::PenStyle style = (Qt::PenStyle)attrs.value (xml_tags[XML_style].tag).toInt ();
      double width = attrs.value (xml_tags[XML_width].tag).toFloat ();
      QPen pen (brush, width, style);
      plotCurve->setPen (pen);
      if (trace)
	fprintf (stderr, "stack %d: pen colour %s, style %d, width %g\n",
		 index,
		 toCString (brush.color ().name (QColor::HexArgb)),
		 (int)style, width);
    }
    QXmlStreamReader::TokenType tt = stream.readNext ();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      break;
    case QXmlStreamReader::EndDocument:
      run = false;
      break;
    case QXmlStreamReader::EndElement:
      run = false;
      break;
    case QXmlStreamReader::Invalid:
      run = false;
      rc = XML_pen;
      showError (stream);
      break;
    default:
      break;
    }
  }
  
  return rc;
}

int Plot2DWindow::parseAxes (QXmlStreamReader &stream, Plot2dData *plot2DData,
			      bool trace)
{
  if (trace) fprintf (stderr, "parseAxes\n");
  int rc = XML_OK;
  bool run = true;
  while (run && rc == XML_OK) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      plot2DData->axisColour =
	QColor (attrs.value (xml_tags[XML_colour].tag));
      if (trace)
	fprintf (stderr, "axisColour %s\n",
	  toCString (plot2DData->chartTitleColour.name (QColor::HexArgb)));
    }
    QXmlStreamReader::TokenType tt = stream.readNext ();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      switch (xmlhash.value (sn)) {
      case XML_label:
	rc = parseAxesLabel (stream, plot2DData, trace);
        break;
      case XML_title:
	rc  =parseAxesTitle (stream, plot2DData, trace);
        break;
      default:
	if (trace)
	  fprintf (stderr, "unhandled axes value \"%s\"\n", toCString (sn));
	rc = XML_axes;
	break;
      }
      break;
    case QXmlStreamReader::EndDocument:
      run = false;
      break;
    case QXmlStreamReader::EndElement:
      run = false;
      break;
    case QXmlStreamReader::Invalid:
      run = false;
      rc = XML_pen;
      showError (stream);
      break;
    default:
      break;
    }
  }
  
  return rc;
}

int Plot2DWindow::parseRange (QXmlStreamReader &stream,
			       Plot2dData *plot2DData, bool trace)
{
  if (trace) fprintf (stderr, "parseRange\n");
  int rc = XML_OK;
  bool run = true;
  while (run && rc == XML_OK) {
    QXmlStreamReader::TokenType tt = stream.readNext ();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      switch (xmlhash.value (sn)) {
      case XML_initial:
	{
	  coord_e type;
	  double real;
	  double imag;
	  ComplexSpinBox::parseComplex (type, real, imag,
					stream.readElementText ());
	  plot2DData->rangeInit = std::complex<double>(real, imag);
	  if (trace)
	    fprintf (stderr, "initial %gj%g\n", real, imag);
	}
        break;
      case XML_final:
	{
	  coord_e type;
	  double real;
	  double imag;
	  ComplexSpinBox::parseComplex (type, real, imag,
					stream.readElementText ());
	  plot2DData->rangeFinal = std::complex<double>(real, imag);
	  if (trace)
	    fprintf (stderr, "final %gj%g\n", real, imag);
	}
        break;
      default:
	if (trace)
	  fprintf (stderr, "unhandled range value \"%s\"\n", toCString (sn));
	rc = XML_range;
	break;
      }
      break;
    case QXmlStreamReader::EndDocument:
      run = false;
      break;
    case QXmlStreamReader::EndElement:
      run = false;
      break;
    case QXmlStreamReader::Invalid:
      run = false;
      rc = XML_pen;
      showError (stream);
      break;
    default:
      break;
    }
  }
  
  return rc;
}

int Plot2DWindow::parseChart (QXmlStreamReader &stream,
			       Plot2dData *plot2DData, bool trace)
{
  if (trace) fprintf (stderr, "parseChart\n");
  int rc = XML_OK;
  bool run = true;
  while (run && rc == XML_OK) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      plot2DData->chartTitleColour =
	QColor (attrs.value (xml_tags[XML_colour].tag));
      if (trace)
	fprintf (stderr, "chartTitleColour %s\n",
	 toCString (plot2DData->chartTitleColour.name (QColor::HexArgb)));
      {
	QString fs (attrs.value (xml_tags[XML_font].tag).toString ());
	QFont ft;
	ft.fromString (fs);
	plot2DData->chartTitleFont = QFont (ft);
	if (trace)
	  fprintf (stderr, "chartTitlefont %s\n",
		   toCString (fs));
      }
    }
    QXmlStreamReader::TokenType tt = stream.readNext ();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      switch (xmlhash.value (sn)) {
      case XML_axes:
	rc= parseAxes (stream, plot2DData, trace);
        break;
      default:
	if (trace)
	  fprintf (stderr, "unhandled qapl value \"%s\"\n", toCString (sn));
	rc = XML_chart;
	break;
      }
      break;
    case QXmlStreamReader::EndDocument:
      run = false;
      break;
    case QXmlStreamReader::EndElement:
      run = false;
      break;
    case QXmlStreamReader::Invalid:
      run = false;
      rc = XML_pen;
      showError (stream);
      break;
    default:
      break;
    }
  }
  
  return rc;
}

int Plot2DWindow::parseActive (QXmlStreamReader &stream,
				Plot2dData *plot2DData, bool trace)
{
  if (trace) fprintf (stderr, "parseActive\n");
  int rc = XML_OK;
  bool run = true;
  while (run && rc == XML_OK) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      plot2DData->activeCurve.setAspect ((aspect_e)((attrs.value (xml_tags[XML_aspect].tag)).toInt ()));
      plot2DData->activeCurve.setMode ((series_mode_e)((attrs.value (xml_tags[XML_mode].tag)).toInt ()));
      plot2DData->activeCurve.setMarkerSize ((attrs.value (xml_tags[XML_marker].tag)).toFloat ());
      if (trace)
	fprintf (stderr, "active: aspect %d, mode %d, marker size %g\n",
		 plot2DData->activeCurve.aspect (),
		 plot2DData->activeCurve.mode (),
		 plot2DData->activeCurve.markerSize ());
		 
    }
    QXmlStreamReader::TokenType tt = stream.readNext ();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      switch (xmlhash.value (sn)) {
      case XML_Xexpression:
	{
	  QString exp = stream.readElementText ();
	  plot2DData->activeCurve.setXExpression (exp);
	  if (trace)
	    fprintf (stderr, "active X exp: %s\n",
		     toCString (exp));
	}
        break;
      case XML_Yexpression:
	{
	  QString exp = stream.readElementText ();
	  plot2DData->activeCurve.setYExpression (exp);
	  if (trace)
	    fprintf (stderr, "active Y exp: %s\n",
		     toCString (exp));
	}
        break;
      case XML_label:
	plot2DData->activeCurve.setTitle (stream.readElementText ());
	if (trace)
	  fprintf (stderr, "active curve title: %s\n",
		   toCString (plot2DData->activeCurve.title ()));
        break;
      case XML_pen:
	rc = parsePen (stream, plot2DData, trace);
        break;
      default:
	if (trace)
	  fprintf (stderr, "unhandled active value \"%s\"\n", toCString (sn));
	rc = XML_active;
	break;
      }
      break;
    case QXmlStreamReader::EndDocument:
      run = false;
      break;
    case QXmlStreamReader::EndElement:
      run = false;
      break;
    case QXmlStreamReader::Invalid:
      run = false;
      rc = XML_pen;
      showError (stream);
      break;
    default:
      break;
    }
  }
  
  return rc;
}


int Plot2DWindow::parseParameter (QXmlStreamReader &stream,
				  Plot2dData *plot2DData, bool trace)
{
  if (trace) fprintf (stderr, "parseParameter\n");
  int rc = XML_OK;
  bool run = true;
  int index = -1;
  PlotParameter *plotParameter = new PlotParameter ();
  while (run && rc == XML_OK) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      if (!attrs.isEmpty ()) {
	index = ((attrs.value (xml_tags[XML_index].tag)).toInt ());
	plotParameter->setReal ((attrs.value (xml_tags[XML_real].tag)).toFloat ());
	plotParameter->setImag ((attrs.value (xml_tags[XML_imag].tag)).toFloat ());
      }
      if (trace)
	fprintf (stderr, "parameter %d, real %g, imag %g\n",
		 index,
		 plotParameter->real (),
		 plotParameter->imag ());
    }
    {
      QString name = stream.readElementText ();
      plotParameter->setVname (name);
      if (trace)
	fprintf (stderr, "vname %d: %s\n", index, toCString (name));
    }
    run = false;
  }

  plot2DData->plotParameters.append(plotParameter);
    
  return rc;
}

int Plot2DWindow::parseLabel (QXmlStreamReader &stream,
			      Plot2dData *plot2DData, bool trace)
{
  if (trace) fprintf (stderr, "parseLabel\n");
  int rc = XML_OK;
  bool run = true;
  int index = -1;
  PlotLabel *plotLabel = new PlotLabel ();
  while (run && rc == XML_OK) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      index = ((attrs.value (xml_tags[XML_index].tag)).toInt ());
      plotLabel->setAngle ((attrs.value (xml_tags[XML_angle].tag)).toFloat ());
      plotLabel->setHorizontalAlignment ((attrs.value (xml_tags[XML_halign].tag)).toInt ());
      plotLabel->setVerticalAlignment ((attrs.value (xml_tags[XML_valign].tag)).toInt ());
      plotLabel->setWorldCoordinates ((attrs.value (xml_tags[XML_world].tag)).toInt ());
      double xpos = (attrs.value (xml_tags[XML_xpos].tag)).toFloat ();
      double ypos = (attrs.value (xml_tags[XML_ypos].tag)).toFloat ();
      plotLabel->setPosition (QPointF (xpos, ypos));
      {
	QString fs (attrs.value (xml_tags[XML_font].tag).toString ());
	QFont ft;
	ft.fromString (fs);
	plotLabel->setFont (QFont (ft));
      }
      plotLabel->setColour (QColor (attrs.value (xml_tags[XML_colour].tag)));
      if (trace) {
	fprintf (stderr,
		 "labels %d: angle %g, halign %d, valign %d, xp %g, yp %g\n",
		 index,
		 plotLabel->getAngle (),
		 plotLabel->getHorizontalAlignment (),
		 plotLabel->getVerticalAlignment (),
		 xpos,
		 ypos);
	fprintf (stderr, "\tfont \"%s\"m colour %s \n",
		 toCString (plotLabel->getFont ().toString ()),
		 toCString (plotLabel->getColour ().name (QColor::HexArgb)));
      }
    }
    {
      QString name = stream.readElementText ();
      plotLabel->setLabel (name);
      if (trace)
	fprintf (stderr, "\tlabel %s\n",  toCString (plotLabel->getLabel ()));
    }
    run = false;
  }
  plot2DData->plotLabels.append (plotLabel);

  return rc;
}

int Plot2DWindow::parseStack (QXmlStreamReader &stream,
			       Plot2dData *plot2DData, bool trace)
{
  if (trace) fprintf (stderr, "parseStack\n");
  int rc = XML_OK;
  bool run = true;
  int index = -1;
  PlotCurve *plotCurve = new PlotCurve ();
  while (run && rc == XML_OK) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      index = ((attrs.value (xml_tags[XML_index].tag)).toInt ());
      plotCurve->setAspect ((aspect_e)((attrs.value (xml_tags[XML_aspect].tag)).toInt ()));
      plotCurve->setMode ((series_mode_e)((attrs.value (xml_tags[XML_mode].tag)).toInt ()));
      plotCurve->setMarkerSize ((attrs.value (xml_tags[XML_marker].tag)).toFloat ());
      if (trace)
	fprintf (stderr, "stack %d: aspect %d, mode %d, marker size %g\n",
		 index,
		 plotCurve->aspect (),
		 plotCurve->mode (),
		 plotCurve->markerSize ());
    }
    QXmlStreamReader::TokenType tt = stream.readNext ();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      switch (xmlhash.value (sn)) {
      case XML_Xexpression:
	{
	  QString exp = stream.readElementText ();
	  plotCurve->setXExpression (exp);
	}
        break;
      case XML_Yexpression:
	{
	  QString exp = stream.readElementText ();
	  plotCurve->setYExpression (exp);
	}
        break;
      case XML_label:
	plotCurve->setTitle (stream.readElementText ());
        break;
      case XML_pen:
	rc = parseStackPen (stream, plotCurve, trace, index);
        break;
      default:
	if (trace)
	  fprintf (stderr, "unhandled active value \"%s\"\n", toCString (sn));
	rc = XML_stack;
	break;
      }
      break;
    case QXmlStreamReader::EndDocument:
      run = false;
      break;
    case QXmlStreamReader::EndElement:
      run = false;
      break;
    case QXmlStreamReader::Invalid:
      run = false;
      rc = XML_pen;
      showError (stream);
      break;
    default:
      break;
    }
  }

  plot2DData->plotCurves.append(plotCurve);
  
  return rc;
}

int Plot2DWindow::parseQapl (QXmlStreamReader &stream, Plot2dData *plot2DData,
			 bool trace)
{
  if (trace) fprintf (stderr, "parseQapl\n");
  int rc = XML_OK;
  bool run = true;
  while (run && rc == XML_OK) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      plot2DData->resolution =
	(attrs.value (xml_tags[XML_resolution].tag)).toInt ();
      plot2DData->theme =
	(attrs.value (xml_tags[XML_theme].tag)).toInt ();
      plot2DData->doPolar =
	(attrs.value (xml_tags[XML_polar].tag)).toInt ();
      if (trace) 
	fprintf (stderr, "resolution = %d, theme = %d, doPolar = %d\n",
		 plot2DData->resolution,
		 plot2DData->theme,
		 plot2DData->doPolar);
    }
    QXmlStreamReader::TokenType tt = stream.readNext ();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      switch (xmlhash.value (sn)) {
      case XML_chart:
	rc = parseChart (stream, plot2DData, trace);
        break;
      case XML_title:
	plot2DData->chartTitle = stream.readElementText ();
	if (trace)
	  fprintf (stderr, "chartTitle \"%s\"\n",
		   toCString (plot2DData->chartTitle));
        break;
      case XML_index:
	plot2DData->indexVariable = stream.readElementText ();
	if (trace)
	  fprintf (stderr, "indexVariable \"%s\"\n",
		   toCString (plot2DData->indexVariable));
        break;
      case XML_xlabel:
	plot2DData->xTitle = stream.readElementText ();
	if (trace)
	  fprintf (stderr, "xTitle \"%s\"\n",
		   toCString (plot2DData->xTitle));
        break;
      case XML_ylabel:
	plot2DData->yTitle = stream.readElementText ();
	if (trace)
	  fprintf (stderr, "yTitle \"%s\"\n",
		   toCString (plot2DData->yTitle));
        break;
      case XML_range:
	rc = parseRange (stream, plot2DData, trace);
        break;
      case XML_active:
	rc = parseActive (stream, plot2DData, trace);
        break;
      case XML_stack:
	rc = parseStack (stream, plot2DData, trace);
        break;
      case XML_parameter:
	rc = parseParameter (stream, plot2DData, trace);
        break;
      case XML_label:
	{
	  rc = parseLabel (stream, plot2DData, trace);
	}
        break;
      default:
	if (trace)
	  fprintf (stderr, "unhandled qapl value \"%s\"\n", toCString (sn));
	rc = XML_qapl;
	break;
      }
      break;
    case QXmlStreamReader::Invalid:
      run = false;
      rc = XML_pen;
      showError (stream);
      break;
    case QXmlStreamReader::EndDocument:
      run = false;
      break;
    case QXmlStreamReader::EndElement:
      run = false;
      break;
    default:
      break;
    }
  }
  
  return rc;
}

 void Plot2DWindow::showError (QXmlStreamReader &stream)
 {
   QString msg = QString ("%1 at line %2, column %3")
     .arg (stream.errorString ())
     .arg (stream.lineNumber ())
     .arg (stream.columnNumber ());
   QMessageBox msgBox (QMessageBox::Critical,
		       "Parsing error",
		       msg);
   msgBox.exec();
 }

void Plot2DWindow::readXML (QString &fileName, MainWindow *mw, bool trace)
{
  static bool xmlInitialised = false;
  if (!xmlInitialised) {
    for (long unsigned int i = 0; i < XML_LAST; i++)
      xmlhash.insert (xml_tags[i].tag, (int)i);
    xmlInitialised = true;
  }

  Plot2dData *plot2DData =  new Plot2dData (mw);
  plot2DData->currentPlotFile = fileName;
  
  QFile file (fileName);
  file.open (QIODevice::ReadOnly | QIODevice::Text);
  QXmlStreamReader stream(&file);

  bool run = true;
  int rc = XML_OK;
  while (run && rc == XML_OK) {
    QXmlStreamReader::TokenType tt = stream.readNext();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      switch (xmlhash.value (sn)) {
      case XML_qapl:
	rc = parseQapl (stream, plot2DData, trace);
        break;
      default:
	if (trace)
	  fprintf (stderr, "unhandled top value \"%s\"\n", toCString (sn));
	rc = XML_LAST;
	break;
      }
      break;
    case QXmlStreamReader::EndElement:
      run = false;
      break;
    case QXmlStreamReader::EndDocument:
      new Plot2DWindow (mw, plot2DData);
      run = false;
      break;
    case QXmlStreamReader::StartDocument:
      break;
    case QXmlStreamReader::Invalid:
      run = false;
      rc = XML_pen;
      showError (stream);
      break;
    default:
      if (trace)
	fprintf (stderr, "unhandled ttt %d \"%s\"\n", tt, toCString (sn));
      rc = XML_LAST;
      break;
    }
  }
#if 0
  if (rc != XML_OK)
    fprintf (stderr, "parsing error %d\n", rc);  // fixme--message box
#endif
}

void  Plot2DWindow::dumpPlotLabel (int index, PlotLabel *plabel,
				   QXmlStreamWriter &stream)
{
  stream.writeStartElement(xml_tags[XML_label].tag);
  stream.writeAttribute(xml_tags[XML_index].tag,
			QString::number (index));
  stream.writeAttribute(xml_tags[XML_angle].tag,
			QString::number (plabel->getAngle ()));
  stream.writeAttribute(xml_tags[XML_halign].tag,
			QString::number (plabel->getHorizontalAlignment ()));
  stream.writeAttribute(xml_tags[XML_valign].tag,
			QString::number (plabel->getVerticalAlignment ()));
  stream.writeAttribute(xml_tags[XML_world].tag,
			QString::number (plabel->getWorldCoordinates ()));
  stream.writeAttribute(xml_tags[XML_xpos].tag,
			QString::number (plabel->getPosition ().x ()));
  stream.writeAttribute(xml_tags[XML_ypos].tag,
			QString::number (plabel->getPosition ().y ()));
  stream.writeAttribute(xml_tags[XML_font].tag,
			plabel->getFont ().toString ());
  stream.writeAttribute(xml_tags[XML_colour].tag,
			plabel->getColour ().name (QColor::HexArgb));
  stream.writeCharacters(plabel->getLabel ());
  stream.writeEndElement(); // label
}

void Plot2DWindow::dumpXML (QString fileName)
{
  QFile file (fileName);
  file.open (QIODevice::WriteOnly | QIODevice::Text);
  QXmlStreamWriter stream(&file);
  stream.setAutoFormatting(true);
  stream.writeStartDocument();

  /*** qapl element ***/
  
  stream.writeStartElement(xml_tags[XML_qapl].tag);
  stream.writeAttribute(xml_tags[XML_resolution].tag,
			QString::number (getResolution ()));
  stream.writeAttribute(xml_tags[XML_theme].tag,
			QString::number (getTheme ()));
  stream.writeAttribute(xml_tags[XML_polar].tag,
			QString::number (getPolar ()));

  /*** chart element ***/
  
  stream.writeStartElement(xml_tags[XML_chart].tag);
  stream.writeAttribute(xml_tags[XML_font].tag,
			getChartTitleFont ().toString ());
  stream.writeAttribute(xml_tags[XML_colour].tag,
			getChartTitleColour ().name (QColor::HexArgb));

  /*** axes element ***/

  stream.writeStartElement(xml_tags[XML_axes].tag);
  stream.writeAttribute(xml_tags[XML_colour].tag,
			getAxisColour ().name (QColor::HexArgb));

  /*** axis label element ***/
  
  stream.writeStartElement(xml_tags[XML_label].tag);
  stream.writeAttribute(xml_tags[XML_font].tag,
			getAxisLabelFont ().toString ());
  stream.writeAttribute(xml_tags[XML_colour].tag,
			getAxisLabelColour ().name (QColor::HexArgb));
  stream.writeEndElement(); // axis label

  /*** axis title element ***/
  
  stream.writeStartElement(xml_tags[XML_title].tag);
  stream.writeAttribute(xml_tags[XML_font].tag,
			getAxisTitleFont ().toString ());
  stream.writeAttribute(xml_tags[XML_colour].tag,
			getAxisTitleColour ().name (QColor::HexArgb));
  stream.writeEndElement(); // axis title

  stream.writeEndElement(); // axes
  
  stream.writeEndElement(); // chart

  /*** chart title element ***/
  
  stream.writeStartElement(xml_tags[XML_title].tag);
  stream.writeCharacters(getChartTitle ());
  stream.writeEndElement(); // chart title

  /*** chart index var element ***/
  
  stream.writeStartElement(xml_tags[XML_index].tag);
  stream.writeCharacters(getIndexVariable ());
  stream.writeEndElement(); // chart index var
  
  /*** chart x label element ***/
  
  stream.writeStartElement(xml_tags[XML_xlabel].tag);
  stream.writeCharacters(getXTitle ());
  stream.writeEndElement(); // chart x label
  
  /*** chart y label element ***/
  
  stream.writeStartElement(xml_tags[XML_ylabel].tag);
  stream.writeCharacters(getYTitle ());
  stream.writeEndElement(); // chart y label

  /*** range element ***/
  
  stream.writeStartElement(xml_tags[XML_range].tag);

  /*** initial element ***/
  
  stream.writeStartElement(xml_tags[XML_initial].tag);
  stream.writeCharacters(rangeInit->getString ());
  stream.writeEndElement(); // initial 

  /*** final element ***/
  
  stream.writeStartElement(xml_tags[XML_final].tag);
  stream.writeCharacters(rangeFinal->getString ());
  stream.writeEndElement(); // final

  stream.writeEndElement(); // range

  /*** active element ***/
  
  stream.writeStartElement(xml_tags[XML_active].tag);
  stream.writeAttribute(xml_tags[XML_aspect].tag,
			QString::number (getAspect ()));
  stream.writeAttribute(xml_tags[XML_mode].tag,
			QString::number (getMode ()));
  stream.writeAttribute(xml_tags[XML_marker].tag,
			QString::number (getMarkerSize ()));

  /*** expression elements ***/
  
  stream.writeStartElement(xml_tags[XML_Xexpression].tag);
  stream.writeCharacters(getAplXExpression ());
  stream.writeEndElement(); // expression
  
  stream.writeStartElement(xml_tags[XML_Yexpression].tag);
  stream.writeCharacters(getAplYExpression ());
  stream.writeEndElement(); // expression

  /*** label element ***/
  
  stream.writeStartElement(xml_tags[XML_label].tag);
  stream.writeCharacters(getCurveTitle ());
  stream.writeEndElement(); // label

  /*** pen element ***/
  
  stream.writeStartElement(xml_tags[XML_pen].tag);
  stream.writeAttribute(xml_tags[XML_colour].tag,
			getPen ()->color ().name (QColor::HexArgb));
  stream.writeAttribute(xml_tags[XML_style].tag,
			QString::number (getPen ()->style ()));
  stream.writeAttribute(xml_tags[XML_width].tag,
			QString::number (getPen ()->widthF ()));
  stream.writeEndElement(); // pen

  stream.writeEndElement(); // active

  for (int i = 0; i < getPlotCurves ().size (); i++) {

    /*** stack element ***/

    PlotCurve *pc = getPlotCurves ()[i];
  
    stream.writeStartElement(xml_tags[XML_stack].tag);
    stream.writeAttribute(xml_tags[XML_index].tag,
			  QString::number (i));
    stream.writeAttribute(xml_tags[XML_aspect].tag,
			  QString::number (pc->aspect ()));
    stream.writeAttribute(xml_tags[XML_mode].tag,
			  QString::number (pc->mode ()));

    /*** expression elements ***/
  
    stream.writeStartElement(xml_tags[XML_Xexpression].tag);
    stream.writeCharacters(pc->Xexpression ());
    stream.writeEndElement(); // expression
  
    stream.writeStartElement(xml_tags[XML_Yexpression].tag);
    stream.writeCharacters(pc->Yexpression ());
    stream.writeEndElement(); // expression

    /*** label element ***/
  
    stream.writeStartElement(xml_tags[XML_label].tag);
    stream.writeCharacters(pc->title ());
    stream.writeEndElement(); // label

    /*** pen element ***/
  
    stream.writeStartElement(xml_tags[XML_pen].tag);
    stream.writeAttribute(xml_tags[XML_colour].tag,
			  pc->pen ()->color ().name (QColor::HexArgb));
    stream.writeAttribute(xml_tags[XML_style].tag,
			  QString::number (pc->pen ()->style ()));
    stream.writeAttribute(xml_tags[XML_width].tag,
			  QString::number (pc->pen ()->widthF ()));
    stream.writeEndElement(); // pen

    stream.writeEndElement(); // stack
  }

  for (int i = 0; i < getPlotParameters ().size (); i++) {

    /*** parameter element ***/

    PlotParameter *pp = getPlotParameters ()[i];

    stream.writeStartElement(xml_tags[XML_parameter].tag);
    stream.writeAttribute(xml_tags[XML_index].tag,
			  QString::number (i));
    stream.writeAttribute(xml_tags[XML_real].tag,
			  QString::number (pp->real ()));
    stream.writeAttribute(xml_tags[XML_imag].tag,
			  QString::number (pp->imag ()));
    
    stream.writeCharacters(pp->vname ());

    stream.writeEndElement(); // parameter

  }

  for (int i = 0; i < getPlotLabelsSize (); i++) {
    PlotLabel *al = getPlotLabels ().at (i);
    dumpPlotLabel (i, al, stream);
  }


  stream.writeEndElement(); // qapl
  stream.writeEndDocument();
  file.close ();
}


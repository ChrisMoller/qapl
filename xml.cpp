#include "mainwindow.h"
#include "plot2dwindow.h"

#include "enums.h"

/***
                         // SETTINGS_PLOT_THEME
    <qapl resolution="..." theme="...">
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
      <active aspect="..." mode="...">
        <expression>....</expression>
        <label>....</label>
        <pen colour="..." style="..." width="..."/>
      </active>
      <stack index="..." aspect="..." mode="...">
        <expression>....</expression>
        <label>....</label>
        <pen colour="..." style="..." width="..."/>
      </stack>

      repeat stack...
      
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

bool Plot2DWindow::parseAxesLabel (QXmlStreamReader &stream,
				   Plot2dData *plot2DData)
{
  bool rc = false;
  bool run = true;
  while (run) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      plot2DData->axisLabelColour =
	QColor (attrs.value (xml_tags[XML_colour].tag));
      {
	QString fs (attrs.value (xml_tags[XML_font].tag).toString ());
	QFont ft;
	ft.fromString (fs);
	plot2DData->axisLabelFont = QFont (ft);
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
    default:
      break;
    }
  }
  
  return rc;
}

bool Plot2DWindow::parseAxesTitle (QXmlStreamReader &stream,
				   Plot2dData *plot2DData)
{
  bool rc = false;
  bool run = true;
  while (run) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      plot2DData->axisTitleColour =
	QColor (attrs.value (xml_tags[XML_colour].tag));
      {
	QString fs (attrs.value (xml_tags[XML_font].tag).toString ());
	QFont ft;
	ft.fromString (fs);
	plot2DData->axisTitleFont = QFont (ft);
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
    default:
      break;
    }
  }
  
  return rc;
}

bool Plot2DWindow::parsePen (QXmlStreamReader &stream,
				   Plot2dData *plot2DData)
{
  bool rc = false;
  bool run = true;
  while (run) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      QBrush brush (QColor (attrs.value (xml_tags[XML_colour].tag)));
      Qt::PenStyle style = (Qt::PenStyle)attrs.value (xml_tags[XML_style].tag).toInt ();
      double width = attrs.value (xml_tags[XML_width].tag).toFloat ();
      QPen pen (brush, width, style);
      plot2DData->activeCurve.setPen (pen);
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
    default:
      break;
    }
  }
  
  return rc;
}

bool Plot2DWindow::parseAxes (QXmlStreamReader &stream, Plot2dData *plot2DData)
{
  bool rc = false;
  bool run = true;
  while (run) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      plot2DData->axisColour =
	QColor (attrs.value (xml_tags[XML_colour].tag));
    }
    QXmlStreamReader::TokenType tt = stream.readNext ();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      switch (xmlhash.value (sn)) {
      case XML_label:
	parseAxesLabel (stream, plot2DData);
        break;
      case XML_title:
	parseAxesTitle (stream, plot2DData);
        break;
      default:
	fprintf (stderr, "unhandled axes value \"%s\"\n", toCString (sn));
	break;
      }
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

bool Plot2DWindow::parseRange (QXmlStreamReader &stream, Plot2dData *plot2DData)
{
  bool rc = false;
  bool run = true;
  while (run) {
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
	}
        break;
      default:
	fprintf (stderr, "unhandled range value \"%s\"\n", toCString (sn));
	break;
      }
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

bool Plot2DWindow::parseChart (QXmlStreamReader &stream,
			       Plot2dData *plot2DData)
{
  bool rc = false;
  bool run = true;
  while (run) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      plot2DData->chartTitleColour =
	QColor (attrs.value (xml_tags[XML_colour].tag));
      {
	QString fs (attrs.value (xml_tags[XML_font].tag).toString ());
	QFont ft;
	ft.fromString (fs);
	plot2DData->chartTitleFont = QFont (ft);
      }
    }
    QXmlStreamReader::TokenType tt = stream.readNext ();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      switch (xmlhash.value (sn)) {
      case XML_axes:
	parseAxes (stream, plot2DData);
        break;
      default:
	fprintf (stderr, "unhandled qapl value \"%s\"\n", toCString (sn));
	break;
      }
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

bool Plot2DWindow::parseActive (QXmlStreamReader &stream,
			       Plot2dData *plot2DData)
{
  bool rc = false;
  bool run = true;
  while (run) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      plot2DData->activeCurve.setAspect ((aspect_e)((attrs.value (xml_tags[XML_aspect].tag)).toInt ()));
      plot2DData->activeCurve.setMode ((series_mode_e)((attrs.value (xml_tags[XML_mode].tag)).toInt ()));
    }
    QXmlStreamReader::TokenType tt = stream.readNext ();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      switch (xmlhash.value (sn)) {
      case XML_expression:
	{
	  QString exp = stream.readElementText ();
	  fprintf (stderr, "doing expression \"%s\"\n", toCString (exp));
	  plot2DData->activeCurve.setExpression (exp);
	  fprintf (stderr, "check %s\n",
		   toCString (plot2DData->activeCurve.expression ()));
	}
        break;
      case XML_label:
	plot2DData->activeCurve.setTitle (stream.readElementText ());
        break;
      case XML_pen:
	parsePen (stream, plot2DData);
        break;
      default:
	fprintf (stderr, "unhandled active value \"%s\"\n", toCString (sn));
	break;
      }
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

bool Plot2DWindow::parseQapl (QXmlStreamReader &stream, Plot2dData *plot2DData)
{
  bool rc = false;
  bool run = true;
  while (run) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      plot2DData->resolution =
	(attrs.value (xml_tags[XML_resolution].tag)).toInt ();
      plot2DData->theme =
	(attrs.value (xml_tags[XML_theme].tag)).toInt ();
    }
    QXmlStreamReader::TokenType tt = stream.readNext ();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      switch (xmlhash.value (sn)) {
      case XML_chart:
	parseChart (stream, plot2DData);
        break;
      case XML_title:
	plot2DData->chartTitle = stream.readElementText ();
        break;
      case XML_index:
	plot2DData->indexVariable = stream.readElementText ();
        break;
      case XML_xlabel:
	plot2DData->xTitle = stream.readElementText ();
        break;
      case XML_ylabel:
	plot2DData->yTitle = stream.readElementText ();
        break;
      case XML_range:
	parseRange (stream, plot2DData);
        break;
      case XML_active:
	parseActive (stream, plot2DData);
        break;
      default:
	fprintf (stderr, "unhandled qapl value \"%s\"\n", toCString (sn));
	break;
      }
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

void Plot2DWindow::readXML (QString &fileName, MainWindow *mw)
{
  static bool xmlInitialised = false;
  if (!xmlInitialised) {
    for (long unsigned int i = 0; i < XML_LAST; i++)
      xmlhash.insert (xml_tags[i].tag, (int)i);
    xmlInitialised = true;
  }

  Plot2dData *plot2DData =  new Plot2dData (mw);
  
  QFile file (fileName);
  file.open (QIODevice::ReadOnly | QIODevice::Text);
  QXmlStreamReader stream(&file);

  bool run = true;
  while (run) {
    QXmlStreamReader::TokenType tt = stream.readNext();
    QString sn = stream.name ().toString ();
    switch (tt) {
    case QXmlStreamReader::StartElement:
      switch (xmlhash.value (sn)) {
      case XML_qapl:
	parseQapl (stream, plot2DData);
        break;
      case XML_chart:
	fprintf (stderr, "chart\n");
        break;
      case XML_label:
	fprintf (stderr, "label\n");
        break;
      case XML_title:
	fprintf (stderr, "title\n");
        break;
      default:
	fprintf (stderr, "unhandled top value \"%s\"\n", toCString (sn));
	break;
      }
      break;
    case QXmlStreamReader::EndElement:
      fprintf (stderr, "ee\n");
      run = false;
      break;
    case QXmlStreamReader::EndDocument:
      fprintf (stderr, "ed\n");
      new Plot2DWindow (mw, plot2DData);
      run = false;
      break;
    case QXmlStreamReader::Invalid:
      break;
    default:
      // fprintf (stderr, "unhandled ttt %d \"%s\"\n", tt, toCString (sn));
      break;
    }
  }
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

  /*** expression element ***/
  
  stream.writeStartElement(xml_tags[XML_expression].tag);
  stream.writeCharacters(getAplExpression ());
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

    /*** expression element ***/
  
    stream.writeStartElement(xml_tags[XML_expression].tag);
    stream.writeCharacters(pc->expression ());
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


  stream.writeEndElement(); // qapl
  stream.writeEndDocument();
  file.close ();
}


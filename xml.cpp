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
        <pen colour="..." style="..."/>
      </active>
      <stack index="..." aspect="..." mode="...">
        <expression>....</expression>
        <label>....</label>
        <pen colour="..." style="..."/>
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

bool Plot2DWindow::parseQapl (QXmlStreamReader &stream)
{
  bool rc = false;
  bool run = true;
  while (run) {
    QXmlStreamAttributes attrs = stream.attributes();
    if (!attrs.isEmpty ()) {
      int resolution = (attrs.value (xml_tags[XML_resolution].tag)).toInt ();
      int theme      = (attrs.value (xml_tags[XML_theme].tag)).toInt ();
      fprintf (stderr, "res = %d, theme = %d\n", resolution, theme);
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
	parseQapl (stream);
        break;
      case XML_chart:
	fprintf (stderr, "chart\n");
        break;
      case XML_axes:
	fprintf (stderr, "axes\n");
        break;
      case XML_label:
	fprintf (stderr, "label\n");
        break;
      case XML_title:
	fprintf (stderr, "title\n");
        break;
      default:
	fprintf (stderr, "unhandled value \"%s\"\n", toCString (sn));
	break;
      }
      break;
    case QXmlStreamReader::EndElement:
      run = false;
      break;
    case QXmlStreamReader::EndDocument:
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
    stream.writeEndElement(); // pen

    stream.writeEndElement(); // stack
  }


  stream.writeEndElement(); // qapl
  stream.writeEndDocument();
  file.close ();
}


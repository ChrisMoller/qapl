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
			QString::number (resolution));
  stream.writeAttribute(xml_tags[XML_theme].tag,
			QString::number (theme));

  /*** chart element ***/
  
  stream.writeStartElement(xml_tags[XML_chart].tag);
  stream.writeAttribute(xml_tags[XML_font].tag, chartTitleFont.toString ());
  stream.writeAttribute(xml_tags[XML_colour].tag,
			chartTitleColour.name (QColor::HexArgb));

  /*** axes element ***/

  stream.writeStartElement(xml_tags[XML_axes].tag);
  stream.writeAttribute(xml_tags[XML_colour].tag,
			axisColour.name (QColor::HexArgb));

  /*** axis label element ***/
  
  stream.writeStartElement(xml_tags[XML_label].tag);
  stream.writeAttribute(xml_tags[XML_font].tag, axisLabelFont.toString ());
  stream.writeAttribute(xml_tags[XML_colour].tag,
			axisLabelColour.name (QColor::HexArgb));
  stream.writeEndElement(); // axis label

  /*** axis title element ***/
  
  stream.writeStartElement(xml_tags[XML_title].tag);
  stream.writeAttribute(xml_tags[XML_font].tag, axisTitleFont.toString ());
  stream.writeAttribute(xml_tags[XML_colour].tag,
			axisTitleColour.name (QColor::HexArgb));
  stream.writeEndElement(); // axis title

  stream.writeEndElement(); // axes
  
  stream.writeEndElement(); // chart

  /*** chart title element ***/
  
  stream.writeStartElement(xml_tags[XML_title].tag);
  stream.writeCharacters(chartTitle);
  stream.writeEndElement(); // chart title

  /*** chart index var element ***/
  
  stream.writeStartElement(xml_tags[XML_index].tag);
  stream.writeCharacters(indexVariable->text ());
  stream.writeEndElement(); // chart index var
  
  /*** chart x label element ***/
  
  stream.writeStartElement(xml_tags[XML_xlabel].tag);
  stream.writeCharacters(xTitle);
  stream.writeEndElement(); // chart x label
  
  /*** chart y label element ***/
  
  stream.writeStartElement(xml_tags[XML_ylabel].tag);
  stream.writeCharacters(yTitle);
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
  stream.writeCharacters(aplExpression->text ());
  stream.writeEndElement(); // expression

  /*** label element ***/
  
  stream.writeStartElement(xml_tags[XML_label].tag);
  stream.writeCharacters(curveTitle->text ());
  stream.writeEndElement(); // label

  /*** pen element ***/
  
  stream.writeStartElement(xml_tags[XML_pen].tag);
  stream.writeAttribute(xml_tags[XML_colour].tag,
			activePen.color ().name (QColor::HexArgb));
  stream.writeAttribute(xml_tags[XML_style].tag,
			QString::number (activePen.style ()));
  stream.writeEndElement(); // pen

  stream.writeEndElement(); // active

  for (int i = 0; i < plotCurves.size (); i++) {
  }


  stream.writeEndElement(); // qapl
  stream.writeEndDocument();
  file.close ();
}


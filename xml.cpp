#include "mainwindow.h"
#include "plot2dwindow.h"

#include "enums.h"

/***
    <qapl resolution="..." theme="...">
      <title>....</title>
      <index>....</index>
      <xlabel>....</xlabel>
      <ylabel>....</ylabel>
      <range format="..." init="..." final="..."/>
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

  stream.writeStartElement(xml_tags[XML_qapl].tag);


  stream.writeEndElement(); // qvis
  stream.writeEndDocument();
  file.close ();
}


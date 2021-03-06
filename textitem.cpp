//  https://stackoverflow.com/questions/42941508/display-text-on-qtcharts
// courtesy of Kto To

#include "textitem.h"
#include <QtCharts/QChart>
#include <QPainter>
#include <QRect>

#include "chart2dwindow.h"

#if 0
TextItem::TextItem(QString text, QPoint pos, bool world, QChart *chart,
		   QAbstractSeries *series)
  : QGraphicsItem(chart), _chart(chart), _series(series), _anchor(pos),
      _world (world)
{
  //    setText(text);
}
#endif

TextItem::TextItem(QChart *chart, QAbstractSeries *series)
    : QGraphicsItem(chart), _chart(chart), _series(series) {
  //    setText(text);
}

QRectF TextItem::boundingRect() const {
    QPointF anchor = mapFromParent(_chart->mapToPosition(_anchor, _series));
    QRectF rect;
    rect.setLeft(qMin(_textRect.left(), anchor.x()));
    rect.setRight(qMax(_textRect.right(), anchor.x()));
    rect.setTop(qMin(_textRect.top(), anchor.y()));
    rect.setBottom(qMax(_textRect.bottom(), anchor.y()));
    return rect;
}

void TextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		     QWidget *widget) {
  Q_UNUSED(option);
  Q_UNUSED(widget);

#if 1
  painter->setFont (_font);
#else
  fprintf (stderr, "fontScale = %g\n", _fontScale);
  if (_fontScale != 1.0) {
    QFont ufont = _font;
    QFont tfont = Chart2DWindow::scaleFont (_fontScale, ufont);
    fprintf (stderr, "tfont \"%s\"\n", toCString (tfont.toString ()));
    painter->setFont (tfont);
  }
  else {
    painter->setFont (_font);
    fprintf (stderr, "_font \"%s\"\n", toCString (_font.toString ()));
  }
#endif
      
  painter->setPen (_colour);

  painter->save();


  /***
      The horizontal flags are:

      Qt::AlignLeft		x += 0
      Qt::AlignRight		x += right
      Qt::AlignHCenter		x += (right - left) / 2
      Qt::AlignJustify		x += (right - left) / 2

      The vertical flags are:

      Qt::AlignTop		y += top
      Qt::AlignBottom		y += bottom
      Qt::AlignVCenter		y += (bottom - top) / 2
      Qt::AlignBaseline		y += 0;

      |¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨|
      |                          |
      |                          |
      ○--------------------------|
      |__________________________|
      
  ***/
  
  QPointF anchor;
  QPointF delta (0.0, 0.0);
  switch(_halign) {
  case Qt::AlignLeft:
    delta.setX (0.0);
    break;
  case Qt::AlignRight:
    delta.setX (_textRect.right ());
    break;
  case Qt::AlignHCenter:
    delta.setX ((_textRect.right () - _textRect.left ()) / 2.0);
    break;
  case Qt::AlignJustify:
    delta.setX ((_textRect.right () - _textRect.left ()) / 2.0);
    break;
  }
  switch(_valign) {
  case Qt::AlignTop:
    delta.setY (_textRect.top ());
    break;
  case Qt::AlignBottom:
    delta.setY (_textRect.bottom ());
    break;
  case Qt::AlignVCenter:
    delta.setY ((_textRect.top () - _textRect.bottom ()) / 2.0);
    break;
  case Qt::AlignJustify:
    delta.setY (0.0);
    break;
  }
  if (_world) {
    QPointF im = _chart->mapToPosition(_anchor, _series);
    painter->translate (im);
    painter->rotate (_angle);
    painter->translate (-im);
    painter->translate (-delta);
    anchor = im;
  }
  else {			//screen
    painter->translate (_anchor);
    painter->rotate (_angle);
    painter->translate (-_anchor);
    painter->translate (-delta);
    anchor = _anchor;
    
  }

  painter->drawText(anchor, _text);

  painter->restore();
}

void TextItem::setText(const QString &text, QPointF pos, bool world)
{
  _text = text;
  _anchor = pos;
  _world = world;
  QFontMetrics metrics(_font);
  
  _textRect = metrics.boundingRect (_text);
  prepareGeometryChange();
}


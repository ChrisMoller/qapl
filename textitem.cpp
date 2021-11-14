//  https://stackoverflow.com/questions/42941508/display-text-on-qtcharts
// courtesy of Kto To

#include "textitem.h"
#include <QtCharts/QChart>
#include <QPainter>
#include <QRect>

TextItem::TextItem(QString text, QPoint pos, QChart *chart,
		   QAbstractSeries *series)
    : QGraphicsItem(chart), _chart(chart), _series(series), _anchor(pos) {
    setText(text);
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
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QFont font = painter->font();
    font.setPixelSize(24);
    font.setFamily ("Serif");
    font.setItalic (true);
    painter->setFont(font);
    painter->setPen (QColor ("red"));
    painter->save();
    //    painter->rotate (10);

#if 0
    QPointF anchor = _anchor;
#else
    QPointF anchor = mapFromParent(_chart->mapToPosition(_anchor, _series));
#endif
    painter->drawText(anchor, _text);

    painter->restore();
}

void TextItem::setText(const QString &text) {
  _text = text;
  QFontMetrics metrics((QFont()));
  _textRect = metrics.boundingRect(QRect(0, 0, 150, 150),
                                   Qt::AlignLeft, _text);
  _textRect.translate(5, 5);
  prepareGeometryChange();
}

void TextItem::setAnchor(QPointF point) { _anchor = point; }

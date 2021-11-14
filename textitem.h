#pragma once
#include <QtWidgets>
#include <QtCharts/QChartGlobal>
#include <QtWidgets/QGraphicsItem>

//QT_CHARTS_BEGIN_NAMESPACE
class QChart;
class QAbstractSeries;
//QT_CHARTS_END_NAMESPACE

//QT_CHARTS_USE_NAMESPACE


class TextItem : public QGraphicsItem {
public:
#if 0
  TextItem(QString text, QPoint pos, bool world,
	   QChart *chart, QAbstractSeries *series);
  TextItem (QChart *chart, QAbstractSeries *series)
    : QGraphicsItem(chart), _chart(chart), _series(series) {
  }
#endif
  TextItem (QChart *chart, QAbstractSeries *series);
  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	     QWidget *widget) override;

  void setText(const QString &text, QPointF pos, bool world);
  void setFont(const QFont &font) { _font = font; }
  void setColour(const QColor &colour) { _colour = colour; }
  void setAngle(const double angle) { _angle = angle; }
  void setAlignment(const int halign, int valign) {
    _halign = halign;  _valign = valign; }
  void setAnchor(QPointF point) { _anchor = point; }
private:
  QChart *_chart;
  QAbstractSeries *_series;
  QString _text;
  QRectF _textRect;
  QPointF _anchor;
  bool _world;
  QFont _font;
  QColor _colour;
  double _angle;
  int _halign;
  int _valign;
};

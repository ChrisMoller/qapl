#pragma once

#include <QtCharts/QChartGlobal>
#include <QtWidgets/QGraphicsItem>

//QT_CHARTS_BEGIN_NAMESPACE
class QChart;
class QAbstractSeries;
//QT_CHARTS_END_NAMESPACE

//QT_CHARTS_USE_NAMESPACE


class TextItem : public QGraphicsItem {
public:
    TextItem(QString text, QPoint pos, QChart *chart, QAbstractSeries *series);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	       QWidget *widget) override;

    void setText(const QString &text);
    void setAnchor(QPointF point);
private:
    QChart *_chart;
    QAbstractSeries *_series;
    QString _text;
    QRectF _textRect;
    QPointF _anchor;
};

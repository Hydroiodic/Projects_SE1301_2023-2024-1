// QLinkThread.h
// header file for QLinkThread.cpp

#pragma once

#include <QObject>
#include <QList>
#include <QColor>
#include <QPainter>
#include <QRegion>
#include "QLinkSettings.h"

class QLinkThread : public QObject
{
	Q_OBJECT

public:
	QLinkThread(QObject* parent = nullptr);
	~QLinkThread();

	QPainter* painter;
	QRegion _old_region;

	const double _game_width_ratio = 4.0 / 5;

	QPolygon drawOneLine(int x1, int y1, int x2, int y2, double _pen_width, 
		const QColor& color, QPaintDevice* device);
	QRegion drawAllLines(const QList<QPoint>& list, double _pen_width, 
		const QColor& color, QPaintDevice* device);

public slots:
	void paintPixmap(int width, int height, double _pen_width, QList<QPair<QList<QPoint>, QColor>> list);

signals:
	void sendPixmap(QPixmap map, QRegion region);
};

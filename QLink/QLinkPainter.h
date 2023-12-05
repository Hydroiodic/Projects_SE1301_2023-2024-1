// QLinkPainter.h
// header file for QLinkPainter.cpp

#pragma once

#include <QPainter>
#include "QLinkSettings.h"

class QLinkPainter : public QPainter
{
private:
	const double _interval_width = 1.0;
	const double _max_transparency = 255.0;

	double _painter_width = 1.0;
	QColor color;

public:
	QLinkPainter(QPaintDevice* parent);
	~QLinkPainter();

	// basic painting methods
	void drawOneLine(int x1, int y1, int x2, int y2);
	void drawAllLines(const QList<QPoint>& list);

	// basic setting methods
	void setWidth(double _width_);
	void setColor(const QColor& _color_);
};

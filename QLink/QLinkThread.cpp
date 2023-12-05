// QLinkThread.cpp
// provide the method of multi-threads for the program

#include "QLinkThread.h"

QLinkThread::QLinkThread(QObject* parent) : QObject(parent) {
	painter = new QPainter;
}

QLinkThread::~QLinkThread() {
	delete painter;
}

// QLinkThread::paintPixmap, to give a pixmap for painting
void QLinkThread::paintPixmap(int width, int height, double _pen_width, QList<QPair<QList<QPoint>, QColor>> list) {
	QPixmap map(width, height);
	map.fill(Qt::transparent);

	// draw the lines in the list
	QRegion region;
	for (int i = 0; i < list.size(); ++i) {
		QPair<QList<QPoint>, QColor> lines = list[i];
		region = region.united(drawAllLines(lines.first, _pen_width, lines.second, &map));
	}

	// draw the info line
	region = region.united(drawOneLine(width * _game_width_ratio, 0, width * 
		_game_width_ratio, height, 10.0, Qt::black, &map));

	// deal with _old_region
	emit sendPixmap(map, region.united(_old_region));
	_old_region = region;
}

// QLinkPainter::drawOneLine, draw a more beautiful line
QPolygon QLinkThread::drawOneLine(int x1, int y1, int x2, int y2, 
	double _pen_width, const QColor& color, QPaintDevice* device) {

	static const double _interval_width = 1.0;
	static const double _max_transparency = 255.0;

	int red = color.red(), green = color.green(), blue = color.blue();
	double width = _pen_width, _half_width = width / 2.0;

	double m = 0.0, sin = 0.0, cos = 0.0;
	if (y1 != y2) {
		m = static_cast<double>(x2 - x1) / (y2 - y1);
		sin = 1.0 / sqrt(m * m + 1);
		cos = m / sqrt(m * m + 1);
	}
	else { sin = 0; cos = 1; }

#ifndef MORE_BEAUTIFUL_LINE
	painter->begin(device);

	QPen _current_pen(color, _pen_width);
	painter->setPen(_current_pen);

	painter->drawLine(x1, y1, x2, y2);
	painter->end();
#else
	painter->begin(device);
	for (double pos = 0.0; pos <= _half_width; pos += _interval_width) {
		QPen _current_pen(QColor(red, green, blue, _max_transparency / _half_width * (_half_width - pos)));
		_current_pen.setWidthF(_interval_width);

		painter->setPen(_current_pen);
		painter->drawLine(x1 + pos * sin, y1 - pos * cos, x2 + pos * sin, y2 - pos * cos);
		painter->drawLine(x1 - pos * sin, y1 + pos * cos, x2 - pos * sin, y2 + pos * cos);
	}
	painter->end();
#endif // !MORE_BEAUTIFUL_LINE

	// to increase the accuracy of updating
	_half_width *= 1.1;

	// set the area to update
	QPolygon polygon;
	int points[8]{ x1 - _half_width * sin, y1 + _half_width * cos, 
		x1 + _half_width * sin, y1 - _half_width * cos, x2 + _half_width * sin, y2 - _half_width * cos, 
		x2 - _half_width * sin, y2 + _half_width * cos };
	polygon.setPoints(4, points);

	return polygon;
}

// QLinkPainter::drawAllLines, provide a method to draw lines
QRegion QLinkThread::drawAllLines(const QList<QPoint>& list, 
	double _pen_width, const QColor& color, QPaintDevice* device) {
	int size = list.size();
	QRegion region;

	for (int i = 1; i <= size / 2; ++i) {
		QPoint _first_point = list[(i << 1) - 2], _second_point = list[(i << 1) - 1];
		region = region.united(drawOneLine(_first_point.x(), _first_point.y(), 
			_second_point.x(), _second_point.y(), _pen_width, color, device));
	}
	return region;
}

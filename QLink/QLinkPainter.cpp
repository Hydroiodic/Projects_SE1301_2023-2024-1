// QLinkPainter.cpp
// to provide a more beautiful line-drawing method for this program

#include "QLinkPainter.h"

QLinkPainter::QLinkPainter(QPaintDevice* parent) : QPainter(parent) {
	/* there's nothing more to do actually */
}

QLinkPainter::~QLinkPainter() {
	/* there's nothing more to do actually */
}

// QLinkPainter::drawOneLine, draw a more beautiful line
void QLinkPainter::drawOneLine(int x1, int y1, int x2, int y2) {
#ifndef MORE_BEAUTIFUL_LINE
	this->begin(device());

	QPen _current_pen(color, this->_painter_width);
	this->setPen(_current_pen);

	drawLine(x1, y1, x2, y2);
	this->end();

	return;
#endif // !MORE_BEAUTIFUL_LINE

	int red = color.red(), green = color.green(), blue = color.blue();
	double width = this->_painter_width, _half_width = width / 2.0;

	double m = 0.0, sin = 0.0, cos = 0.0;
	if (y1 != y2) {
		m = static_cast<double>(x2 - x1) / (y2 - y1);
		sin = 1.0 / sqrt(m * m + 1);
		cos = m / sqrt(m * m + 1);
	}
	else { sin = 0; cos = 1; }

	this->begin(device());
	for (double pos = 0.0; pos <= _half_width; pos += _interval_width) {
		QPen _current_pen(QColor(red, green, blue, _max_transparency / _half_width * (_half_width - pos)));
		_current_pen.setWidthF(_interval_width);

		this->setPen(_current_pen);
		this->drawLine(x1 + pos * sin, y1 - pos * cos, x2 + pos * sin, y2 - pos * cos);
		this->drawLine(x1 - pos * sin, y1 + pos * cos, x2 - pos * sin, y2 + pos * cos);
	}
	this->end();
}

// QLinkPainter::drawAllLines, provide a method to draw lines
void QLinkPainter::drawAllLines(const QList<QPoint>& list) {
#ifndef MORE_BEAUTIFUL_LINE
	this->begin(device());

	QPen _current_pen(color, this->_painter_width);
	this->setPen(_current_pen);

	drawLines(list);
	this->end();

	return;
#endif // !MORE_BEAUTIFUL_LINE

	int size = list.size();
	
	for (int i = 1; i <= size / 2; ++i) {
		QPoint _first_point = list[(i << 1) - 2], _second_point = list[(i << 1) - 1];
		drawOneLine(_first_point.x(), _first_point.y(), _second_point.x(), _second_point.y());
	}
}

// QLinkPainter::setWidth, to change the width of the painter
void QLinkPainter::setWidth(double _width_) {
	_painter_width = _width_;
}

// QLinkPainter::setColor, to change the color of the painter
void QLinkPainter::setColor(const QColor& _color_) {
	color = _color_;
}

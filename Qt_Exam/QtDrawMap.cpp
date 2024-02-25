// QtDrawMap.cpp
// provide a way to draw a grid

#include "QtDrawMap.h"

QtDrawMap::QtDrawMap(QObject* parent, const QDrawMapSettings& settings)
	: QObject(parent), settings(settings) {
	// create the instance of the QPainter
	painter = new QPainter;
}

QtDrawMap::~QtDrawMap() {
	// delete the instance of the painter
	delete painter;
}

void QtDrawMap::config(const QDrawMapSettings& settings_to_set) {
	// resize the size of this class
	settings = settings_to_set;
}

void QtDrawMap::paintBg(QPixmap& map, size_t width, size_t height) const {
	// the width and height of a single square
	double max_width = static_cast<double>(width - (settings.padding << 1)) / settings.N;
	double max_height = static_cast<double>(height - (settings.padding << 1)) / settings.M;
	double block_width = qMin(max_width, max_height);

	// calculate for the start position for painting
	double x_start = static_cast<double>(width) / 2 - 
		static_cast<double>(settings.N) * block_width / 2;
	double y_start = static_cast<double>(height) / 2 - 
		static_cast<double>(settings.M) * block_width / 2;
	double x_end = x_start + settings.N * block_width;
	double y_end = y_start + settings.M * block_width;

	// start painting
	painter->begin(&map);

	// set the style of the painter
	painter->setPen(QPen(settings.bgLineColor, settings.painter_size));

	// the vertical lines
	for (int i = 0; i <= settings.N; ++i) {
		painter->drawLine(QPointF(x_start + block_width * i, y_start),
			QPointF(x_start + block_width * i, y_end));
	}

	// the horizontal lines
	for (int j = 0; j <= settings.M; ++j) {
		painter->drawLine(QPointF(x_start, y_start + block_width * j),
			QPointF(x_end, y_start + block_width * j));
	}

	// finish painting
	painter->end();
}

void QtDrawMap::paintBlock(QPixmap& map, size_t width, size_t height,
	const QList<QBlockData>& blocks_to_paint) const {
	// the width and height of a single square
	double max_width = static_cast<double>(width - (settings.padding << 1)) / settings.N;
	double max_height = static_cast<double>(height - (settings.padding << 1)) / settings.M;
	double block_width = qMin(max_width, max_height);

	// calculate for the start position for painting
	double x_start = static_cast<double>(width) / 2 -
		static_cast<double>(settings.N) * block_width / 2;
	double y_start = static_cast<double>(height) / 2 -
		static_cast<double>(settings.M) * block_width / 2;

	// start painting
	painter->begin(&map);

	// paint blocks one by one
	{
		// this is a temp variable
		QBlockData pos;

		// use Qt-style foreach to iterate
		foreach(pos, blocks_to_paint) {
			QRectF rect_to_paint(x_start + pos.x * block_width, y_start +
				pos.y * block_width, block_width, block_width);

			// set the style of the painter and then paint for it
			painter->setBrush(QBrush(settings.colors[pos.color]));
			painter->drawRect(rect_to_paint);

			if (pos.valueShown) {
				painter->setFont(QFont("ËÎÌו", 10, QFont::Bold, false));
				painter->drawText(rect_to_paint, Qt::AlignCenter | Qt::AlignHCenter,
					QString::number(pos.value));
			}
		}
	}

	// finish painting
	painter->end();
}

void QtDrawMap::drawPixmap(size_t width, size_t height, QList<QBlockData> list) const {
	// the canvas for painting
	QPixmap map(width, height);
	map.fill(Qt::transparent);

	// paint the background lines
	paintBg(map, width, height);

	// paint the blocks
	paintBlock(map, width, height, list);

	// send signals
	emit sendPixMap(map);
}

// define an instance of QtDrawMap
QtDrawMap qdraw_map(nullptr);

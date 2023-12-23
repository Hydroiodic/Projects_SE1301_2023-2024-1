// QtDrawMap.h
// header file for QtDrawMap.cpp

#pragma once

#include <QObject>
#include <QPainter>
#include <QPixmap>

enum BlockColor {
	GREEN, RED, BLUE
};

struct QBlockData {
	int x, y;
	int value;
	bool valueShown = false;
	BlockColor color;
};

struct QDrawMapSettings {
	size_t M = 10, N = 10;
	size_t painter_size = 1;
	size_t padding = 10;

	QColor bgLineColor = QColor(0, 0, 0);
	QColor colors[3] = { Qt::green, Qt::red, Qt::blue };
};

class QtDrawMap : public QObject
{
	Q_OBJECT

public:
	QtDrawMap(QObject* parent, const QDrawMapSettings& settings = QDrawMapSettings());
	~QtDrawMap();

private:
	QDrawMapSettings settings;

	// the painter to use
	QPainter* painter = nullptr;

public:
	void config(const QDrawMapSettings& settings);

private:
	void paintBg(QPixmap& map, size_t width, size_t height) const;
	void paintBlock(QPixmap& map, size_t width, size_t height,
		const QList<QBlockData>& blocks_to_paint) const;

public slots:
	void drawPixmap(size_t width, size_t height, QList<QBlockData> list) const;

signals:
	void sendPixMap(const QPixmap map) const;
};

extern QtDrawMap qdraw_map;

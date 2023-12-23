#pragma once

#include <QtWidgets/QMainWindow>
#include <QThread>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QMenuBar>
#include <QMenu>
#include <QQueue>
#include <QAction>
#include <fstream>
#include "ui_Qt_Exam.h"

#include "options.h"

#include "QtDrawMap.h"

class Qt_Exam : public QMainWindow
{
	Q_OBJECT

public:
	Qt_Exam(QWidget* parent = nullptr);
	~Qt_Exam();

private:
	void initializeTricks();
	bool initialized = false;

protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;

private:
	Ui::Qt_ExamClass ui;

private:
	QThread* thread = nullptr;

private:
	void initializeMenuBar();

private:
	// here use an array to store the data
	const static size_t MAX_M = 15, MAX_N = 15;
	int data[MAX_M][MAX_N] = {};
	int safety[MAX_M][MAX_N] = {};
	int min_safety = -1;

	size_t M = -1, N = -1;

private:
	bool safety_shown = false;

	void prepareUI();
	void resizeAll() const;

private:
	void loadFromFile();

private:
	void Warning(const QString& msg) const;

private:
	void prepareSafetyValue();
	void findPath();
	bool recordPath(int i, int j, QList<QPair<int, int>>& list) const;

	/********** functions for QtDrawMap **********/

private:
	QPixmap background;
	QList<QBlockData> blocks_list;

private slots:
	void setBg(const QPixmap map);

signals:
	void requestBg(size_t width, size_t height, QList<QBlockData> list);
};

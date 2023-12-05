// QLinkGameover.h
// header file for QLinkGameover.cpp

#pragma once

#include <QWidget>
#include "ui_QLinkGameover.h"
#include "QLink.h"

class QLink;

class QLinkGameover : public QWidget
{
	Q_OBJECT

public:
	QLinkGameover(QLink* main, QWidget* parent = nullptr);
	~QLinkGameover();

	void setResults(int result, int score);

private:
	Ui::QLinkGameover ui;
	QLink* _main_window;

private slots:
	void clickOk();
	void clickContinue();
};

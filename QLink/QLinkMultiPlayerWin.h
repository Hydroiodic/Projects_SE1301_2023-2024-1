// QLinkMultiPlayerWin.h
// header file for QLinkMultiPlayerWin.cpp

#pragma once

#include <QWidget>
#include "ui_QLinkMultiPlayerWin.h"

class QLinkMultiPlayerWin : public QWidget
{
	Q_OBJECT

public:
	QLinkMultiPlayerWin(QWidget* parent);
	~QLinkMultiPlayerWin();

	void setWinners(const QString& name);

private:
	Ui::QLinkMultiPlayerWin ui;
};

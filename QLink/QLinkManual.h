// QLinkManual.h
// header file for QLinkManual.cpp

#pragma once

#include <QWidget>
#include <QCloseEvent>
#include "ui_QLinkManual.h"

class QLinkManual : public QWidget
{
	Q_OBJECT

private:
	Ui::QLinkManual ui;

	void closeEvent(QCloseEvent* event) override;

public:
	QLinkManual(QWidget* parent);
	~QLinkManual();
};

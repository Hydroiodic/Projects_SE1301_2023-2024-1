#pragma once

#include <QWidget>
#include <QCloseEvent>
#include "ui_QBasicHelp.h"

class QBasicHelp : public QWidget
{
	Q_OBJECT

public:
	QBasicHelp(QWidget* parent = nullptr);
	~QBasicHelp();

public:
	// rewrite the function "closeEvent" to not close the window actually
	void closeEvent(QCloseEvent* event) override;

private:
	Ui::QBasicHelp ui;
};

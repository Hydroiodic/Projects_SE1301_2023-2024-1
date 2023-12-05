// QLinkInit.h
// header file for QLinkInit.cpp

#pragma once

#include <QWidget>
#include "ui_QLinkInit.h"
#include "QLink.h"

class QLink;

class QLinkInit : public QWidget
{
	Q_OBJECT

public:
	QLinkInit(QWidget* parent);
	~QLinkInit();

private:
	Ui::QLinkInit ui;

private slots:
	void startNewGame();
	void changeWidthValue(int width);
	void changeHeightValue(int height);
	void changeIconKind(int kind);
};

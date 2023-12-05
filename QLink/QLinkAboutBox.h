// QLinkAboutBox.h
// header file for QLinkAboutBox.cpp

#pragma once

#include <QWidget>
#include "ui_QLinkAboutBox.h"

class QLinkAboutBox : public QWidget
{
	Q_OBJECT

public:
	QLinkAboutBox(QWidget* parent);
	~QLinkAboutBox();

private:
	Ui::QLinkAboutBox ui;
};

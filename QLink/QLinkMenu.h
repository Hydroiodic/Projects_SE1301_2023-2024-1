// QLinkMenu.h
// header file for QLinkMenu.cpp

#pragma once

#include <QMenu>
#include <QKeyEvent>
#include "QLink.h"

class QLinkMenu : public QMenu
{
	Q_OBJECT

public:
	QLinkMenu(QWidget* parent = nullptr);
	~QLinkMenu();

	void setMainWindow(QLink* _main_window);

protected:
	void keyReleaseEvent(QKeyEvent* event);

private:
	QLink* _main_window = nullptr;
};

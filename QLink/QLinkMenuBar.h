// QLinkMenuBar.h
// header file for QLinkMenuBar.cpp

#pragma once

#include <QMenuBar>
#include <QAction>
#include "QLinkMenu.h"
#include "QLink.h"

class QLink;
class QLinkMenu;

class QLinkMenuBar : public QMenuBar
{
private:
	QLinkMenu* _menu;
	QAction* _action;

	// Below is the count of menu and action. When adding them, please change these variables
	int _menu_count = 2;
	int _action_count = 6;

public:
	QLinkMenuBar(QLink* parent);
	~QLinkMenuBar();

	void initMenu();
	void newGame();
	void showManual();
};

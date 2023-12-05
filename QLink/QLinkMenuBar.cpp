// QLinkMenuBar.cpp
// to create the menu for the main window

#include "QLinkMenuBar.h"

QLinkMenuBar::QLinkMenuBar(QLink* parent) : QMenuBar(parent) {
	_menu = new QLinkMenu[_menu_count];
	_action = new QAction[_action_count];

	for (int i = 0; i < _menu_count; ++i)
		_menu[i].setMainWindow(parent);

	initMenu();
}

QLinkMenuBar::~QLinkMenuBar() {
	delete[] _menu;
	delete[] _action;
}

// QLinkMenuBar::initMenu, to create the menubar and add it to the main window
void QLinkMenuBar::initMenu() {
	// _menu[0], _action[0]
	connect(&_action[0], &QAction::triggered, this, &QLinkMenuBar::newGame);
	_action[0].setText("New game(&N)");
	_action[0].setShortcut(Qt::CTRL | Qt::Key_N);

	// _menu[0], _action[1]
	connect(&_action[1], &QAction::triggered, (QLink*)parentWidget(), &QLink::switchGameMode);
	_action[1].setText("Switch game mode(&G)");
	_action[1].setShortcut(Qt::CTRL | Qt::Key_G);

	// _menu[0], _action[2]
	connect(&_action[2], &QAction::triggered, (QLink*)parentWidget(), &QLink::saveToFile);
	_action[2].setText("Save to...(&S)");
	_action[2].setShortcut(Qt::CTRL | Qt::Key_S);

	// _menu[0], _action[3]
	connect(&_action[3], &QAction::triggered, (QLink*)parentWidget(), &QLink::loadFromFile);
	_action[3].setText("Load game from...(&L)");
	_action[3].setShortcut(Qt::CTRL | Qt::Key_L);

	// _menu[1], _action[4]
	connect(&_action[4], &QAction::triggered, this, &QLinkMenuBar::showManual);
	_action[4].setText("Manual(&M)");
	_action[4].setShortcut(Qt::CTRL | Qt::Key_M);

	// _menu[1], _action[5]
	connect(&_action[5], &QAction::triggered, (QLink*)parentWidget(), &QLink::createAboutBox);
	_action[5].setText("About(&A)");
	_action[5].setShortcut(Qt::CTRL | Qt::Key_A);

	_menu[0].addAction(&_action[0]);
	_menu[0].addAction(&_action[1]);
	_menu[0].addSeparator();
	_menu[0].addAction(&_action[2]);
	_menu[0].addAction(&_action[3]);

	_menu[1].addAction(&_action[4]);
	_menu[1].addSeparator();
	_menu[1].addAction(&_action[5]);

	_menu[0].setTitle("Game(&G)");
	_menu[1].setTitle("Help(&H)");

	// TODO: a lot more menus and actions
	addMenu(&_menu[0]);
	addMenu(&_menu[1]);
}

// QLinkMenuBar::newGame, to call some functions of the main window for a new game to start
void QLinkMenuBar::newGame() {
	((QLink*)parentWidget())->_init_window->show();
	((QLink*)parentWidget())->_init_window->raise();
}

// QLinkMenuBar::showManual, to show the manual
void QLinkMenuBar::showManual() {
	if (!((QLink*)parentWidget())->_game_stopped)
		((QLink*)parentWidget())->pauseGame();

	((QLink*)parentWidget())->manual->show();
}

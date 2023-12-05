// QLinkMenu.cpp
// to solve the problem that QMenu don't have a overloading of keyReleaseEvent

#include "QLinkMenu.h"

QLinkMenu::QLinkMenu(QWidget* parent) : QMenu(parent) {
	/* there's nothing more to do */
}

QLinkMenu::~QLinkMenu() {
	/* there's nothing more to do */
}

// QLinkMenu::setMainWindow, to set the value of _main_window
void QLinkMenu::setMainWindow(QLink* _main_window) {
	this->_main_window = _main_window;
}

// QLinkMenu::keyReleaseEvent, it's strange that QMenu don't have a overloading of keyReleaseEvent
void QLinkMenu::keyReleaseEvent(QKeyEvent* event) {
	if (_main_window) _main_window->keyReleaseEvent(event);
}

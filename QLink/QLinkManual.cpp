// QLinkManual.cpp
// a window to provide the manual of the game

#include "QLinkManual.h"

QLinkManual::QLinkManual(QWidget* parent) : QWidget(parent) {
	ui.setupUi(this);
}

QLinkManual::~QLinkManual() {
	/* there's nothing more to do */
}

void QLinkManual::closeEvent(QCloseEvent* event) {
	this->hide();
	event->ignore();
}

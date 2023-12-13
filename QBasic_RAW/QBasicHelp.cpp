// QBasicHelp.cpp
// providing a window to provide the user with help

#include "QBasicHelp.h"

QBasicHelp::QBasicHelp(QWidget* parent) : QWidget(parent) {
	ui.setupUi(this);
}

QBasicHelp::~QBasicHelp() {
	/* there's nothing to do here */
}

void QBasicHelp::closeEvent(QCloseEvent* event) {
	// hide this window but not delete
	this->hide();

	// ignore the event, thus the window will not be closed actually
	event->ignore();
}

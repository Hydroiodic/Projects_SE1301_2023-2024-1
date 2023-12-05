// QLinkMultiPlayerWin.cpp
// a window to exhibit all winners

#include "QLinkMultiPlayerWin.h"

QLinkMultiPlayerWin::QLinkMultiPlayerWin(QWidget* parent) : QWidget(parent) {
	ui.setupUi(this);
}

QLinkMultiPlayerWin::~QLinkMultiPlayerWin() {
	/* there's nothing more to do here */
}

// QLinkMultiPlayerWin::setWinners, to change the text of _winner_name_label into name
void QLinkMultiPlayerWin::setWinners(const QString& name) {
	ui._winner_name_label->setText(name);
}

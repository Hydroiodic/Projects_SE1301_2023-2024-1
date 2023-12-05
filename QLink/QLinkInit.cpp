// QLinkInit.cpp
// a son-window of the main window to change the scale of the game

#include "QLinkInit.h"

QLinkInit::QLinkInit(QWidget *parent) : QWidget(parent) {
	ui.setupUi(this);
}

QLinkInit::~QLinkInit() {
	/* Nothing to do here */
}

/*************************  Below are QLinkInit slots  ************************/

// QLinkInit::startNewGame, to collect infomation and start a new game
void QLinkInit::startNewGame() {
	this->hide();

	int width = ui._width_slider->value();
	int height = ui._height_slider->value();
	int kind = ui._kind_slider->value();

	((QLink*)this->parentWidget())->startGame(height, width, kind);

	for (int i = 0; i < ((QLink*)parentWidget())->_kid_num; ++i)
		((QLink*)parentWidget())->kid[i]->repaintCharactor();
}

// QLinkInit::changeWidthValue, QLinkInit::changeHeightValue and QLinkInit::changeIconKind, 
//     to change the text of the _line_edit
void QLinkInit::changeWidthValue(int width) {
	ui._width_edit->setText(QString(std::to_string(width).c_str()));
}

void QLinkInit::changeHeightValue(int height) {
	ui._height_edit->setText(QString(std::to_string(height).c_str()));
}

void QLinkInit::changeIconKind(int kind) {
	ui._kind_edit->setText(QString(std::to_string(kind).c_str()));
}

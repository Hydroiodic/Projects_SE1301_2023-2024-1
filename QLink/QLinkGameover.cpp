// QLinkGameover.cpp
// an app-modal window representing the end of the game

#include "QLinkGameover.h"

QLinkGameover::QLinkGameover(QLink* main, QWidget* parent) 
	: QWidget(parent), _main_window(main) {
	ui.setupUi(this);
}

QLinkGameover::~QLinkGameover() {
	/* there's nothing to do here */
}

// QLinkGameover::setResults, to change the text of the widget to inform different results
void QLinkGameover::setResults(int result, int score) {
	static const char* _result_text[] = {
		"# 恭喜！您消除了所有方块！",
		"# 恭喜！您已消除所有可消除的方块！",
		"# 时间到！游戏结束！"
	};
	static const char* _title_text[] = {
		"Winner!",
		"Winner!",
		"Timeout!"
	};

	QString _score_text = QString::fromLocal8Bit("## 您的最终得分是：<strong><font color=Red size=10>") 
		+ QString::number(score) + QString::fromLocal8Bit("分</font></strong>");

	this->setWindowTitle(_title_text[result]);
	ui._success_label->setText(QString::fromLocal8Bit(_result_text[result]));
	ui._score_label->setText(_score_text);
}

// slot QLinkGameover::clickOk, to close the window
void QLinkGameover::clickOk() {
	this->close();
}

// slot QLinkGameover::clickContinue, to close the window and open _init_window
void QLinkGameover::clickContinue() {
	this->close();
	_main_window->_init_window->show();
	_main_window->_init_window->raise();
}

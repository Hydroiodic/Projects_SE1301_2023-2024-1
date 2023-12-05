// QLinkMainTimer.cpp
// to provide a timer for the main window to use

#include "QLinkMainTimer.h"

QLinkMainTimer::QLinkMainTimer(QWidget* parent) {
	timer = new QTimer(parent);
	timer->start(1000);
	connect(timer, &QTimer::timeout, this, 
		QOverload<>::of(&QLinkMainTimer::reduceOneSecond));
}

QLinkMainTimer::~QLinkMainTimer() {
	delete timer;
}

// QLinkMainTimer::reduceOneSecond, to reduce a second when a second passed
void QLinkMainTimer::reduceOneSecond() {
	if (_is_paused) return;

	if (_cur_seconds > 0) {
		--_cur_seconds;
		updateTime();
	}
	else if (_cur_seconds == 0 && _first_timeout) {
		_first_timeout = false;
		emit timeout();
	}
}

// QLinkMainTimer::addSeconds, to add some seconds for the main timer
void QLinkMainTimer::addSeconds(int seconds) {
	if (_is_paused) return;
	_cur_seconds += seconds;
}

// QLinkMainTimer::resetTimer, to reset the timer with some initialised seconds
void QLinkMainTimer::resetTimer(int seconds) {
	_cur_seconds = seconds;
	_first_timeout = true;
	_is_paused = false;
}

// QLinkMainTimer::updateTime, to update the text of the _line_edit to show the left time
void QLinkMainTimer::updateTime() {
	char time[9];
	int minute = _cur_seconds / 60, second = _cur_seconds % 60;

	sprintf(time, "%d%d:%d%d", minute / 10, 
		minute % 10, second / 10, second % 10);

	emit updateText(time);
}

// QLinkMainTimer::clearTimer, to reset the timer with empty state
void QLinkMainTimer::clearTimer() {
	_cur_seconds = 0;
	_first_timeout = false;
}

// QLinkMainTimer::pauseTimer and QLinkMainTimer::continueTimer, to pause or continue the timer
void QLinkMainTimer::pauseTimer() {
	_is_paused = true;
}

void QLinkMainTimer::continueTimer() {
	_is_paused = false;
}

// QLinkMainTimer::getLeftSeconds, return the value of left seconds
int QLinkMainTimer::getLeftSeconds() const {
	return _cur_seconds;
}

// QLinkMainTimer::isWorking, to judge whether the timer is still working
bool QLinkMainTimer::isWorking() const {
	return _first_timeout;
}

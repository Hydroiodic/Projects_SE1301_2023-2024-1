// QLinkTimer.cpp
// provide a timer for this program to use

#include "QLinkTimer.h"

QLinkTimer::QLinkTimer(QObject *parent) : QTimer(parent) {
	connect(this, &QTimer::timeout, this, &QLinkTimer::Count);
};

QLinkTimer::~QLinkTimer() {
	/* there's nothing to do here */
}

// QLinkTimer::Count, to add _cur_milliseconds and judge whether to emit signal
void QLinkTimer::Count() {
	if ((_cur_milliseconds += 10) >= milliseconds) {
		emit TIMEOUT();
		_cur_milliseconds = 0;
	}
}

// QLinkTimer::Start, to start the timer with milliseconds ms
void QLinkTimer::Start(int milliseconds) {
	this->milliseconds = milliseconds;
	this->_cur_milliseconds = 0;
	this->start(10);
	_is_working = true;
}

// QLinkTimer::Terminate, nearly the same with QTimer::stop
void QLinkTimer::Terminate() {
	this->stop();
	_is_working = false;
}

// QLinkTimer::Stop, to stop the timer while recording _cur_milliseconds for later continuing
void QLinkTimer::Stop() {
	this->stop();
}

// QLinkTimer::Continue, to continue the timer after stopping with the previous _cur_milliseconds
void QLinkTimer::Continue() {
	if (!_is_working) return;
	this->start(10);
}

// QLinkTimer::Clear, to clear all settings of the timer
void QLinkTimer::Clear() {
	this->stop();

	int milliseconds = 0;
	int _cur_milliseconds = 0;
	bool _is_working = false;
}

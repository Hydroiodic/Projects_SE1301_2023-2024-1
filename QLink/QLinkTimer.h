// QLinkTimer.h
// header file for QLinkTimer.cpp

#pragma once

#include <QTimer>

class QLinkTimer : public QTimer
{
	Q_OBJECT

public:
	QLinkTimer(QObject* parent = nullptr);
	~QLinkTimer();

private:
	int milliseconds = 0;
	int _cur_milliseconds = 0;
	bool _is_working = false;

	void Count();

public:
	void Start(int);
	void Terminate();
	void Stop();
	void Continue();
	void Clear();

signals:
	void TIMEOUT();
};

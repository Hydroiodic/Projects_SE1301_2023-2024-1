// QLinkMainTimer.h
// header file for QLinkMainTimer.cpp

#pragma once

#include <QTimer>
#include <QLabel>

class QLinkMainTimer : public QObject
{
	Q_OBJECT

private:
	int _cur_seconds = 0;
	bool _first_timeout = false;
	bool _is_paused = false;
	QTimer* timer;

	void reduceOneSecond();

public:
	QLinkMainTimer(QWidget* parent = nullptr);
	~QLinkMainTimer();

	void resetTimer(int seconds = 60);
	void updateTime();
	void clearTimer();

	void pauseTimer();
	void continueTimer();

	void addSeconds(int seconds);

	int getLeftSeconds() const;
	bool isWorking() const;

signals:
	void updateText(QString);
	void timeout();
};

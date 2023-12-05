// QLinkProp.h
// header file for QLinkProp.cpp

#pragma once

#include <QLabel>
#include <QPixmap>
#include "QLink.h"
#include "QLinkTimer.h"

class QLink;
class QLinkCharactor;

class QLinkProp : public QLabel
{
	Q_OBJECT

private:
	static const int _total_prop_num = 7;

	int _prop_kind = 0;
	const int _time_interval = 10000;
	bool _is_shown = false;
	QPixmap* picture;
	QPixmap _unknown_picture;
	QLinkTimer* timer = nullptr;
	QLink* _parent;

	enum Direction { UP, LEFT, DOWN, RIGHT };
	void (QLinkProp::*_function_pointer[_total_prop_num])(int);

	void addTime(int);
	void shuffleButtons(int);
	void hint(int);
	void flash(int);
	void freeze(int);
	void dizzy(int);
	void speedup(int);

	// some variables to record how many times a kind of prop is picked up
	//     due to different achieving method, the hint prop doesn't need this
	int* _freeze_num;
	int* _dizzy_num;
	int* _speedup_num;

	// to process an existed timer to apply for QLinkProp
	void processTempTimer(QLinkTimer* _temp_timer);

	/*********************  Below are QLinkProp hint functions  ********************/

	int _hint_counter = 0;
	static const int _hint_milisecond = 10000;
	static const int _freeze_milisecond = 10000;
	static const int _dizzy_milisecond = 10000;
	static const int _speedup_milisecond = 10000;
	QLinkTimer* _hint_timer = nullptr;

	bool getFeasibleSolution(bool _need_highlight = true) const;
	void highlight(int i, int j, const QPair<int, int>& destination, int _fruit_kind) const;
	QPair<int, int> dfs(int i, int j, int _turn_count, int _fruit_kind, Direction dir) const;

public:
	QLinkProp(QLink* parent);
	~QLinkProp();

	bool isOnProp(QLinkCharactor* charactor) const;
	void takeEffect(int);

	void setRandomProp();
	void randomUpdate();
	int getRandomValue() const;

	void pauseTimer();
	void continueTimer();

	int getPropType() const;

	double _current_x, _current_y;
	inline int _true_x() const;
	inline int _true_y() const;
	void repaintProp();
	void movePropTo(int x, int y);

	// this function isn't related to QLinkProp, just for convenience
	bool checkSolution() const;

signals:
	void PAUSE();
	void CONTINUE();

private slots:
	void setShowState();
	void checkHint();

	void stopFreeze(int);
	void stopDizzy(int);
	void stopSpeedup(int);
	void deleteTimer(QLinkTimer*);
};

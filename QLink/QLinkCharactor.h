// QLinkCharactor.h
// header file for QLinkCharactor.cpp

#pragma once

#include <QPoint>
#include <QLabel>
#include <QKeyEvent>
#include <QObject>
#include "QLink.h"
#include "QLinkSettings.h"

class QLink;

// class QLinkCharactor: represent the main charactor for interaction in the game
class QLinkCharactor : public QLabel
{
public:
	enum direction { UP, LEFT, DOWN, RIGHT, STILL };

private:
	direction _current_direction = STILL;
	double speed = 1.0, _speed_order = .0;
	bool movable = false;
	double _current_x, _current_y;
	const int _time_interval = 10;

	// maybe use a vector<QPixmap> instead later, which 
	//     is easier for creating multi-charactors
	QPixmap _run_right[2], _run_left[2], _stay_still[2];
	int _current_run_image = 0;
	int _current_facing_direction = 0;

	/*****************  Below are QLinkCharactor position functions  ****************/
	inline int _true_x() const;
	inline int _true_y() const;

	// timer is used to update the charactor every 10ms
	QTimer* timer = nullptr;

	bool checkPosCorrect() const;
	bool checkPosCorrect(int x, int y) const;

	/*******************  Below are QLinkCharactor name functions  ******************/
	QString name;
	QLabel* _name_label = nullptr;

	void paintEvent(QPaintEvent* event) override;

public:
	// TODO: randomly sommon the main charactor without being trapped in a pit
	QLinkCharactor(QLink* parent, double SPEED_ORDER = 1.0, const QString& name = "");
	~QLinkCharactor();

	// rewrite some functions of QLabel
	void hide();
	void show();
	void raise();

	void moveCharactor();

	// repaint the charactor with a position and a image
	void repaintCharactor();
	void changeImage();

	// methods in regards to the charactor's movement
	void setMove(int dir);
	void setStill(int dir);
	void forceSetStill();

	inline void updateCharactor();

	// a method to change a direct coordination to the inner coordination
	void moveCharactorTo(int x, int y);

	// methods to set the variables _current_x and _current_y directly
	QPair<double, double> getInnerXY() const;
	void setInnerXY(const QPair<double, double>& pos);

	// update the charactor with random position
	void randomlySetPos();

	// to change the speed of the charactor
	void updateSpeed(double SPEED_ORDER);
};

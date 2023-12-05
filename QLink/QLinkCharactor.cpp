// QLinkCharactor.cpp
// a label to simulate the charactor 'kid'

#include "QLinkCharactor.h"

QLinkCharactor::QLinkCharactor(QLink* parent, double SPEED_ORDER, const QString& name) 
	: QLabel(parent), name(name) {
	// the picture will fill the QLabel entirely
	setScaledContents(true);
	_speed_order = SPEED_ORDER;

	_current_x = -parent->width() * ((QLink*)parentWidget())->_game_width_ratio / 2.0;
	_current_y = -parent->height() / 2.0;
	int _temp_width = parent->getButtonWidth() * parent->_kid_button_width_ratio;
	speed = _temp_width * SPEED_ORDER * _time_interval / 100;
	resize(_temp_width, _temp_width * parent->_kid_height_width_ratio);

	_run_right[0].load(":/QLink/kid/1.png");
	_run_right[1].load(":/QLink/kid/2.png");
	_run_left[0].load(":/QLink/kid/1_mir.png");
	_run_left[1].load(":/QLink/kid/2_mir.png");
	_stay_still[0].load(":/QLink/kid/3.png");
	_stay_still[1].load(":/QLink/kid/3_mir.png");

	timer = new QTimer(parent);
	timer->start(_time_interval);
	connect(timer, &QTimer::timeout, 
		this, &QLinkCharactor::updateCharactor);

	_name_label = new QLabel(parent);
	_name_label->setText(name);
}

QLinkCharactor::~QLinkCharactor() {
	delete timer;
	delete _name_label;
}

// QLinkCharactor::_true_x and QLinkCharactor::_true_y, to change the inner coordination to the direct coordination
inline int QLinkCharactor::_true_x() const {
	return parentWidget()->width() * ((QLink*)parentWidget())->_game_width_ratio / 2 + _current_x;
}

inline int QLinkCharactor::_true_y() const {
	return parentWidget()->height() / 2 + _current_y;
}

// QLinkCharactor::repaintCharactor, to update the coordination
void QLinkCharactor::repaintCharactor() {
	QLink* _parent = (QLink*)parentWidget();
	int _width_of_label = _parent->getButtonWidth() * _parent->_kid_button_width_ratio;
	int _height_of_label = _width_of_label * _parent->_kid_height_width_ratio;

	// keep the relative position of the main charactor to all buttons
	int _old_width_of_label = width();
	double ratio = _width_of_label / double(_old_width_of_label);
	if (ratio != 1) {
		_current_x *= ratio;
		_current_y *= ratio;
		speed = _width_of_label * _speed_order * _time_interval / 100;
	}

	int _half_game_width = _parent->width() * _parent->_game_width_ratio / 2;
	// prevent the charactor from getting out of the main window
	if (_true_x() < 0) _current_x = -_half_game_width;
	else if (_true_x() > _parent->width()) _current_x = _half_game_width - _width_of_label;
	if (_true_y() < 0) _current_y = -_parent->height() / 2;
	else if (_true_y() > _parent->height()) _current_y = _parent->height() / 2 - _height_of_label;

	resize(_width_of_label, _height_of_label);
	move(_true_x(), _true_y());
}

// QLinkCharactor::moveCharactor, to move the charactor by changing the variables _current_x and _current_y
void QLinkCharactor::moveCharactor() {
	QLink* _parent = (QLink*)parentWidget();
	int _width_of_label = _parent->getButtonWidth() * _parent->_kid_button_width_ratio;
	int _height_of_label = _width_of_label * _parent->_kid_height_width_ratio;
	int _previous_x = _current_x, _previous_y = _current_y;

	// there's a "if" for each condition to prevent the charactor being out of the window
	switch (_current_direction) {
	case UP:
		if (_true_y() <= int(speed)) break;
		_current_y -= speed; break;

	case LEFT:
		_current_facing_direction = 1;

		if (_true_x() <= int(speed)) break;
		_current_x -= speed; break;

	case DOWN:
		if (_true_y() >= _parent->height() - int(speed) - _width_of_label) break;
		_current_y += speed; break;

	case RIGHT:
		_current_facing_direction = 0;

		if (_true_x() >= _parent->width() * _parent->_game_width_ratio - 
			int(speed) - _height_of_label) break;
		_current_x += speed; break;

	case STILL: return;
	}

	if (!checkPosCorrect()) _current_x = _previous_x, _current_y = _previous_y;
	// update the position of the charactor
	move(_true_x(), _true_y());
}

// QLinkCharactor::setMove and QLinkCharactor::setStill and QLinkCharactor::forceSetStill, 
//     providing methods to change the state of the charactor's movement
void QLinkCharactor::setMove(int dir) {
	_current_direction = direction(dir);
	movable = true;
}

void QLinkCharactor::setStill(int dir) {
	if (dir == _current_direction) {
		movable = false;
		_current_direction = STILL;
	}
}

void QLinkCharactor::forceSetStill() {
	movable = false;
	_current_direction = STILL;
}

// QLinkCharactor::changeImage, to change the image to show on the label
void QLinkCharactor::changeImage() {
	// count is used to lower the speed of changing image
	static int count = 0;

	if (movable) {
		++count;

		if (count >= 5) {
			_current_run_image ^= 1;
			if (_current_facing_direction)
				setPixmap(_run_left[_current_run_image]);
			else setPixmap(_run_right[_current_run_image]);

			// reset the state of count
			count = 0;
		}
	}
	else setPixmap(_stay_still[_current_facing_direction]);
}

// QLinkCharactor::checkPosCorrect, to check whether the charactor is in a pit or not
bool QLinkCharactor::checkPosCorrect() const {
	int _central_x = _true_x() + width() / 2, _central_y = _true_y() + height() / 2;
	return checkPosCorrect(_central_x, _central_y);
}

bool QLinkCharactor::checkPosCorrect(int x, int y) const {
	// some data of the parent widget
	QLink* _parent = (QLink*)parentWidget();
	int** _simulated_data = _parent->_simulated_data;

	QPair<int, int> _next_btn = (_parent->findPos(QPoint(x, y)));
	return _next_btn.first == -1 || !_simulated_data[_next_btn.first][_next_btn.second];
}

// QLinkCharactor::updateCharactor, call QLinkCharactor::moveCharactor and 
//     QLinkCharactor::changeImage to update the charactor
inline void QLinkCharactor::updateCharactor() {
	moveCharactor();
	changeImage();
}

// QLinkCharactor::moveCharactorTo, to move the charactor to (x, y) while updating the inner coordination
void QLinkCharactor::moveCharactorTo(int x, int y) {
	_current_x = x - parentWidget()->width() * ((QLink*)parentWidget())->_game_width_ratio / 2;
	_current_y = y - parentWidget()->height() / 2;
	this->move(x, y);
}

// QLinkCharactor::getInnerXY and QLinkCharactor::setInnerXY, methods in regards to _current_x and _current_y
QPair<double, double> QLinkCharactor::getInnerXY() const {
	return QPair<double, double>(_current_x, _current_y);
}

void QLinkCharactor::setInnerXY(const QPair<double, double>& pos) {
	_current_x = pos.first; _current_y = pos.second;
}

// QLinkCharactor::paintEvent, called when the window needs to be painted
void QLinkCharactor::paintEvent(QPaintEvent* event) {
	QLabel::paintEvent(event);

	// adjust the size of the label
	_name_label->adjustSize();
	_name_label->move(this->x() + this->width() / 2 - _name_label->width() / 2, 
		this->y() - _name_label->height());
}

// QLinkCharactor::hide, QLinkCharactor::show and QLinkCharactor::raise, rewrite to deal with _name_label
void QLinkCharactor::hide() {
	QLabel::hide();
	_name_label->hide();
}

void QLinkCharactor::show() {
	QLabel::show();
	_name_label->show();
}

void QLinkCharactor::raise() {
	QLabel::raise();
	_name_label->raise();
}

// QLinkCharactor::randomlySetPos, to randomly set the position of the charactor randomly and correctly
void QLinkCharactor::randomlySetPos() {
	QLink* _parent = (QLink*)parentWidget();

	int x = rand() % static_cast<int>(_parent->width() * 
		_parent->_game_width_ratio - width() / 2);
	int y = rand() % (_parent->height() - height() / 2);

	if (checkPosCorrect(x + width() / 2, y + height() / 2))
		moveCharactorTo(x, y);
	else randomlySetPos();
}

// QLinkCharactor::updateSpeed, to change the order of the speed to a new value
void QLinkCharactor::updateSpeed(double SPEED_ORDER) {
	// update the variable _speed_order
	_speed_order = SPEED_ORDER;

	// update the speed instantly
	QLink* _parent = (QLink*)parentWidget();
	int _width_of_label = _parent->getButtonWidth() * _parent->_kid_button_width_ratio;
	speed = _width_of_label * _speed_order * _time_interval / 100;
}

// QLinkProp.cpp
// use a label to represent props

#include "QLinkProp.h"

QLinkProp::QLinkProp(QLink* parent) : QLabel(parent) {
	_parent = parent;

	// load images
	picture = new QPixmap[_total_prop_num];
	picture[0].load(":/props/addSecond.png");
	picture[1].load(":/props/shuffle.png");
	picture[2].load(":/props/hint.png");
	picture[3].load(":/props/flash.png");
	picture[4].load(":/props/freeze.png");
	picture[5].load(":/props/dizzy.png");
	picture[6].load(":/props/speedup.png");
	_unknown_picture.load(":/props/unknown.png");

	setScaledContents(true);
	timer = new QLinkTimer;
	_hint_timer = new QLinkTimer;

	_freeze_num = new int[parent->_kid_num] {};
	_dizzy_num = new int[parent->_kid_num] {};
	_speedup_num = new int[parent->_kid_num] {};

	// load functions
	_function_pointer[0] = &QLinkProp::addTime;
	_function_pointer[1] = &QLinkProp::shuffleButtons;
	_function_pointer[2] = &QLinkProp::hint;
	_function_pointer[3] = &QLinkProp::flash;
	_function_pointer[4] = &QLinkProp::freeze;
	_function_pointer[5] = &QLinkProp::dizzy;
	_function_pointer[6] = &QLinkProp::speedup;

	resize(_parent->getButtonWidth() / 2, _parent->getButtonWidth() / 2);
	setRandomProp();

	connect(_hint_timer, &QLinkTimer::TIMEOUT, this, &QLinkProp::checkHint);
	connect(timer, &QLinkTimer::TIMEOUT, this, &QLinkProp::setShowState);
	timer->Start(_time_interval);
}

QLinkProp::~QLinkProp() {
	delete[] picture;
	delete timer;
	delete _hint_timer;

	delete[] _freeze_num;
	delete[] _dizzy_num;
	delete[] _speedup_num;
}

// QLinkProp::dfs, called by function QLinkProp::hint, to find a feasible pair for linking
QPair<int, int> QLinkProp::dfs(int i, int j, int _turn_count, int _fruit_kind, Direction dir) const {
	// temporarily store the data of the game
	int M = _parent->M, N = _parent->N;
	int** _simulated_data = _parent->_simulated_data;
	int** _cur_turn_count = _parent->_cur_turn_count;

	if (_cur_turn_count[i][j] < _turn_count || _turn_count > _parent->_max_allow_turn) return QPair<int, int>(-1, -1);
	if (_simulated_data[i][j] && _simulated_data[i][j] == _fruit_kind) return QPair<int, int>(i, j);
	if (_simulated_data[i][j] && _simulated_data[i][j] != _fruit_kind) return QPair<int, int>(-1, -1);
	_cur_turn_count[i][j] = _turn_count;

	QPair<int, int> temp(-1, -1);
	if (i - 1 >= 0) {
		temp = dfs(i - 1, j, _turn_count + (dir != UP), _fruit_kind, UP);
		if (temp.first != -1) return temp;
	}
	if (j - 1 >= 0) {
		temp = dfs(i, j - 1, _turn_count + (dir != LEFT), _fruit_kind, LEFT);
		if (temp.first != -1) return temp;
	}
	if (i + 1 <= M + 1) {
		temp = dfs(i + 1, j, _turn_count + (dir != DOWN), _fruit_kind, DOWN);
		if (temp.first != -1) return temp;
	}
	if (j + 1 <= N + 1) {
		temp = dfs(i, j + 1, _turn_count + (dir != RIGHT), _fruit_kind, RIGHT);
		if (temp.first != -1) return temp;
	}
	return temp;
}

// QLinkProp::highlight, to set the hinted button highlighted with yellow color
void QLinkProp::highlight(int i, int j, const QPair<int, int>& destination, int _fruit_kind) const {
	_parent->_first_highlight_btn = QPair<int, int>(i, j);
	_parent->_second_highlight_btn = destination;
	_parent->_simulated_data[i][j] = _fruit_kind;
	_parent->resetDfsState();
}

// QLinkProp::getFeasibleSolution, to find a feasible pair for linking
bool QLinkProp::getFeasibleSolution(bool _need_highlight) const {
	// temporarily store the data of the game
	int M = _parent->M, N = _parent->N;

	QPair<int, int> temp(-1, -1);
	for (int i = 1; i <= M; ++i) {
		for (int j = 1; j <= N; ++j) {
			// if the button is hidden or a pit
			if (_parent->_simulated_data[i][j] <= 0) continue;

			int _fruit_kind = _parent->_simulated_data[i][j];
			_parent->_cur_turn_count[i][j] = -1;

			// using a lambda expression to simplify the code
			auto function = [&](bool condition, int _next_i, int _next_j,
				int _initial_turn_count, int _fruit_kind, Direction dir) {
					if (condition) {
						temp = dfs(_next_i, _next_j, _initial_turn_count, _fruit_kind, dir);
						if (temp.first != -1) {
							if (_need_highlight) highlight(i, j, temp, _fruit_kind);
							return true;
						}
					}
					return false;
				};

			bool result = 
				function(i - 1 >= 0, i - 1, j, 0, _fruit_kind, UP) || 
				function(j - 1 >= 0, i, j - 1, 0, _fruit_kind, LEFT) || 
				function(i + 1 <= M + 1, i + 1, j, 0, _fruit_kind, DOWN) || 
				function(j + 1 <= N + 1, i, j + 1, 0, _fruit_kind, RIGHT);

			// reset the dfs state
			_parent->resetDfsState();
			if (result) return true;
		}
	}

	// no feasible solution
	return false;
}

// QLinkProp::checkSolution, to check whether there is a feasible pair for the game
bool QLinkProp::checkSolution() const {
	return getFeasibleSolution(false);
}

// QLinkProp::addTime, the addition prop, to add 30 seconds for the game
void QLinkProp::addTime(int) {
	_parent->_main_timer->addSeconds(30);
}

// QLinkProp::shuffleButtons, the shuffle prop, to randomly reorganise the distribution of buttons
void QLinkProp::shuffleButtons(int) {
	// temporarily store the data of the game
	int M = _parent->M, N = _parent->N;
	int** _simulated_data = _parent->_simulated_data;
	QLinkButton*** data = _parent->data;
	QLinkIcon* icon = &_parent->icon;

	// save the kinds and counts of all the button
	QVector<int> vector;
	for (int i = 1; i <= M; ++i) {
		for (int j = 1; j <= N; ++j) {
			if (_simulated_data[i][j] != -1) vector.push_back(_simulated_data[i][j]);
		}
	}

	// randomly choose a existing fruit
	for (int i = 1; i <= M; ++i) {
		for (int j = 1; j <= N; ++j) {
			if (_simulated_data[i][j] == -1) continue;

			int _rand_num = rand() % vector.size();
			_simulated_data[i][j] = vector[_rand_num];
			vector.erase(vector.begin() + _rand_num);

			if (_simulated_data[i][j]) {
				data[i][j]->setIcon(icon->operator[](_simulated_data[i][j]));
				data[i][j]->show();
			}
			else data[i][j]->hide();
		}
	}

	for (int i = 0; i < _parent->_kid_num; ++i)
		_parent->_cur_pressed_button[i] = QPair<int, int>(-1, -1);
}

// QLinkProp::hint, the hint prop, to start the timer so that the prop takes effects
void QLinkProp::hint(int) {
	_hint_counter = 0;
	_hint_timer->Start(10);
}

// QLinkProp::flash, the flash prop, not built yet
void QLinkProp::flash(int) {
	// TODO: the flash prop
}

// QLinkProp::freeze, the freeze prop, to freeze the other player for 3 seconds
void QLinkProp::freeze(int _charactor_no) {
	_parent->freeze[1 - _charactor_no] = true;
	_parent->kid[1 - _charactor_no]->forceSetStill();

	// create timer to count for the interval
	QLinkTimer* _freeze_timer = new QLinkTimer;
	connect(_freeze_timer, &QLinkTimer::TIMEOUT, this,
		std::bind(&QLinkProp::stopFreeze, this, 1 - _charactor_no));
	processTempTimer(_freeze_timer);
	_freeze_timer->Start(_freeze_milisecond);

	++_freeze_num[1 - _charactor_no];
}

// QLinkProp::dizzy, the dizzy prop, to reverse the operations of the other player for 10 seconds
void QLinkProp::dizzy(int _charactor_no) {
	_parent->dizzy[1 - _charactor_no] = true;

	// create timer to count for the interval
	QLinkTimer* _dizzy_timer = new QLinkTimer;
	connect(_dizzy_timer, &QLinkTimer::TIMEOUT, this,
		std::bind(&QLinkProp::stopDizzy, this, 1 - _charactor_no));
	processTempTimer(_dizzy_timer);
	_dizzy_timer->Start(_dizzy_milisecond);

	++_dizzy_num[1 - _charactor_no];
}

// QLinkProp::speedup, the speedup prop, to accelerate for the kid for 8 seconds
void QLinkProp::speedup(int _charactor_no) {
	_parent->kid[_charactor_no]->updateSpeed(_parent->_speed_up_kid_speed);

	// create timer to count for the interval
	QLinkTimer* _speedup_timer = new QLinkTimer;
	connect(_speedup_timer, &QLinkTimer::TIMEOUT, this,
		std::bind(&QLinkProp::stopSpeedup, this, _charactor_no));
	processTempTimer(_speedup_timer);
	_speedup_timer->Start(_speedup_milisecond);

	++_speedup_num[_charactor_no];
}

// QLinkProp::stopFreeze, to remove the freeze state of the kid when time is up
void QLinkProp::stopFreeze(int _charactor_no) {
	if (!--_freeze_num[_charactor_no]) _parent->freeze[_charactor_no] = false;
}

// QLinkProp::stopDizzy, to remove the dizzy state of the kid when time is up
void QLinkProp::stopDizzy(int _charactor_no) {
	if (!--_dizzy_num[_charactor_no]) _parent->dizzy[_charactor_no] = false;
}

// QLinkProp::stopSpeedup, to remove the speedup state of the kid when time is up
void QLinkProp::stopSpeedup(int _charactor_no) {
	if (!--_speedup_num[_charactor_no])
		_parent->kid[_charactor_no]->updateSpeed(_parent->_original_kid_speed);
}

// QLinkProp::createTempTimer, to create a timer applying for QLinkProp
void QLinkProp::processTempTimer(QLinkTimer* _temp_timer) {
	// only execute once, and release the space after that
	connect(_temp_timer, &QLinkTimer::TIMEOUT, this,
		std::bind(&QLinkProp::deleteTimer, this, _temp_timer));

	// pause and continue on the same time with QLinkProp
	connect(this, &QLinkProp::PAUSE, _temp_timer, &QLinkTimer::Stop);
	connect(this, &QLinkProp::CONTINUE, _temp_timer, &QLinkTimer::Continue);
}

// QLinkProp::deleteTimer, a slot to release the space on heap
void QLinkProp::deleteTimer(QLinkTimer* _timer) {
	_timer->Stop();
	delete _timer;
}

// QLinkProp::isOnProp, to check whether the charactor is on the prop or not
bool QLinkProp::isOnProp(QLinkCharactor* charactor) const {
	if (!_is_shown) return false;

	int _charactor_x = charactor->x() + charactor->width() / 2,
		_charactor_y = charactor->y() + charactor->height() / 2;

	return _charactor_x >= x() && _charactor_x <= x() + width() &&
		_charactor_y >= y() && _charactor_y <= y() + height();
}

// QLinkProp::takeEffect, to take effect for the prop
void QLinkProp::takeEffect(int _charactor_no) {
	(this->*_function_pointer[_prop_kind])(_charactor_no);
	this->hide();
	_is_shown = false;
	setRandomProp();
}

// QLinkProp::setRandomProp, to randomly set the genre of the prop with a specific odd
void QLinkProp::setRandomProp() {
	static const int _MAX_USED_RANDOM_NUM_SINGLE_PLAYER = 11;
	static const int _MAX_USED_RANDOM_NUM_TWO_PLAYERS = 15;
	int _random_num = rand();

	// the probabilities of each prop is determined here
	/**************************************************************************** /

				+1 : shuffle : hint : flash : freeze : dizzy : speedup
				 2 :    5    :   2  :   1   :    2   :   2   :    2

	/ ****************************************************************************/
	switch (_parent->_game_mode) {
		// single-player mode
	case 0:
		_random_num %= _MAX_USED_RANDOM_NUM_SINGLE_PLAYER;
		if (_random_num <= 1) _prop_kind = 0;
		else if (_random_num <= 6) _prop_kind = 1;
		else if (_random_num <= 8) _prop_kind = 2;
		else if (_random_num <= 10) _prop_kind = 6;
		else if (_random_num <= 11) _prop_kind = 3;	// not built now
		break;

	case 1:
		_random_num %= _MAX_USED_RANDOM_NUM_TWO_PLAYERS;
		if (_random_num <= 1) _prop_kind = 0;
		else if (_random_num <= 6) _prop_kind = 1;
		else if (_random_num <= 8) _prop_kind = 2;
		else if (_random_num <= 10) _prop_kind = 4;
		else if (_random_num <= 12) _prop_kind = 5;
		else if (_random_num <= 14) _prop_kind = 6;
	}

	// set the icon of the prop
	if (rand() % 4) this->setPixmap(picture[_prop_kind]);
	else this->setPixmap(_unknown_picture);
	randomUpdate();

	// set a random appearing time
	timer->Start(_time_interval + getRandomValue());
}

// QLinkProp::randomUpdate, to randomly set the position of the prop correctly
void QLinkProp::randomUpdate() {
	int _x = rand() % static_cast<int>(_parent->width() * 
		_parent->_game_width_ratio - width()), 
		_y = rand() % (_parent->height() - height());

	// check whether the prop is in a pit or not
	QPoint _central_position(_x + width() / 2, _y + height() / 2);
	QPair<int, int> _cur_btn = _parent->findPos(_central_position);

	if (_cur_btn.first != -1) randomUpdate();
	else movePropTo(_x, _y);
}

// QLinkProp::getRandomValue, return a value ranging from -2000 to 2000
int QLinkProp::getRandomValue() const {
	// the function is like "f(x) = 2000 * (1 - e^(-kx))"
	int x = rand();
	double k = 3.0 / RAND_MAX;
	int sign = rand() % 2 ? 1 : -1;

	return sign * 2000 * (1 - exp(-k * x));
}

// QLinkProp::getPropType, return a value representing the genre of the prop
int QLinkProp::getPropType() const {
	// 0: +1, 1: shuffle, 2: hint
	return _prop_kind;
}

// QLinkProp::setShowState, to show the prop
void QLinkProp::setShowState() {
	_is_shown = true;
	repaintProp();
	this->show();
	timer->Stop();
}

// QLinkProp::checkHint, to check whether to find the next hinted pair
void QLinkProp::checkHint() {
	if (_parent->_first_highlight_btn.first == -1) {
		getFeasibleSolution();
	}

	if (++_hint_counter >= _hint_milisecond / 10) {
		_hint_timer->Stop();

		_parent->_first_highlight_btn = QPair<int, int>(-1, -1);
		_parent->_second_highlight_btn = QPair<int, int>(-1, -1);
	}
}

// QLinkProp::pauseTimer, to stop the timer
void QLinkProp::pauseTimer() {
	timer->Stop();
	_hint_timer->Stop();
	emit PAUSE();
}

// QLinkProp::continueTimer, to continue the timer
void QLinkProp::continueTimer() {
	timer->Continue();
	_hint_timer->Continue();
	emit CONTINUE();
}

// QLinkProp::_true_x and QLinkProp::_true_y, to transit the inner coordination to the real one
inline int QLinkProp::_true_x() const {
	return parentWidget()->width() * _parent->_game_width_ratio / 2 + _current_x;
}

inline int QLinkProp::_true_y() const {
	return parentWidget()->height() / 2 + _current_y;
}

// QLinkProp::repaintProp, to move and resize the prop when the parent window is resized
void QLinkProp::repaintProp() {
	if (!_is_shown) return;

	int _width_of_prop = ((QLink*)parentWidget())->getButtonWidth() / 2;

	// keep the relative position of the main charactor to all buttons
	int _old_width_of_prop = width();
	double ratio = _width_of_prop / double(_old_width_of_prop);
	if (ratio != 1) {
		_current_x *= ratio;
		_current_y *= ratio;
	}

	int _half_game_width = parentWidget()->width() * _parent->_game_width_ratio / 2;
	// prevent the charactor from getting out of the main window
	if (_true_x() < 0) _current_x = -_half_game_width;
	else if (_true_x() > parentWidget()->width())
		_current_x = _half_game_width - _width_of_prop;
	if (_true_y() < 0) _current_y = -parentWidget()->height() / 2;
	else if (_true_y() > parentWidget()->height())
		_current_y = parentWidget()->height() / 2 - _width_of_prop;

	resize(_width_of_prop, _width_of_prop);
	move(_true_x(), _true_y());
}

// QLinkProp::movePropTo, to move the prop to a real coordination
void QLinkProp::movePropTo(int x, int y) {
	_current_x = x - parentWidget()->width() * _parent->_game_width_ratio / 2;
	_current_y = y - parentWidget()->height() / 2;
	move(x, y);
}

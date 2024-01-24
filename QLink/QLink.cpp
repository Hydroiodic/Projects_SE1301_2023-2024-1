// QLink.cpp
// the main window for the program

#include <ctime>
#include <functional>
#include <fstream>
#include "QLink.h"

/*********************  Below are QLink window functions  ********************/

QLink::QLink(QWidget *parent) : QMainWindow(parent) {
    // set the game random
    srand(time(NULL));

    // show initial page
    setInitialPage();
}

QLink::~QLink() {
    if (!_game_initialized) return;

    if (_in_game) clearGame();

    for (int i = 0; i < _kid_num; ++i)
        delete kid[i];

    delete _main_timer;
    delete _main_painter;
    delete _link_painter;

    if (_gameover_window) delete _gameover_window;
    if (_about_window) delete _about_window;
    if (_multi_window) delete _multi_window;
    if (_painter_timer) delete _painter_timer;

    delete menu;
    delete manual;
    delete _init_window;

#ifdef MULTITHREADS
    thread->quit();
    thread->wait();
#endif // MULTITHREADS
}

// QLink::setInitialPage, called by the construction funciton, to paint the initial page
void QLink::setInitialPage() {
    this->resize(600, 400);
    this->setFocusPolicy(Qt::NoFocus);
    this->setWindowTitle("QLink");
    this->setCentralWidget(_init_class._start_widget);
    _init_class._start_label->installEventFilter(this);
}

// QLink::prepareGame, to prepare the basic variables for the game
void QLink::prepareGame() {
    // prevent wild pointer being called
    _init_class.stopLabel();

    ui.setupUi(this);
    ui._no_pair_label->hide();
    ui._no_pair_label->installEventFilter(this);

    _main_timer = new QLinkMainTimer(this);
    connect(_main_timer, &QLinkMainTimer::timeout, 
        this, std::bind(&QLink::createGameoverWindow, this, 2));
    connect(_main_timer, &QLinkMainTimer::updateText,
        this->ui._left_time_edit, &QLineEdit::setText);

    initializeKid();

    connect(this, &QLink::linkState, this, &QLink::setLinkState);

    _link_painter = new QLinkPainter(this);
    _main_painter = new QLinkPainter(this);

    menu = new QLinkMenuBar(this);
    this->setMenuBar(menu);

    manual = new QLinkManual(nullptr);
    manual->hide();

    _init_window = new QLinkInit(this);
    _init_window->hide();

    initializeThread();

    // set game initialized
    _game_initialized = true;
    _init_class._game_initialized = true;
}

// QLink::initializeKid, to create kids at the beginning of the game
void QLink::initializeKid() {
    // initialize all of the kids and their name
    for (int i = 0; i < _kid_num; ++i) {
        char _temp_name[10];
        sprintf(_temp_name, "Player_%d", i);

        kid[i] = new QLinkCharactor(this, _original_kid_speed, _temp_name);
        if (i) kid[i]->hide();
        else kid[i]->show();

        _cur_pressed_button[i] = QPair<int, int>(-1, -1);
    }

    // to set the position of all kids randomly
    for (int i = 0; i < _kid_num; ++i) {
        kid[i]->repaintCharactor();
        kid[i]->randomlySetPos();
    }
}

// QLink::initializeThread, to use multi-thread for the painting
void QLink::initializeThread() {
#ifndef MULTITHREADS
    return;
#endif // !MULTITHREADS

    _painter_timer = new QTimer(this);
    _painter_timer->start(30);
    connect(_painter_timer, &QTimer::timeout, this, &QLink::emitPaintingSignal);

    thread = new QThread;
    QLinkThread* _thread_ = new QLinkThread();
    _thread_->moveToThread(thread);
    thread->start();

    connect(_thread_, &QLinkThread::sendPixmap, this, &QLink::setPixmap);
    connect(_painter_timer, &QTimer::timeout, this, &QLink::emitPaintingSignal);
    connect(this, &QLink::getPixmap, _thread_, &QLinkThread::paintPixmap);
}

// QLink::createGameoverWindow, called when the game ends
void QLink::createGameoverWindow(int result) {
    stopGame();

    if (_game_mode) {
        settleMultiGame();
        return;
    }

    if (_gameover_window) delete _gameover_window;
    _gameover_window = new QLinkGameover(this);
    _gameover_window->setResults(result, score[0]);
    _gameover_window->show();
}

// QLink::createAboutBox, to provide info about this game
void QLink::createAboutBox() {
    if (!_game_stopped) pauseGame();

    if (_about_window) delete _about_window;
    _about_window = new QLinkAboutBox(nullptr);
    _about_window->show();
}

// QLink::createMultiWindow, to provides winners when this is a multi-player game
void QLink::createMultiWindow() {
    stopGame();

    if (_multi_window) delete _multi_window;
    _multi_window = new QLinkMultiPlayerWin(nullptr);
    _multi_window->show();
}

// QLink::settleMultiGame, settle the game when the number of players is beyond 1
void QLink::settleMultiGame() {
    int _max_score = -1;
    QVector<int> _max_score_no;

    for (int i = 0; i < _kid_num; ++i) {
        if (i > _game_mode) break;

        if (score[i] > _max_score) {
            _max_score = score[i];
            _max_score_no.clear();
            _max_score_no.push_back(i);
        }
        else if (score[i] == _max_score) {
            _max_score_no.push_back(i);
        }
    }

    QString text = "<strong><font size=6 color=Blue>Player_" + QString::number(_max_score_no[0]);
    for (int i = 1; i < _max_score_no.size(); ++i) {
        text += ", Player_" + QString::number(_max_score_no[i]);
    }
    text += "</font></strong>";

    createMultiWindow();
    _multi_window->setWinners(text);
}

// QLink::setBackground, to set a background with a suitable size
void QLink::setBackground() {
    // if background picture is changed, these variables need changing, too
    static const double _bg_width = 960.0, _bg_height = 600.0;
    static const double _init_bg_width = 1800.0, _init_bg_height = 1200.0;

    QPainter* painter = new QPainter(this);
    if (_game_initialized) {
        double ratio = qMax(this->width() / _bg_width, this->height() / _bg_height);
        painter->drawPixmap(0, 0, _bg_width * ratio,
            _bg_height * ratio, QPixmap(":QLink/background/background.png"));
    }
    else {
        double ratio = qMax(this->width() / _init_bg_width, this->height() / _init_bg_height);
        painter->drawPixmap(0, 0, _init_bg_width * ratio,
            _init_bg_height * ratio, QPixmap(":QLink/background/initial_background.png"));
    }
    delete painter;
}

// QLink::setScoreText, to show scores correctly in the textbox
void QLink::setScoreText() {
    QString _score_text = QString::number(score[0]);
    for (int i = 1; i < _kid_num; ++i) {
        if (i > _game_mode) break;
        _score_text += ":";
        _score_text += QString::number(score[i]);
    }
    ui._cur_score_edit->setText(_score_text);
}

// QLink::paintButtonSurround, to present the state of a specific button
void QLink::paintButtonSurround() {
    // highlight the hinted buttons
    if (_first_highlight_btn.first != -1) {
        if (data[_first_highlight_btn.first][_first_highlight_btn.second]->isHidden() || 
            data[_second_highlight_btn.first][_second_highlight_btn.second]->isHidden()) {
            _first_highlight_btn = _second_highlight_btn = QPair<int, int>(-1, -1);
        }
        else {
            data[_first_highlight_btn.first][_first_highlight_btn.second]->setState(Qt::yellow);
            data[_second_highlight_btn.first][_second_highlight_btn.second]->setState(Qt::yellow);
        }
    }

    static const QColor _color_btn_to_set[_kid_num] { Qt::blue, Qt::blue };
    static const QColor _color_pressed_btn[_kid_num] { Qt::black, Qt::white };

    // call off QLink::findKid() and QLinkButton::setOnState() functions
    for (int i = 0; i < _kid_num; ++i) {
        if (i > _game_mode) break;

        QPair<int, int> _button_to_set = findKid(kid[i], _find_kid_ratio);
        if (_button_to_set.first != -1 && data[_button_to_set.first][_button_to_set.second]) {
            data[_button_to_set.first][_button_to_set.second]->repaintButton(_color_btn_to_set[i]);
        }

        if (_cur_pressed_button[i].first != -1) {
            data[_cur_pressed_button[i].first][_cur_pressed_button[i].second]->setState(_color_pressed_btn[i]);
        }
    }
}

// QLink::showButtons, to update the position and size of buttons
void QLink::resizeButtons() {
    // aquire the updated basic info
    QRect rect(this->rect());
    // the operation below is to leave some space for game details
    rect.setWidth(rect.width() * _game_width_ratio);
    // calculate a suitable width for each QLinkButton
    int _line_width = qMin(rect.width() / N, rect.height() / M) * _button_size_ratio;
    int _shown_width = _line_width * _shown_width_ratio;
    int _position_interval = (_shown_width - _line_width) / 2;
    QSize size(_line_width, _line_width);
    QSize _icon_size(_line_width * 9 / 10, _line_width * 9 / 10);

    // show all the buttons
    for (int i = 1; i <= M; ++i) {
        for (int j = 1; j <= N; ++j) {
            if (data && data[i][j]) {
                QPoint pos(rect.width() / 2 - _shown_width * N / 2 + _shown_width * (j - 1) + _position_interval,
                    rect.height() / 2 - _shown_width * M / 2 + _shown_width * (i - 1) + _position_interval);
                data[i][j]->resize(size);
                data[i][j]->move(pos);
                data[i][j]->updateEdge();
                data[i][j]->setIconSize(_icon_size);
            }
        }
    }
}

// QLink::drawInfoLine, to add a dividing line between game and information
void QLink::drawInfoLine() {
    QRect rect = this->rect();

    _main_painter->setWidth(10.0);
    _main_painter->setColor(Qt::black);
    _main_painter->drawOneLine(rect.width() * _game_width_ratio, 0,
        rect.width() * _game_width_ratio, rect.height());
}

// QLink::paintEvent, called whenever the window is to be repainted
void QLink::paintEvent(QPaintEvent* event) {
    // always needed to be done
    setBackground();

    // if game isn't initialized, no more operations needed
    if (!_game_initialized) return;

    // deal with multi-threads
#ifdef MULTITHREADS
    _main_painter->begin(this);
    _main_painter->drawPixmap(0, 0, _painter_map);
    _main_painter->end();
#else
    drawInfoLine();
    paintButtonSurround();

    if (!_link_lines.isEmpty()) {
        showPath();

        // to update the entire window only once to show the path
        if (_need_update) {
            _need_update = false;
            update();
        }
    }
#endif // MULTITHREADS

    // raise kids to the top of the window
    for (int i = 0; i < _kid_num; ++i)
        kid[i]->raise();

    // raise prop to the top of the window and check if needed to take effect
    if (prop) {
        prop->raise();
        checkProp();
    }
}

// QLink::resizeEvent, called whenever the size of the window is changed
void QLink::resizeEvent(QResizeEvent* event) {
    if (!_game_initialized) return;

    QMainWindow::resizeEvent(event);

    resizeButtons();
    drawInfoLine();
    if (prop) prop->repaintProp();

    for (int i = 0; i < _kid_num; ++i)
        kid[i]->repaintCharactor();
}

// QLink::closeEvent, called when the window is to be closed
void QLink::closeEvent(QCloseEvent* event) {
    if (!_game_initialized) return;
    if (!_game_stopped) pauseGame();

    // create a messagebox to give a warning
    QMessageBox msg_box(QMessageBox::Information, "Warning",
        "Are you sure to exit the game? Your game will not be saved if continuing.",
        QMessageBox::Yes | QMessageBox::No);
    int return_value = msg_box.exec();

    switch (return_value) {
    case QMessageBox::Yes:
        break;
        
    case QMessageBox::No:
        // to ignore the event, which means the event won't go on to the next operation
        event->ignore();
        break;

    default: throw("Return value error when existing!");
    }
}

// QLink::getButtonWidth, used by other classes
int QLink::getButtonWidth() const {
    QRect rect(this->rect());
    // the operation below is to leave some space for game details
    rect.setWidth(rect.width() * _game_width_ratio);

    return qMin(rect.width() / N, rect.height() / M) * _button_size_ratio;
}

/**********************  Below are QLink game functions  *********************/

// QLink::initializeGame, called by QLink::startGame
void QLink::initializeGame(int _M, int _N, int _used_kinds = 0) {
    if (_in_game) clearGame();
    _in_game = true;
    _left_buttons = 0;

    _game_stopped = false;
    ui._cur_score_edit->setText("0");
    ui._link_state_edit->setText("");
    ui._pause_button->setDisabled(false);
    ui._pause_button->setText(QString::fromLocal8Bit("暂停"));

    _main_timer->resetTimer(_M * _N + _used_kinds * 2);
    _main_timer->updateTime();

    // reset _layout_version
    _layout_version = 0;
    _layout_version_of_linking.clear();

    // set the size of the game
    M = _M; N = _N;
    if (_used_kinds) icon.resize(_used_kinds);

    // reset the speed of the kid
    for (int i = 0; i <= _game_mode; ++i)
        kid[i]->updateSpeed(_original_kid_speed);

    // create buttons
    data = new QLinkButton * *[M + 2];
    _simulated_data = new int* [M + 2];
    _cur_turn_count = new int* [M + 2];
    _prev_button = new QPair<int, int>* [M + 2];

    for (int i = 0; i <= M + 1; ++i) {
        _simulated_data[i] = new int[N + 2] {};
        _cur_turn_count[i] = new int[N + 2];
        _prev_button[i] = new QPair<int, int>[N + 2];
        std::memset(_cur_turn_count[i], 0x3f, (N + 2) * sizeof(int));
    }

    for (int i = 1; i <= M; ++i) {
        data[i] = new QLinkButton * [N + 2] {};
        for (int j = 1; j <= N; ++j) {
            data[i][j] = new QLinkButton(this);
            data[i][j]->setFocusPolicy(Qt::NoFocus);
        }
    }
}

// QLink::setKidPosRandomly, to initialize the position of charactors
void QLink::setKidPosRandomly() {
    for (int i = 0; i < _kid_num; ++i) {
        if (i > _game_mode) break;
        kid[i]->repaintCharactor();
        kid[i]->randomlySetPos();
    }
}

// QLink::startGame, to start a new _M * _N game
void QLink::startGame(int _M, int _N, int _used_kinds) {
    // some basic initialization of a game
    initializeGame(_M, _N, _used_kinds);

    // assure the game solvable or nearly solvable by adding buttons in pairs
    QVector<int> _random_container;
    for (int i = 1; i <= M * N / 2; ++i) {
        int temp = rand() % icon.getTotalNum();
        _random_container.push_back(temp);
        _random_container.push_back(temp);

        if (temp) _left_buttons += 2;
    }
    if (_random_container.size() != M * N) _random_container.push_back(0);

    // randomly summon buttons
    for (int i = 1; i <= M; ++i) {
        for (int j = 1; j <= N; ++j) {
            int no = rand() % _random_container.size();
            int type = _random_container[no];
            _random_container.erase(_random_container.begin() + no);

            _simulated_data[i][j] = type;
            data[i][j]->setIcon(icon[type]);
            data[i][j]->show();

            if (!type) _simulated_data[i][j] = -1;
        }
    }

    // adjust buttons with correct positions and size
    resizeButtons();

    // initialization of QLinkProp
    prop = new QLinkProp(this);
    prop->hide();

    // initialize the positions of the main charactors
    setKidPosRandomly();

    if (!_left_buttons) createGameoverWindow(0);
    checkSolution();
}

// QLink::startGame, to start a new game with existed data
//     ATTENTION! No checking existing, please ensure the data right
void QLink::startGame(int _M, int _N, const int _data[][_MAX_N + 1]) {
    initializeGame(_M, _N);

    for (int i = 1; i <= M; ++i) {
        for (int j = 1; j <= N; ++j) {
            _simulated_data[i][j] = _data[i][j];
            if (_data[i][j] > 0) {
                data[i][j]->setIcon(icon[_data[i][j]]);
                ++_left_buttons;
            }
            else if (_data[i][j] == -1) data[i][j]->setIcon(icon[0]);

            if (_data[i][j]) data[i][j]->show();
            else data[i][j]->hide();
        }
    }

    // adjust buttons with correct positions and size
    resizeButtons();

    // adjust the position and size of the main charactor
    for (int i = 0; i < _kid_num; ++i)
        kid[i]->repaintCharactor();

    // initialization of QLinkProp
    prop = new QLinkProp(this);
    prop->hide();
}

// QLink::clearGame, to erase a game entirely
void QLink::clearGame() {
    _in_game = false;

    for (int i = 1; i <= M; ++i) {
        for (int j = 1; j <= N; ++j)
            if (data[i][j]) delete data[i][j];
        delete[] data[i];
    }
    for (int i = 0; i <= M + 1; ++i) {
        delete[] _simulated_data[i];
        delete[] _cur_turn_count[i];
        delete[] _prev_button[i];
    }

    delete[] data;
    delete[] _simulated_data;
    delete[] _cur_turn_count;
    delete[] _prev_button;

    data = nullptr;
    _simulated_data = nullptr;
    _cur_turn_count = nullptr;
    _prev_button = nullptr;
    _first_highlight_btn = QPair<int, int>(-1, -1);
    _second_highlight_btn = QPair<int, int>(-1, -1);

    for (int i = 0; i < _kid_num; ++i)
        _cur_pressed_button[i] = QPair<int, int>(-1, -1);

    if (prop) delete prop;
    prop = nullptr;
    std::memset(dizzy, 0, sizeof(bool) * _kid_num);
    std::memset(freeze, 0, sizeof(bool) * _kid_num);

    for (int i = 0; i < _kid_num; ++i)
        score[i] = 0;

    _main_timer->clearTimer();
    ui._cur_score_edit->setText("");
    ui._left_time_edit->setText("");
}

// QLink::pauseGame, to interrupt the game for later continuing
void QLink::pauseGame() {
    if (!_game_stopped) {
        for (int i = 0; i < _kid_num; ++i)
            kid[i]->forceSetStill();

        _game_stopped = true;
        _main_timer->pauseTimer();
        ui._pause_button->setText(QString::fromLocal8Bit("继续"));

        if (_in_game) prop->pauseTimer();
    }
    else {
        _game_stopped = false;
        _main_timer->continueTimer();
        ui._pause_button->setText(QString::fromLocal8Bit("暂停"));

        if (_in_game) prop->continueTimer();
    }
}

// QLink::stopGame, to interrupt the game when time's up and so on
void QLink::stopGame() {
    for (int i = 0; i < _kid_num; ++i)
        kid[i]->forceSetStill();

    _game_stopped = true;
    _main_timer->pauseTimer();
    prop->pauseTimer();
    ui._pause_button->setDisabled(true);
}

// QLink::switchGameMode, to change game mode between single player and two players
void QLink::switchGameMode() {
    if (_in_game) {
        if (!_game_stopped) pauseGame();

        QMessageBox warning(QMessageBox::Warning, QString("Warning!"),
            QString("Are you sure to switch game mode now? Your current game will be lost forever!"), 
            QMessageBox::Ok | QMessageBox::Cancel, this);
        int _return_value = warning.exec();

        switch (_return_value) {
        case QMessageBox::Cancel: return;
        case QMessageBox::Ok: clearGame(); break;
        default: throw "ERROR!";
        }
    }

    ui._pause_button->setEnabled(true);
    if (_game_stopped) ui._pause_button->setText(QString::fromLocal8Bit("继续"));
    else ui._pause_button->setText(QString::fromLocal8Bit("暂停"));

    _game_mode = 1 - _game_mode;
    updateGameMode();
}

// QLink::updateGameMode, to update the text of _game_mode_info_label
void QLink::updateGameMode() {
    switch (_game_mode) {
    case 0:
        ui._game_mode_info_label->setText(QString::fromLocal8Bit(
            "<font color=Yellow>当前游戏模式</font><br/><font color=Red>单人游戏</font>"));
        kid[1]->hide();
        break;

    case 1:
        ui._game_mode_info_label->setText(QString::fromLocal8Bit(
            "<font color=Yellow>当前游戏模式</font><br/><font color=Red>双人游戏</font>"));
        kid[1]->show();
        break;

    default:
        throw "ERROR!";
    }
}

// QLink::findKid, return a pair representing the button under kid now
// return QPair<int, int>(-1, -1) if kid isn't on any button
QPair<int, int> QLink::findKid(const QLinkCharactor* _kid_, double _width_ratio) const {
    if (!_in_game) return QPair<int, int>(-1, -1);

    QPoint _kid_position = _kid_->pos();
    int buttonWidth = getButtonWidth();
    int _kid_x_position = _kid_position.x() + buttonWidth * _kid_button_width_ratio / 2;
    int _kid_y_position = _kid_position.y() + buttonWidth * _kid_button_width_ratio * _kid_height_width_ratio / 2;
    QPair<int, int> _current_btn = findPos(QPoint(_kid_x_position, _kid_y_position), _width_ratio);
    return _current_btn.first == -1 || _simulated_data[_current_btn.first][_current_btn.second] == -1 ?
        QPair<int, int>(-1, -1) : _current_btn;
}

// QLink::findPos, return a pair representing the button on a specific position
// return QPair<int, int>(-1, -1) if there isn't any button on the position
QPair<int, int> QLink::findPos(const QPoint& _point_, double _width_ratio) const {
    if (!_in_game) return QPair<int, int>(-1, -1);

    int i = (M + 1) / 2, j = (N + 1) / 2, buttonWidth = getButtonWidth() * _width_ratio;
    auto _get_button_position = [buttonWidth](QLinkButton* btn) -> QPoint {
        QPoint pos = btn->getButtonCenter();
        pos.setX(pos.x() - buttonWidth / 2.);
        pos.setY(pos.y() - buttonWidth / 2.);
        return pos;
    };
    QPoint _button_position = _get_button_position(data[i][j]);

    bool _x_greater = _point_.x() >= _button_position.x();
    bool _y_greater = _point_.y() >= _button_position.y();

    while (i >= 1 && i <= M) {
        _button_position = _get_button_position(data[i][j]);

        if (_point_.y() >= _button_position.y() && _point_.y()
            <= _button_position.y() + buttonWidth) break;
        if (!_y_greater && _point_.y() > _button_position.y() + buttonWidth ||
            _y_greater && _point_.y() < _button_position.y())
            return QPair<int, int>(-1, -1);

        if (_point_.y() < _button_position.y()) --i;
        else ++i;
    }
    while (j >= 1 && j <= N) {
        if (i < 1 || i > M) break;
        _button_position = _get_button_position(data[i][j]);

        if (_point_.x() >= _button_position.x() && _point_.x()
            <= _button_position.x() + buttonWidth) break;
        if (!_x_greater && _point_.x() > _button_position.x() + buttonWidth ||
            _x_greater && _point_.x() < _button_position.x())
            return QPair<int, int>(-1, -1);

        if (_point_.x() < _button_position.x()) --j;
        else ++j;
    }

    if (i < 1 || i > M || j < 1 || j > N || data[i][j]->isHidden())
        return QPair<int, int>(-1, -1);
    else return QPair<int, int>(i, j);
}

/*********************  Below are QLink main algorithms  *********************/

// QLink:::resetDfsState, to prepare for next dfs
void QLink::resetDfsState() {
    for (int i = 0; i <= M + 1; ++i) {
        std::memset(_cur_turn_count[i], 0x3f, (N + 2) * sizeof(int));
    }
}

// QLink::dfs, called by function QLink::linkTwoButtons, to find a path between two buttons
bool QLink::dfs(int i, int j, int _turn_count, Direction dir, int min_count = -1, 
    const QPair<int, int>& _last_button = QPair<int, int>(-1, -1)) {
    //if (_cur_turn_count[i][j] < _turn_count || _simulated_data[i][j] || 
    //    _turn_count > _max_allow_turn) return false;
    if (_cur_turn_count[i][j] < _turn_count || _simulated_data[i][j]) return false;
    _cur_turn_count[i][j] = _turn_count;

    // record the path
    _prev_button[i][j] = QPair<int, int>(i + (dir == UP) - (dir == DOWN),
        j + (dir == LEFT) - (dir == RIGHT));

    // to interrupt whenever finding the right path
    if (i == _last_button.first && j == _last_button.second && _turn_count == min_count) return true;

    if (i - 1 >= 0 && dfs(i - 1, j, _turn_count + (dir != UP), UP, min_count, _last_button)) return true;
    if (j - 1 >= 0 && dfs(i, j - 1, _turn_count + (dir != LEFT), LEFT, min_count, _last_button)) return true;
    if (i + 1 <= M + 1 && dfs(i + 1, j, _turn_count + (dir != DOWN), DOWN, min_count, _last_button)) return true;
    if (j + 1 <= N + 1 && dfs(i, j + 1, _turn_count + (dir != RIGHT), RIGHT, min_count, _last_button)) return true;

    return false;
}

// QLink::prepareNextLink, called by _link_timer
void QLink::prepareNextLink(QTimer* _temp_timer, int _kid_no) {
    _temp_timer->stop();
    delete _temp_timer;

    QPair<int, int> _first_btn = _link_first_last.front();
    _link_first_last.pop_front();
    QPair<int, int> _last_btn = _link_first_last.front();
    _link_first_last.pop_front();

    // get the version code of linking
    int _current_layout_version = _layout_version_of_linking.front();
    _layout_version_of_linking.pop_front();

    // deal with _link_lines and calculate for the score
    int _temp_score = 1;
    _link_lines.pop_front();
    QPoint _cur_point = _link_lines.front();
    _link_lines.pop_front();
    while (!_link_lines.isEmpty() && _cur_point == _link_lines.front()) {
        _link_lines.pop_front();
        _cur_point = _link_lines.front();
        _link_lines.pop_front();
        ++_temp_score;
    }

    // the main linking function, buttons won't be linked if conditions not met
    if (_layout_version == _current_layout_version && _main_timer->isWorking()) {
        data[_first_btn.first][_first_btn.second]->hide();
        data[_last_btn.first][_last_btn.second]->hide();
        _simulated_data[_first_btn.first][_first_btn.second] = 0;
        _simulated_data[_last_btn.first][_last_btn.second] = 0;

        _left_buttons -= 2;
        score[_kid_no] += _temp_score;
        setScoreText();

        emit linkState(0);
    }
    else emit linkState(4);

#ifndef MULTITHREADS
    update();
#endif // !MULTITHREADS

    if (!_left_buttons) createGameoverWindow(0);
    checkSolution();
}

// QLink::getPath, to add lines to a list for later painting
void QLink::getPath(const QPair<int, int>& _end_btn) {
    int _button_width = data[_end_btn.first][_end_btn.second]->width() * _shown_width_ratio;

    QPair<int, int> _cur_btn = _end_btn;
    QPair<int, int> _prev_btn = _prev_button[_cur_btn.first][_cur_btn.second];
    QPoint _cur_pos = data[_cur_btn.first][_cur_btn.second]->getButtonCenter();
    QPoint _prev_pos = _cur_pos;
    _link_first_last.push_back(_cur_btn);

    while (_prev_btn.first != -1) {
        _prev_pos.setX(_prev_pos.x() + (_prev_btn.second - _cur_btn.second) * _button_width);
        _prev_pos.setY(_prev_pos.y() + (_prev_btn.first - _cur_btn.first) * _button_width);

        // add the lines to be painted
        _link_lines.push_back(_cur_pos);
        _link_lines.push_back(_prev_pos);

        // continue for the next line
        _cur_btn = _prev_btn;
        _prev_btn = _prev_button[_cur_btn.first][_cur_btn.second];
        _cur_pos = _prev_pos;
    }

    _link_first_last.push_back(_cur_btn);

    // assure the _layout_version is correct
    _layout_version_of_linking.push_back(_layout_version);

    _need_update = true;
}

// QLink::showPath, to show the path of linking two buttons
void QLink::showPath() {
    int _button_width = data[1][1]->width() * 11 / 10;

    _link_painter->setColor(Qt::red);
    _link_painter->setWidth(_button_width / 20.0);
    _link_painter->drawAllLines(_link_lines);
}

// QLink::linkTwoButtons, main algorithm, to check whether two buttons can be linked
// using dfs to find the path, see https://www.luogu.com.cn/problem/P1649
void QLink::linkTwoButtons(const QPair<int, int>& btn_1, const QPair<int, int>& btn_2, int _kid_no) {
    if (_simulated_data[btn_1.first][btn_1.second] != _simulated_data[btn_2.first][btn_2.second]) {
        emit linkState(1); return;
    }

    // record some data in advance for later recovery
    int _kind_of_btn_1 = _simulated_data[btn_1.first][btn_1.second];
    int _kind_of_btn_2 = _simulated_data[btn_2.first][btn_2.second];
    _simulated_data[btn_1.first][btn_1.second] = 0;
    _simulated_data[btn_2.first][btn_2.second] = 0;
    
    // the first dfs is used to find the minimum turning count
    dfs(btn_1.first, btn_1.second, -1, NONE);

    if (_cur_turn_count[btn_2.first][btn_2.second] <= _max_allow_turn) {
        int min_count = _cur_turn_count[btn_2.first][btn_2.second];

        // the second dfs is used to find the right path with the minimun turning count
        dfs(btn_1.first, btn_1.second, -1, NONE, min_count, btn_2);
        _prev_button[btn_1.first][btn_1.second] = QPair<int, int>(-1, -1);
        getPath(btn_2);

        QTimer* _temp_timer = new QTimer(this);
        connect(_temp_timer, &QTimer::timeout, this,
            std::bind(&QLink::prepareNextLink, this, _temp_timer, _kid_no));
        _temp_timer->start(500);
    }
    else if (_cur_turn_count[btn_2.first][btn_2.second] < INF) emit linkState(2);
    else emit linkState(3);

    _simulated_data[btn_1.first][btn_1.second] = _kind_of_btn_1;
    _simulated_data[btn_2.first][btn_2.second] = _kind_of_btn_2;
    resetDfsState();
}

// QLink::setLinkState, a slot used to change the text of the _link_state_edit
void QLink::setLinkState(int _state_code) {
    static const char* _state_text[] = {
        "Link Success", 
        "Type Disagree", 
        "Turn Limited", 
        "No Direct Path",
        "Linking Interrupted"
    };

    /************************************ /
    // or use Chinese version instead
    static const char* _state_text[] = {
        "连接成功", 
        "类型不匹配", 
        "弯折次数超过限制", 
        "不存在直接路径",
        "连接被打断"
    };
    / ************************************/
    
    // a way to demonstrate Chinese charactors correctly
    ui._link_state_edit->setText(QString::fromLocal8Bit(_state_text[_state_code]));
}

/**********************  Below are QLinkProp functions  **********************/

// QLink::correctKidPos, to avoid the kid to be stuck in a button when shuffling
void QLink::correctKidPos() {
    int _step_interval = getButtonWidth() * (_shown_width_ratio - 1);
    for (int i = 0; i <= _game_mode; ++i)
        while (findKid(kid[i]).first != -1)
            kid[i]->moveCharactorTo(kid[i]->x() + _step_interval, kid[i]->y());
}

// QLink::checkProp, a method to check if the prop need take effect
void QLink::checkProp() {
    for (int i = 0; i < _kid_num; ++i)
        if (prop->isOnProp(kid[i])) {
            // some special conditions
            if (prop->getPropType() == 1) {
                ++_layout_version;
                _first_highlight_btn = QPair<int, int>(-1, -1);
            }

            // let the prop take effect
            prop->takeEffect(i);
            checkSolution();
            correctKidPos();
        }
}

// QLink::checkSolution, if there's no solution possible, execute some functions
void QLink::checkSolution() {
    // if the palyer wins
    if (!_left_buttons) return;

    if (!prop->checkSolution()) {
        if (!_game_stopped && ui._no_pair_label->isHidden()) pauseGame();
        ui._no_pair_label->show();
    }
    else ui._no_pair_label->hide();
}

/********************  Below are QLinkFile main functions  *******************/

// QLink::saveToFile, to save the current game data to a file
void QLink::saveToFile() {
    if (!_in_game) {
        QMessageBox warning(QMessageBox::Warning, QString("Warning!"), 
            QString("There is no game to save!"), QMessageBox::Ok, this);
        warning.exec();
        return;
    }

    if (!_game_stopped) pauseGame();

    QString _file_path = QFileDialog::getSaveFileName(this, 
        "Choose The Path", "../data", "QLINK(*.qlink)");

    if (_file_path == "") return;
    
    std::fstream _game_file(_file_path.toStdWString(), 
        std::ios::out | std::ios::binary);
    
    _FILE_STRUCTURE content;
    saveDataToStructure(content);

    _game_file.write((char*)&content, sizeof(_FILE_STRUCTURE));
    _game_file.close();
}

// QLink::saveDataToStructure, called by QLink::saveToFile, to save the game data in a structure
void QLink::saveDataToStructure(_FILE_STRUCTURE& content) const {
    // all kinds of game data is saved to the structure
    content.mode = _game_mode;
    content.M = M;
    content.N = N;
    content._left_time = _main_timer->getLeftSeconds();
    content._width_of_kid = kid[0]->width();

    for (int i = 0; i < _kid_num; ++i) {
        if (i > _game_mode) break;

        content._kid_pos[i] = kid[i]->getInnerXY();
        content.score[i] = score[i];
    }

    for (int i = 1; i <= M; ++i) {
        for (int j = 1; j <= N; ++j) {
            content._cur_data[i][j] = _simulated_data[i][j];
        }
    }

    content._used_icon_num = icon._max_icon_num;
    for (int i = 0; i < content._used_icon_num; ++i) {
        content._icon_order[i] = icon.map[i];
    }

    content._hash_value = getHashValue(content);
}

// QLink::getHashValue, to check whether the game file can be read correctly
long long QLink::getHashValue(const _FILE_STRUCTURE& content) const {
    static const long long mod = 1145141919810;
    long long base = content.M * content.N, _cur_base = 1;
    long long _hash_value = 1;

    if (content.M <= 0 || content.M > _MAX_M || 
        content.N <= 0 || content.N > _MAX_N) return -1;

    for (int i = 1; i <= content.M; ++i) {
        for (int j = 1; j <= content.N; ++j) {
            _hash_value += content._cur_data[i][j] * _cur_base;
            _hash_value %= mod;
            _cur_base *= base;
            _cur_base %= mod;
        }
    }

    _hash_value += qPow(content._left_time, 2);
    for (int i = 0; i < content.mode; ++i) {
        _hash_value += qPow(content.score[i], 2);
        _hash_value %= mod;
    }

    return (_hash_value + mod) % mod;
}

// QLink::loadFromFile, to load the game from a previously saved file
void QLink::loadFromFile() {
    if (_in_game) {
        if (!_game_stopped) pauseGame();

        QMessageBox warning(QMessageBox::Warning, QString("Warning!"),
            QString("Are you sure to load a game from a file? Your current game will be lost forever!"), 
            QMessageBox::Ok | QMessageBox::Cancel, this);
        int _return_value = warning.exec();

        switch (_return_value) {
        case QMessageBox::Cancel: return;
        case QMessageBox::Ok: break;
        default: throw "ERROR!";
        }
    }

    QString _file_path = QFileDialog::getOpenFileName(this,
        "Choose The File", "../data", "QLINK(*.qlink)");

    if (_file_path == "") return;

    _FILE_STRUCTURE content;
    std::fstream _game_file(_file_path.toStdWString(), std::ios::in | std::ios::binary);
    if (!_game_file) {
        QMessageBox warning(QMessageBox::Warning, QString("Warning!"),
            QString("The file are not opened correctly"), QMessageBox::Ok, this);
        warning.exec();
        return;
    }

    _game_file.read((char*)&content, sizeof(_FILE_STRUCTURE));
    _game_file.close();

    loadDataFromStructure(content);
}

// QLink::loadDataFromStructure, called by QLink::loadFromFile, to load the game from a structure
void QLink::loadDataFromStructure(const _FILE_STRUCTURE& content) {
    if (content._hash_value != getHashValue(content) || content._hash_value < 0) {
        QMessageBox warning(QMessageBox::Warning, QString("Warning!"),
            QString("Game loading failed! This game file is broken or incorrect!"),
            QMessageBox::Ok | QMessageBox::Cancel, this);
        warning.exec();
        return;
    }

    icon.randomOrder(content._icon_order, content._used_icon_num);
    startGame(content.M, content.N, content._cur_data);

    for (int i = 0; i < content.mode; ++i)
        score[i] = content.score[i];
    setScoreText();

    _main_timer->resetTimer(content._left_time);
    _main_timer->updateTime();

    _game_mode = content.mode;
    updateGameMode();

    for (int i = 0; i < _kid_num; ++i) {
        if (i > _game_mode) break;

        kid[i]->setInnerXY(content._kid_pos[i]);
        kid[i]->resize(content._width_of_kid, content._width_of_kid * _kid_height_width_ratio);
        kid[i]->repaintCharactor();
    }
    pauseGame();

    if (!_left_buttons) createGameoverWindow(0);
    checkSolution();
}

/************************  Below are QLink key event  ************************/

// QLink::buttonPressed, to deal with button pressing events
void QLink::buttonPressed(QPair<int, int>& _pressed_btn, QPair<int, int>& _pressing_btn, int _kid_no) {
    if (_pressed_btn.first == -1) {
        data[_pressing_btn.first][_pressing_btn.second]->Press();
        _pressed_btn = _pressing_btn;
    }
    else if (_pressing_btn == _pressed_btn) {
        data[_pressing_btn.first][_pressing_btn.second]->Press();
        _pressed_btn = QPair<int, int>(-1, -1);
    }
    else {
        data[_pressed_btn.first][_pressed_btn.second]->Press();
        linkTwoButtons(_pressed_btn, _pressing_btn, _kid_no);
        _pressed_btn = QPair<int, int>(-1, -1);
    }
}

// QLink::keyPressSetMove, to deal with the kids' movement when any key is pressed
void QLink::keyPressSetMove(int _kid_no, int _cur_key, bool isRepeated) {
    // to apply to all game modes
    if (_kid_no > _game_mode) return;

    // if the charactor is frozen
    if (freeze[_kid_no]) return;

    // temp is used for save the value of findKid function
    QPair<int, int> temp;

    // prepare keys in advance
    static const Qt::Key keys[][10] = {
        { Qt::Key_W, Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_J, 
            Qt::Key_S, Qt::Key_D, Qt::Key_W, Qt::Key_A, Qt::Key_J }, 
        { Qt::Key_Up, Qt::Key_Left, Qt::Key_Down, Qt::Key_Right, Qt::Key_0, 
            Qt::Key_Down, Qt::Key_Right, Qt::Key_Up, Qt::Key_Left, Qt::Key_0 }
    };
    
    // to iterate the keys to deal with
    for (int i : { 0, 1, 2, 3 })
        if (_cur_key == keys[_kid_no][i + dizzy[_kid_no] * 5]) {
            kid[_kid_no]->setMove(i);
            break;
        }
    if (_cur_key == keys[_kid_no][4 + dizzy[_kid_no] * 5]) {
        temp = findKid(kid[_kid_no], _find_kid_ratio);
        if (temp.first != -1 && temp != _cur_pressed_button[_kid_no] &&
            !_link_first_last.contains(temp) && !isRepeated) {
            buttonPressed(_cur_pressed_button[_kid_no], temp, _kid_no);
        }
    }
}

// QLink:keyReleaseSetStill, to deal with the kids' stopping when any key is released
void QLink::keyReleaseSetStill(int _kid_no, int _cur_key) {
    // if the charactor is frozen
    if (freeze[_kid_no]) return;

    // prepare keys in advance
    static const Qt::Key keys[][10] = {
        { Qt::Key_W, Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_J,
            Qt::Key_S, Qt::Key_D, Qt::Key_W, Qt::Key_A, Qt::Key_J },
        { Qt::Key_Up, Qt::Key_Left, Qt::Key_Down, Qt::Key_Right, Qt::Key_0,
            Qt::Key_Down, Qt::Key_Right, Qt::Key_Up, Qt::Key_Left, Qt::Key_0 }
    };

    for (int i : { 0, 1, 2, 3 }) {
        if (_cur_key == keys[_kid_no][i + dizzy[_kid_no] * 5]) {
            kid[_kid_no]->setStill(i);
            break;
        }
    }
}

void QLink::keyPressEvent(QKeyEvent* event) {
    if (_game_stopped) return;

    // if (event->isAutoRepeat() == true) return;
    bool isRepeated = event->isAutoRepeat();

    // deal with the key press event
    for (int i = 0; i <= _game_mode; ++i)
        keyPressSetMove(i, event->key(), isRepeated);
}

void QLink::keyReleaseEvent(QKeyEvent* event) {
    // assure the event is the release-event
    if (event->isAutoRepeat() == true) return;

    // key release events
    for (int i = 0; i <= _game_mode; ++i)
        keyReleaseSetStill(i, event->key());
}

// QLink::eventFilter, to provide QLabel with a method to check if clicked
bool QLink::eventFilter(QObject* watched, QEvent* event) {
    if (watched == _init_class._start_label) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* _mouse_event = static_cast<QMouseEvent*>(event);
            if (_mouse_event->button() == Qt::LeftButton) {
                prepareGame(); return true;
            }
            else return false;
        }
        else return false;
    }
    else if (watched == ui._no_pair_label) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* _mouse_event = static_cast<QMouseEvent*>(event);
            if (_mouse_event->button() == Qt::LeftButton) {
                createGameoverWindow(1); return true;
            }
            else return false;
        }
        else return false;
    }
    else return QMainWindow::eventFilter(watched, event);
}

/***********************  Below are QLinkThread slots  ***********************/

// QLink::emitPaintingSignal, to prepare parameters and emit signal
void QLink::emitPaintingSignal() {
    QList<QPair<QList<QPoint>, QColor>> list;

    // highlight the hinted buttons
    if (_first_highlight_btn.first != -1) {
        if (data[_first_highlight_btn.first][_first_highlight_btn.second]->isHidden() ||
            data[_second_highlight_btn.first][_second_highlight_btn.second]->isHidden()) {
            _first_highlight_btn = _second_highlight_btn = QPair<int, int>(-1, -1);
        }
        else {
            QList<QPoint> _hinted_btn;
            _hinted_btn.append(data[_first_highlight_btn.first][_first_highlight_btn.second]->getEdgeLines());
            _hinted_btn.append(data[_second_highlight_btn.first][_second_highlight_btn.second]->getEdgeLines());
            list.push_back(qMakePair(_hinted_btn, Qt::yellow));
        }
    }

    static const QColor _color_btn_to_set[_kid_num]{ Qt::blue, Qt::blue };
    static const QColor _color_pressed_btn[_kid_num]{ Qt::black, Qt::white };

    // call off QLink::findKid() and QLinkButton::setOnState() functions
    for (int i = 0; i < _kid_num; ++i) {
        if (i > _game_mode) break;

        QPair<int, int> _button_to_set = findKid(kid[i], _find_kid_ratio);
        if (_button_to_set.first != -1 && data[_button_to_set.first][_button_to_set.second]) {
            if (!data[_button_to_set.first][_button_to_set.second]->isHidden())
                list.push_back(qMakePair(data[_button_to_set.first][_button_to_set.second]
                    ->getEdgeLines(), _color_btn_to_set[i]));
        }

        if (_cur_pressed_button[i].first != -1) {
            list.push_back(qMakePair(data[_cur_pressed_button[i].first][_cur_pressed_button[i].second]
                ->getEdgeLines(), _color_pressed_btn[i]));
        }
    }

    // adding linking path
    list.append(qMakePair(_link_lines, Qt::red));

    emit getPixmap(width(), height(), getButtonWidth() / 5.0, list);
}

// QLink::setPixmap, a slot to draw the QPixmap, using value-passing due to different threads
void QLink::setPixmap(QPixmap map, QRegion region) {
    this->_painter_map = map;
    update(region);
}

/**********************  Below are QLinkTest functions  **********************/

// QLink::initializeTest, to initialize some basic parameters
void QLink::initializeTest() {
    // initialize parameters of the game
    icon.resize(6);
    static const int _test_data[_MAX_M + 1][_MAX_N + 1] = {
        { 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, -1, 1, 2, -1, -1 },
        { 0, 3, 2, 4, 3, 5, 4 },
        { 0, 2, 3, -1, 4, 4, 6 },
        { 0, 4, 6, 3, 1, 5, 5 },
        { 0, 3, 6, 2, 6, 3, 1 },
        { 0, 5, -1, 4, 4, 4, -1 }
    };

    // M = 6, N = 6, _used_kinds = 6
    startGame(6, 6, _test_data);

    // remove the influence brought by QLinkProp
    prop->pauseTimer();

    // time is given enough for the test
    _main_timer->resetTimer(3600);
}

// QLink::linkTwoButtonsTest, called by QLinkTest, return different value for different linking state
/************************************************************************************************** /
            return value    meaning
            0               chosen buttons are of different types
            1               there's no path between chosen buttons
            2               the turning count of the path between chosen buttons exceeds limit
            3               the turning count of the path between chosen buttons is 2
            4               the turning count of the path between chosen buttons is 1
            5               the turning count of the path between chosen buttons is 0
/ **************************************************************************************************/
int QLink::linkTwoButtonsTest(const QPair<int, int>& btn_1, const QPair<int, int>& btn_2) {
    if (_simulated_data[btn_1.first][btn_1.second] != _simulated_data[btn_2.first][btn_2.second])
        return 0;

    // record some data in advance for later recovery
    int _kind_of_btn_1 = _simulated_data[btn_1.first][btn_1.second];
    int _kind_of_btn_2 = _simulated_data[btn_2.first][btn_2.second];
    _simulated_data[btn_1.first][btn_1.second] = 0;
    _simulated_data[btn_2.first][btn_2.second] = 0;

    // the first dfs is used to find the minimum turning count
    dfs(btn_1.first, btn_1.second, -1, NONE);

    int _return_value;

    // judge whether the condition is satisfied
    if (_cur_turn_count[btn_2.first][btn_2.second] <= _max_allow_turn) {
        int min_count = _cur_turn_count[btn_2.first][btn_2.second];

        // erase the linking buttons
        data[btn_1.first][btn_1.second]->hide();
        data[btn_2.first][btn_2.second]->hide();
        _simulated_data[btn_1.first][btn_1.second] = 0;
        _simulated_data[btn_2.first][btn_2.second] = 0;

        _left_buttons -= 2;
        resetDfsState();

        return 5 - min_count;
    }
    else if (_cur_turn_count[btn_2.first][btn_2.second] < INF) _return_value = 2;
    else _return_value = 1;

    _simulated_data[btn_1.first][btn_1.second] = _kind_of_btn_1;
    _simulated_data[btn_2.first][btn_2.second] = _kind_of_btn_2;
    resetDfsState();

    return _return_value;
}

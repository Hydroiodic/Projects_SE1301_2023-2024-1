#include "Qt_Exam.h"

Qt_Exam::Qt_Exam(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);

	// initialize this program
	initializeTricks();
	initializeMenuBar();
}

Qt_Exam::~Qt_Exam() {
#ifdef MULTITHREADS
	// wait for the thread to quit
	thread->quit();
	thread->wait();
	delete thread;
#endif // MULTITHREADS
}

void Qt_Exam::initializeTricks() {
	// connect signals with slots
	connect(&qdraw_map, &QtDrawMap::sendPixMap,
		this, &Qt_Exam::setBg);
	connect(this, &Qt_Exam::requestBg,
		&qdraw_map, &QtDrawMap::drawPixmap);

#ifdef MULTITHREADS
	// multi-threads operation
	thread = new QThread;
	qdraw_map.moveToThread(thread);
	thread->start();
#endif // MULTITHREADS

	// finish initialization
	initialized = true;
}

void Qt_Exam::initializeMenuBar() {
	QMenuBar* menubar = new QMenuBar(this);
	QMenu* menu = new QMenu(this);
	QAction* action = new QAction(this);

	connect(action, &QAction::triggered, this, &Qt_Exam::loadFromFile);
	action->setText("Load from...(&L)");
	action->setShortcut(Qt::CTRL | Qt::Key_L);

	menu->addAction(action);
	menu->setTitle("Operations");

	menubar->addMenu(menu);

	this->setMenuBar(menubar);
}

void Qt_Exam::paintEvent(QPaintEvent* event) {
	QPainter painter(this);

	if (M <= 15) {
		painter.drawPixmap(0, 0, background);
	}
}

void Qt_Exam::resizeEvent(QResizeEvent* event) {
	if (initialized && M <= 15) {
		prepareUI();
	}
}

void Qt_Exam::keyPressEvent(QKeyEvent* event) {
	// not accept repeatedly
	if (event->isAutoRepeat()) {
		return;
	}

	switch (event->key()) {
	case Qt::Key_T:
		safety_shown = !safety_shown;
		prepareUI();
		break;

	case Qt::Key_P:
		findPath();
		break;

	default:
		QMainWindow::keyPressEvent(event);
	}
}

void Qt_Exam::prepareSafetyValue() {
	QQueue<QPair<int, int>> queue;
	bool checked[MAX_M][MAX_N] = {};

	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < N; ++j) {
			queue.push_back(qMakePair(i, j));

			while (!queue.empty()) {
				QPair<int, int> temp = queue.front();
				queue.pop_front();

				if (data[temp.first][temp.second]) {
					safety[i][j] = qAbs(temp.first - i) + qAbs(temp.second - j);
					queue.clear();

					for (int k = 0; k < M; ++k) {
						memset(checked[k], false, sizeof(bool) * MAX_N);
					}

					break;
				}

				checked[temp.first][temp.second] = true;

				// push new pairs into the stack
				if (temp.first - 1 < M && !checked[temp.first - 1][temp.second]) {
					queue.push_back(qMakePair(temp.first - 1, temp.second));
				}
				if (temp.first + 1 < M && !checked[temp.first + 1][temp.second]) {
					queue.push_back(qMakePair(temp.first + 1, temp.second));
				}
				if (temp.second - 1 < N && !checked[temp.first][temp.second - 1]) {
					queue.push_back(qMakePair(temp.first, temp.second - 1));
				}
				if (temp.second + 1 < N && !checked[temp.first][temp.second + 1]) {
					queue.push_back(qMakePair(temp.first, temp.second + 1));
				}
			}
		}
	}
}

void Qt_Exam::prepareUI() {
	resizeAll();
	blocks_list.clear();

	// find the data to fill the list
	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < N; ++j) {
			QBlockData temp_data;
			temp_data.x = j;
			temp_data.y = i;
			temp_data.color = BlockColor(data[i][j]);
			temp_data.value = safety[i][j];
			temp_data.valueShown = safety_shown;

			blocks_list.push_back(temp_data);
		}
	}

	emit requestBg(this->width(), this->height(), blocks_list);
}

void Qt_Exam::findPath() {
	QList<QPair<int, int>> list;

	bool result = recordPath(0, 0, list);

	if (!result) {
		Warning("No path found!");
		return;
	}

	while (!list.empty()) {
		QPair<int, int> temp = list.back();
		list.pop_back();

		data[temp.first][temp.second] = 2;
	}

	prepareUI();
}

bool Qt_Exam::recordPath(int i, int j, QList<QPair<int, int>>& list) const {
	if (list.contains(qMakePair(i, j))) {
		return false;
	}

	if (safety[i][j] < min_safety) {
		return false;
	}

	list.push_back(qMakePair(i, j));

	if (i == M - 1 && j == N - 1) {
		return true;
	}

	bool result = false;

	// push new pairs into the stack
	if (!result && i - 1 < M && !list.contains(qMakePair(i - 1, j))) {
		result |= recordPath(i - 1, j, list);
	}
	if (!result && i + 1 < M && !list.contains(qMakePair(i + 1, j))) {
		result |= recordPath(i + 1, j, list);
	}
	if (!result && j - 1 < N && !list.contains(qMakePair(i, j - 1))) {
		result |= recordPath(i, j - 1, list);
	}
	if (!result && j + 1 < N && !list.contains(qMakePair(i, j + 1))) {
		result |= recordPath(i, j + 1, list);
	}

	if (!result) {
		list.pop_back();
	}

	return result;
}

void Qt_Exam::Warning(const QString& msg) const {
	QMessageBox msg_box(QMessageBox::Warning, "Warning", msg);
	msg_box.exec();
}

void Qt_Exam::loadFromFile() {
	QString _file_path = QFileDialog::getOpenFileName(this,
		"Choose The File", "../data");

	// nothing get
	if (_file_path == "") return;

	// open the file
	std::fstream fs(_file_path.toStdWString(), std::ios::in);

	// file not open correctly
	if (!fs) {
		Warning("The file are not opened correctly! Please try again!");
		return;
	}

	size_t new_M = -1, new_N = -1;
	fs >> new_M >> new_N;

	if (!fs || new_M && new_M > 15 || new_N && new_N > 15) {
		Warning("The data stored in the file isn't correct!");
		return;
	}

	// input the data
	for (int i = 0; i < new_M; ++i) {
		for (int j = 0; j < new_N; ++j) {
			fs >> data[i][j];

			if (data[i][j] != 0 && data[i][j] != 1) {
				Warning("The data stored in the file isn't correct!");
				return;
			}
		}
	}

	// input the variable "safety"
	fs >> min_safety;
	if (!fs) {
		Warning("The data stored in the file isn't correct!");
		return;
	}

	M = new_M;
	N = new_N;
	safety_shown = false;

	prepareSafetyValue();
	prepareUI();
}

void Qt_Exam::resizeAll() const {
	QDrawMapSettings new_setting;
	new_setting.M = M;
	new_setting.N = N;
	qdraw_map.config(new_setting);
}

void Qt_Exam::setBg(const QPixmap map) {
	// set the local background to the new one
	this->background = map;

	// update the window to take effect
	update();
}

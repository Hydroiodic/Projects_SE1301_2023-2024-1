// QLinkInitPage.cpp
// set an initial page for the game at the start of the program

#include "QLinkInitPage.h"

QLinkInitPage::QLinkInitPage(QObject* parent) : QObject(parent) {
    _start_widget = new QWidget;
    _start_label = new QLabel(_start_widget);
    _start_layout = new QGridLayout;
    _start_timer = new QTimer;

    QFont font;
    font.setPointSize(50);
    font.setBold(true);
    _start_label->setFont(font);
    _start_label->setTextFormat(Qt::MarkdownText);
    _start_label->setAlignment(Qt::AlignCenter);
    _start_label->setText(text);
    _start_label->adjustSize();

    _start_layout->addWidget(_start_label);
    _start_widget->setLayout(_start_layout);

    setLabel();
}

QLinkInitPage::~QLinkInitPage() {
    delete _start_timer;

    if (!_game_initialized) {
        delete _start_label;
        delete _start_layout;
        delete _start_widget;
    }
}

// QLinkInitPage::changeLabelState, to represent the twinkling effect
void QLinkInitPage::changeLabelState() {
    static int _label_state = 0;

    if (++_label_state %= 2) _start_label->setText("");
    else _start_label->setText(text);
}

// QLinkInitPage::setLabel, to start twinkling
void QLinkInitPage::setLabel() {
    connect(_start_timer, &QTimer::timeout, this, &QLinkInitPage::changeLabelState);
    this->_start_timer->start(500);
}

// QLinkInitPage::stopLabel, to stop twinkling
void QLinkInitPage::stopLabel() {
    _start_timer->stop();
}

// QLinkButton.cpp
// a button providing methods for the main window to use

#include "QLinkButton.h"

QLinkButton::QLinkButton(QWidget* parent) : QPushButton(parent) {
    painter = new QLinkPainter(parent);
    updateEdge();
}

QLinkButton::~QLinkButton() {
    delete painter;
}

// QLinkButton::updateEdge, to update _edge_lines when the size of button changes
void QLinkButton::updateEdge() {
    int buttonWidth = width();

    QPoint position = pos();
    QPoint left_top(position.x(), position.y());
    QPoint right_top(position.x() + buttonWidth, position.y());
    QPoint left_bottom(position.x(), position.y() + buttonWidth);
    QPoint right_bottom(position.x() + buttonWidth, position.y() + buttonWidth);

    _edge_lines.clear();
    _edge_lines.append(left_top);
    _edge_lines.append(right_top);
    _edge_lines.append(right_top);
    _edge_lines.append(right_bottom);
    _edge_lines.append(right_bottom);
    _edge_lines.append(left_bottom);
    _edge_lines.append(left_bottom);
    _edge_lines.append(left_top);
}

// QLinkButton::setState, to draw a box for the button with a specific color
void QLinkButton::setState(const QColor& color = QColor(Qt::blue)) {
    int buttonWidth = width();
    QPoint position = pos();

    painter->setColor(color);
    painter->setWidth(buttonWidth / 5.0);
    painter->drawAllLines(_edge_lines);

    // rect is the area for updating only once
    QRect rect(position.x() - buttonWidth / 10, position.y() - buttonWidth / 10, 
        buttonWidth * 6 / 5, buttonWidth * 6 / 5);
    this->parentWidget()->update(rect);
}

// QLinkButton::repaintButton, based on QLinkButton::setState, return when the button is hidden
void QLinkButton::repaintButton(const QColor& color = QColor(Qt::blue)) {
    if (isHidden()) return;
    if (!isPressed) setState();
}

// QLinkButton::Press, to change the state of the button by invert the variable isPressed
void QLinkButton::Press() {
    isPressed = !isPressed;
}

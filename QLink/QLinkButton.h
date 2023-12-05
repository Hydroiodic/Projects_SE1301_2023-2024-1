// QLinkButton.h
// header file for QLinkButton.cpp

#pragma once

#include <QPainter>
#include <QPen>
#include "QLink.h"
#include "QLinkPainter.h"
#include "QLinkSettings.h"

#ifdef MULTITHREADS
#include <QThread>
#endif // MULTITHREADS

class QLink;

// class QLinkButton, nearly being the same as QPushButton
// additional data members included
class QLinkButton : public QPushButton
{
private:
    QLinkPainter* painter = nullptr;
    QList<QPoint> _edge_lines;

    // isPress is used to judge whether the button has been selected
    bool isPressed = false;

    QThread* thread = nullptr;

public:
    QLinkButton(QWidget* parent = nullptr);
    ~QLinkButton();

    // used when the main charactor interact with the button
    void Press();
    void setState(const QColor&);
    void repaintButton(const QColor&);
    void updateEdge();

    inline const QList<QPoint>& getEdgeLines() const {
        return _edge_lines;
    }
    inline QPoint getButtonCenter() {
        QPoint position = pos();
        return QPoint(position.x() + width() / 2, position.y() + height() / 2);
    }
};

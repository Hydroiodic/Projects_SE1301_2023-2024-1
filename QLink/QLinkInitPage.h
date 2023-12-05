// QLinkInitPage.h
// header file for QLinkInitPage.cpp

#pragma once

#include <QObject>
#include <QLabel>
#include <QGridLayout>
#include <QTimer>

class QLinkInitPage : public QObject
{
    Q_OBJECT

private:
    friend class QLink;

    QLabel* _start_label;
    QWidget* _start_widget;
    QGridLayout* _start_layout;
    QTimer* _start_timer;

    const QString text = "<font color=Blue>QLink</font><br/>Click To Start";

    // a variable to judge whether to delete
    bool _game_initialized = false;

    QLinkInitPage(QObject* parent = nullptr);
    ~QLinkInitPage();

    void setLabel();
    void stopLabel();

private slots:
    void changeLabelState();
};

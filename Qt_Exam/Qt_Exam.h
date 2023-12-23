#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Qt_Exam.h"

class Qt_Exam : public QMainWindow
{
	Q_OBJECT

public:
	Qt_Exam(QWidget* parent = nullptr);
	~Qt_Exam();

private:
	Ui::Qt_ExamClass ui;
};

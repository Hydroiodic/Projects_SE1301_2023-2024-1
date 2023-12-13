// QBasicController.h
// header file for QBasicController.cpp

#pragma once

#include <QObject>
#include <QFileDialog>
#include <fstream>
#include "QBasic.h"
#include "QBasicInform.h"
#include "exceptions.h"

class QBasic;

class QBasicStack :public QObject
{
	Q_OBJECT

private:
	size_t lines_executed = 0ull;
	static const size_t MAX_STACK = 0xffffull;

public:
	void increaseStack() {
		if (!checkStack())
			throw exceptions::stack_overflow();
		++lines_executed;

	}
	void resetStack() {
		lines_executed = 0ull;
	}
	bool checkStack() {
		return lines_executed <= MAX_STACK;
	}
};

class QBasicController : public QObject
{
	Q_OBJECT

private:
	QBasic* basic = nullptr;

private:
	QBasicStack* stack;

public:
	QBasicController(QObject* parent, QBasic* basic);
	~QBasicController();

	/*************** below are the control functions of QBasic ***************/

public:
	void run() const;
	void load() const;
	void list() const;
	void clear() const;
	void help() const;
	void quit() const;
	void execute(int no) const;

private:
	void (QBasicController::* (func)[6])() const = {
		&QBasicController::run,
		&QBasicController::load,
		&QBasicController::list,
		&QBasicController::clear,
		&QBasicController::help,
		&QBasicController::quit
	};

	// function to load codes to QBasicExpression
	void loadExpressions() const;
};

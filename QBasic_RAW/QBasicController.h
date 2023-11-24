#pragma once

#include <QObject>
#include "QBasic.h"

class QBasic;

class QBasicController : public QObject
{
	Q_OBJECT

private:
	QBasic* basic = nullptr;

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
};

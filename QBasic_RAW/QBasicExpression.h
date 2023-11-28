#pragma once

#include <QObject>
#include "QBasicVarList.h"
#include "expressions.h"

using namespace expressions;

class QBasic;

class QBasicExpression : public QObject
{
	Q_OBJECT

public:
	QBasicExpression(QObject* parent, QBasic* b);
	~QBasicExpression();

private:
	struct exp_pack {
		int line_number = -1;
		commands::IMPL type = commands::IMPL(0);
		Expression* exp = nullptr;
		STATE_EXP state = STATE_EXP::ERROR;
	};

	std::vector<exp_pack> exp_list;

private:
	QBasic* basic = nullptr;

public:
	// to create an expression object of type implement
	void addExp(commands::IMPL t, const QString& str, int line, commands::STATE s);
	int executeExp(int index);
	commands::IMPL getExpType(int index) const;
	void clearExp();

public:
	QString getExpTree(int index) const;
};

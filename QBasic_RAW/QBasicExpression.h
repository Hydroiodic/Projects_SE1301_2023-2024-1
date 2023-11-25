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
	int line_number = -1;
	Expression* exp = nullptr;
	STATE_EXP state = STATE_EXP::ERROR;

private:
	QBasic* basic = nullptr;

public:
	// to create an expression object of type implement
	// TODO: create one for the type instant
	void loadExp(commands::IMPL t, const QString& str, int line);
	int executeExp();
	void clearExp();

public:
	QString getExpTree() const;
};

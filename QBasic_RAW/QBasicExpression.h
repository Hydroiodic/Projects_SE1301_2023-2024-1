#pragma once

#include <QObject>
#include "expressions.h"

using namespace expressions;

class QBasicExpression : public QObject
{
	Q_OBJECT

public:
	QBasicExpression(QObject* parent = nullptr);
	~QBasicExpression();

private:
	int line_number = -1;
	Expression* exp = nullptr;
	STATE_EXP state = ERROR;

public:
	// to create an expression object of type implement
	// TODO: create one for the type instant
	void loadExp(commands::IMPL t, const QString& str, int line);
	void executeExp();
	void clearExp();

public:
	QString getExpTree() const;
};



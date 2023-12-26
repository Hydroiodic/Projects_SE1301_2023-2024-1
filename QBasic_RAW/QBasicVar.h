// QBasicVar.h
// header file for QBasicVar.cpp

#pragma once

#include <QObject>
#include <QString>
#include <vector>
#include "exceptions.h"
#include "variables.h"

using namespace variables;

template <typename T>
class QBasicVariable : public QObject
{
public:
	QBasicVariable(QObject* parent, const QString& name) : QObject(parent), name(name) {}
	QBasicVariable(QObject* parent, const QString& name, T value)
		: QObject(parent), name(name), data(value), state(STATE::DEF) {}
	~QBasicVariable() {}

	// assign a value to the variable
	void assign(T value);

	// fetch the value of the variable
	T fetchValue();

	// get used count of a variable
	int fetchCount() const;

	// reset used count of a variable
	void resetCount();

	// fetch the name of the variable
	QString fetchName() const;

	// check the name of the variable
	bool checkVar() const;

	// check the state of the variable
	bool checkState() const;

private:
	// save the name of the variable
	QString name;

	// save the state of the variable
	STATE state = STATE::UNDEF;

	// save the data of the variable
	T data;

	// save the count that the variable is used
	int use_counts = 0;
};

// because of the template attribute of the class QBasicVariable, 
//     the .cpp file should be included here
#include "QBasicVar.cpp"

// in this program, all variables are set as the type "int"
using QBasicVar = QBasicVariable<int>;
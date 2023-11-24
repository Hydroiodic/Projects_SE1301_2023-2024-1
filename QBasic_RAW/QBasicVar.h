#pragma once

#include <QObject>
#include <QString>
#include <vector>
#include "exceptions.h"

namespace variables {
	const std::vector<QString> key_words = {
		"REM",
		"LET",
		"PRINT",
		"INPUT",
		"GOTO",
		"IF",
		"THEN",
		"END",
		"RUN",
		"LOAD",
		"LIST",
		"CLEAR",
		"HELP",
		"QUIT"
	};

	enum STATE {
		UNDEF, DEF
	};

	template <typename T>
	class QBasicVar : public QObject
	{
	public:
		QBasicVar(QObject* parent, const QString& name) : QObject(parent), name(name) {}
		QBasicVar(QObject* parent, const QString& name, T value) 
			: QObject(parent), name(name), data(value), state(STATE::DEF) {}
		~QBasicVar() {};

		// assign a value to the variable
		void assign(T value);

		// fetch the value of the variable
		T fetchValue() const;

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
	};
}

namespace variables {
	template <typename T>
	void QBasicVar<T>::assign(T value) {
		data = value;
		state = STATE::DEF;
	}

	template <typename T>
	bool QBasicVar<T>::checkVar() const {
		// the name of a variable shouldn't be started with a number
		if (name[0] >= '0' && name[0] <= '9') return false;

		// key_words shouldn't be used
		for (int i = 0; i < key_words.size(); ++i) {
			if (key_words[i] == name) return false;
		}

		// the charactor consisting of the string should be right
		for (int i = 0; i < name.length(); ++i) {
			if (!(name[i] <= 'Z' && name[i] >= 'A' ||
				name[i] <= 'z' && name[i] >= 'a' ||
				name[i] <= '9' && name[i] >= '0' ||
				name[i] == '_')) return false;
		}

		return true;
	}

	template <typename T>
	bool QBasicVar<T>::checkState() const {
		return state == STATE::DEF;
	}

	template <typename T>
	T QBasicVar<T>::fetchValue() const {
		// return the value only if the state of the variable is right
		if (checkVar() && checkState()) return data;

		// throw an error
		throw exceptions::unassigned_variable();
	}

	template <typename T>
	QString QBasicVar<T>::fetchName() const {
		return name;
	}
}

// in this program, all variables are set as the type "int"
using QBasicVar = variables::QBasicVar<int>;

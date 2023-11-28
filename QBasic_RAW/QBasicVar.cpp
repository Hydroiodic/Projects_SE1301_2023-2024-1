// this expression ensures no repeat compilation
#pragma once

#include "QBasicVar.h"

template <typename T>
void QBasicVariable<T>::assign(T value) {
	data = value;
	state = STATE::DEF;
}

template <typename T>
bool QBasicVariable<T>::checkVar() const {
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
bool QBasicVariable<T>::checkState() const {
	return state == STATE::DEF;
}

template <typename T>
T QBasicVariable<T>::fetchValue() {
	// if the variable is unassigned, throw an error
	if (!checkState()) throw exceptions::unassigned_variable();

	// if the name of the variable is illegal, throw an error
	// actually this exception won't be thrown if the variable is created normally
	if (!checkVar()) throw exceptions::illegal_variable_name();

	// add use_count every time the data of the variable is fetched
	++use_counts;

	// return the value only if the variable is ready
	return data;
}

template <typename T>
int QBasicVariable<T>::fetchCount() const {
	return use_counts;
}

template <typename T>
void QBasicVariable<T>::resetCount() {
	use_counts = 0;
}

template <typename T>
QString QBasicVariable<T>::fetchName() const {
	return name;
}

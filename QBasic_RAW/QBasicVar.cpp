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
T QBasicVariable<T>::fetchValue() const {
	// return the value only if the state of the variable is right
	if (checkVar() && checkState()) return data;

	// throw an error
	throw exceptions::unassigned_variable();
}

template <typename T>
QString QBasicVariable<T>::fetchName() const {
	return name;
}

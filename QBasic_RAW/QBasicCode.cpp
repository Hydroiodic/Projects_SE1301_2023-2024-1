#include "QBasicCode.h"

QBasicCode::QBasicCode(QObject* parent) : QObject(parent) {
	/* there's nothing to do */
}

QBasicCode::~QBasicCode() {
	/* there's nothing to do */
}

void QBasicCode::addCode(const Command& com) {
	code[com.getLineNum()] = com;
}

QString QBasicCode::getCode() const {
	QString str = "";

	// add string to "str" one by one
	for (std::map<int, Command>::const_iterator it = code.begin();
		it != code.end(); ++it) {
		str += it->second.getRawStr();
		str += "\n";
	}

	return str;
}

void QBasicCode::clearCode() {
	code.clear();
}

int QBasicCode::getCodeNo(int line_number) const {
	// find the index of the line_number
	int i = 0;
	for (std::map<int, Command>::const_iterator it = code.begin(); 
		it != code.end(); ++it, ++i) {
		if (it->first == line_number) {
			return i;
		}
	}

	// if not found, return -1
	return -1;
}

size_t QBasicCode::size() const {
	return code.size();
}

Command QBasicCode::operator[](int n) const {
	if (n >= code.size())
		throw exceptions::wrong_access_to_codes();

	// fetch the command
	// TODO: a lot of redundent iterations are executed here
	std::map<int, Command>::const_iterator it = code.begin();
	for (int i = 0; i < n; ++i, ++it);
	return it->second;
}

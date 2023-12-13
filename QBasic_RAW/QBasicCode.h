// QBasicCode.h
// header file for QBasicCode.cpp

#pragma once

#include <QObject>
#include <map>
#include "QBasicCmdParser.h"

using commands::Command;

class QBasicCode : public QObject
{
	Q_OBJECT

public:
	QBasicCode(QObject* parent = nullptr);
	~QBasicCode();

public:
	void addCode(const Command& com);
	void deleteCode(int line_num);
	QString getCode() const;
	void clearCode();

	int getCodeNo(int line_number) const;

	size_t size() const;
	Command operator[](int n) const;

private:
	std::map<int, Command> code;
};

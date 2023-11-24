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
	QString getCode() const;
	void clearCode();

	size_t size() const;
	Command operator[](int n) const;

private:
	std::map<int, Command> code;
};

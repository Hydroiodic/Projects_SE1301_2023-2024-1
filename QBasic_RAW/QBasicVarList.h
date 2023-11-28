#pragma once

#include <QObject>
#include <map>
#include "QBasicVar.h"

class QBasicVarList : public QObject
{
	Q_OBJECT

private:
	std::map<QString, QBasicVar*> variables_list;

public:
	QBasicVarList(QObject* parent = nullptr);
	~QBasicVarList();
	
	// clear the list of variables
	void clearVarList();

	// clear used counts of all variables
	void clearVarCount() const;

	QBasicVar* fetchVar(const QString& name);
};

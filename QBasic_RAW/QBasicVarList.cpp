// QBasicVarList.cpp
// providing methods to operate on the list of stored variables

#include "QBasicVarList.h"

QBasicVarList::QBasicVarList(QObject* parent) : QObject(parent) {
	/* there's nothing more to do */
}

QBasicVarList::~QBasicVarList() {
	clearVarList();
}

void QBasicVarList::clearVarList() {
	// delete every variable
	for (std::map<QString, QBasicVar*>::iterator it = variables_list.begin();
		it != variables_list.end(); ++it) {
		delete it->second;
	}

	// clear the list
	variables_list.clear();
}

void QBasicVarList::clearVarCount() const {
	// clear used counts of every variable stored
	for (std::map<QString, QBasicVar*>::const_iterator it = variables_list.cbegin();
		it != variables_list.cend(); ++it) {
		it->second->resetCount();
	}
}

QBasicVar* QBasicVarList::fetchVar(const QString& name) {
	// fetch the variable according to "name"
	std::map<QString, QBasicVar*>::iterator result = variables_list.find(name);

	// no result found
	if (result == variables_list.end()) {
		QBasicVar* new_var = new QBasicVar(nullptr, name);
		variables_list.insert(std::make_pair(name, new_var));
		return new_var;
	}
	// the result is found correctly
	else {
		return result->second;
	}
}

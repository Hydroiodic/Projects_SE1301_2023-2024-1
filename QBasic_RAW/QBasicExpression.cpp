#include "QBasicExpression.h"

/************* Below are QBasicExpression functions *************/

QBasicExpression::QBasicExpression(QObject* parent) : QObject(parent) {
	/* there's nothing to do */
}

QBasicExpression::~QBasicExpression() {
	if (exp) delete exp;
}

void QBasicExpression::loadExp(commands::IMPL t, const QString& str, int line) {
	try {
		exp = new Expression(t, str);
		line_number = line;
	}
	catch (exceptions::expression_error) {
		// ERROR will show "ERR" in the expression tree and will not be executed
		state = ERROR;
	}
	catch (...) {
		throw exceptions::unknown_error_internal();
	}

	// ACTIVE means the expression is ready for execution
	state = ACTIVE;
}

void QBasicExpression::executeExp() {
	// repeat execution is not allowed
	if (state != ACTIVE) {
		// TODO: maybe an error thrown?
		return;
	}

	// TODO: execute the command

	state = INACTIVE;
}

void QBasicExpression::clearExp() {
	delete exp;
	exp = nullptr;

	state = ERROR;
}

QString QBasicExpression::getExpTree() const {
	if (state == STATE_EXP::ERROR) {
		return QString::number(line_number) + " Error\n";
	}

	return QString::number(line_number) + " " + exp->getExpTree();
}

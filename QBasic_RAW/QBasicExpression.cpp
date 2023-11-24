#include "QBasicExpression.h"

/************* Below are QBasicExpression functions *************/

QBasicExpression::QBasicExpression(QObject* parent) : QObject(parent) {
	/* there's nothing to do */
}

QBasicExpression::~QBasicExpression() {
	if (exp) delete exp;
}

void QBasicExpression::loadExp(commands::IMPL t, const QString& str, int line) {
	// set the number of the line
	line_number = line;

	try {
		// create diffrent object for different commands
		switch (t) {
		case commands::IMPL::REM:
			exp = new ram_expression(str); break;
		case commands::IMPL::LET:
			exp = new let_expression(str); break;
		case commands::IMPL::PRINT:
			exp = new print_expression(str); break;
		case commands::IMPL::INPUT:
			exp = new input_expression(str); break;
		case commands::IMPL::GOTO:
			exp = new goto_expression(str); break;
		case commands::IMPL::IF:
			exp = new if_expression(str); break;
		case commands::IMPL::END:
			exp = new end_expression(str); break;
		default:
			throw exceptions::impossible_arrival();
		}
	}
	catch (exceptions::expression_error) {
		// ERROR will show "Error" in the expression tree and will not be executed
		state = ERROR;
		return;
	}
	catch (exceptions::illegal_variable_name) {
		state = ERROR;
		return;
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

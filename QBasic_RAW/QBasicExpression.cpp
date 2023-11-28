#include "QBasic.h"
#include "QBasicExpression.h"

/************* Below are QBasicExpression functions *************/

QBasicExpression::QBasicExpression(QObject* parent, QBasic* b) 
	: QObject(parent), basic(b) {
	/* there's nothing to do */
}

QBasicExpression::~QBasicExpression() {
	clearExp();
}

void QBasicExpression::addExp(commands::IMPL t, 
	const QString& str, int line, commands::STATE s) {
	exp_pack pack_to_add;

	// set the number of the line
	pack_to_add.line_number = line;
	pack_to_add.type = t;

	// ACTIVE means the expression is ready for execution
	// if there's any error afterwards, change it to ERROR before push
	pack_to_add.state = ACTIVE;

	// if the command is broken, return directly
	if (s == commands::STATE::BROKEN) {
		pack_to_add.state = ERROR;
		exp_list.push_back(pack_to_add);
		return;
	}

	try {
		// create diffrent object for different commands
		switch (t) {
		case commands::IMPL::REM:
			pack_to_add.exp = new ram_expression(str, basic); break;
		case commands::IMPL::LET:
			pack_to_add.exp = new let_expression(str, basic); break;
		case commands::IMPL::PRINT:
			pack_to_add.exp = new print_expression(str, basic);
			connect(pack_to_add.exp, &Expression::appendOutputText, 
				basic, &QBasic::append_output_text);
			break;
		case commands::IMPL::INPUT:
			pack_to_add.exp = new input_expression(str, basic);
			connect(pack_to_add.exp, &Expression::inputVar,
				basic, &QBasic::setInputState);
			break;
		case commands::IMPL::GOTO:
			pack_to_add.exp = new goto_expression(str, basic); break;
		case commands::IMPL::IF:
			pack_to_add.exp = new if_expression(str, basic); break;
		case commands::IMPL::END:
			pack_to_add.exp = new end_expression(str, basic); break;
		default:
			throw exceptions::impossible_arrival();
		}
	}
	catch (exceptions::expression_error) {
		// ERROR will show "Error" in the expression tree and will not be executed
		pack_to_add.state = ERROR;
	}
	catch (exceptions::illegal_variable_name) {
		pack_to_add.state = ERROR;
	}
	catch (...) {
		throw exceptions::unknown_error_internal();
	}

	// add the expression package to the vector
	exp_list.push_back(pack_to_add);
}

int QBasicExpression::executeExp(int index) {
	// ensure safe access to the vector
	if (index >= exp_list.size()) {
		throw exceptions::unknown_error_internal();
	}

	// if the expression is an error expression, no operations needed
	if (exp_list[index].state != ACTIVE) {
		return -1;
	}

	// execute the command
	try {
		return exp_list[index].exp->executeExpression();
	}
	catch (exceptions::unassigned_variable) {
		basic->inform("An unassigned variable is used");
		return -1;
	}
	catch (...) {
		throw exceptions::unknown_error_internal();
	}
}

commands::IMPL QBasicExpression::getExpType(int index) const {
	// ensure safe access to the vector
	if (index >= exp_list.size()) {
		throw exceptions::unknown_error_internal();
	}

	return exp_list[index].type;
}

void QBasicExpression::clearExp() {
	for (int i = 0; i < exp_list.size(); ++i) {
		exp_list[i].exp->disconnect();
		delete exp_list[i].exp;
	}
	exp_list.clear();
}

QString QBasicExpression::getExpTree(int index) const {
	if (exp_list[index].state == STATE_EXP::ERROR) {
		return QString::number(exp_list[index].line_number) + " Error\n";
	}

	return QString::number(exp_list[index].line_number) + 
		" " + exp_list[index].exp->getExpTree();
}

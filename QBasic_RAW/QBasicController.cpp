#include "QBasicController.h"

QBasicController::QBasicController(QObject* parent, QBasic* basic) 
	: QObject(parent), basic(basic) {
	// create a imaginery stack to count for the amonut of commands executed
	stack = new QBasicStack();
}

QBasicController::~QBasicController() {
	delete stack;
}

void QBasicController::run() const {
	// clear the previous texts and the height of the stack
	basic->ui->treeDisplay->clear();
	basic->ui->textBrowser->clear();
	stack->resetStack();

	// load all of the codes to QBasicExpression
	loadExpressions();

	// iterate all codes saved previously
	int len = basic->code.size(), i = 0;
	while (true) {
		// if the program runs out of the code
		if (i >= len) {
			basic->inform("No \"END\" encountered!");
			break;
		}

		try {
			// get the number of the next line
			int next_line = basic->expression->executeExp(i);

			// the expression is END
			if (next_line == 0x7fffffff) {
				break;
			}

			// find the index of "next_line" and assign it to i
			if (next_line <= commands::max_line_num) {
				int next_i = basic->code.getCodeNo(next_line);
				if (next_i != -1) {
					i = next_i - 1;
				}
			}

			// increase the number of the counter
			stack->increaseStack();
			++i;
		}
		catch (exceptions::stack_overflow) {
			// stack overflow occurs
			basic->inform("Stack overflow!");
			break;
		}
		catch (...) {
			// ATTENTION! Internal Error!
			throw exceptions::unknown_error_internal();
		}
	}

	// append the expression to the tree one by one
	for (int i = 0; i < len; ++i) {
		basic->ui->treeDisplay->append(basic->expression->getExpTree(i));
	}

	// clear all of the expressions loaded into QBasicExpression
	basic->expression->clearExp();
}

void QBasicController::load() const {
	// TODO
}

void QBasicController::list() const {
	/* nothing to do in this program */
}

void QBasicController::clear() const {
	basic->code.clearCode();
	basic->expression->clearExp();
	basic->variables_list->clearVarList();

	// clear the displayed text of the window
	basic->ui->CodeDisplay->clear();
	basic->ui->treeDisplay->clear();
	basic->ui->textBrowser->clear();
}

void QBasicController::help() const {
	// TODO
}

void QBasicController::quit() const {
	exit(0);
}

void QBasicController::execute(int no) const {
	(this->*func[no])();
}

void QBasicController::loadExpressions() const {
	// clear the QBasicExpression first
	basic->expression->clearExp();

	int len = basic->code.size();
	for (int i = 0; i < len; ++i) {
		try {
			Command cur_cmd = basic->code[i];

			// load expression, there may be expression_error thrown
			basic->expression->addExp(cur_cmd.getImplType(), cur_cmd.getExp(), 
				cur_cmd.getLineNum(), cur_cmd.getState());
		}
		catch (...) {
			// ATTENTION! Internal Error!
			throw exceptions::unknown_error_internal();
		}
	}
}

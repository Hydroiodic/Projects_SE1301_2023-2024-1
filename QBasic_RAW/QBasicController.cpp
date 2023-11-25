#include "QBasicController.h"

QBasicController::QBasicController(QObject* parent, QBasic* basic) 
	: QObject(parent), basic(basic) {
	/* there's nothing more to do */
}

QBasicController::~QBasicController() {
	/* there's nothing more to do */
}

void QBasicController::run() const {
	// clear the previous texts
	basic->ui->treeDisplay->clear();
	basic->ui->textBrowser->clear();

	// iterate all codes saved previously
	int len = basic->code.size();
	for (int i = 0; i < len; ++i) {
		try {
			Command cur_cmd = basic->code[i];

			// the state of the command may be BROKEN
			switch (cur_cmd.getState()) {
			case commands::STATE::UNCHECK:
			case commands::STATE::OK:
				break;

			case commands::STATE::BROKEN:
				// the clause is broken, no more checking needed
				basic->ui->treeDisplay->append(QString::number(
					cur_cmd.getLineNum()) + " Error\n");
				continue;

			default:
				throw exceptions::impossible_arrival();
			}

			// load expression, there may be expression_error thrown
			basic->expression->loadExp(cur_cmd.getImplType(),
				cur_cmd.getExp(), cur_cmd.getLineNum());

			// append the expression to the tree
			basic->ui->treeDisplay->append(basic->expression->getExpTree());

			// get the number of the next line
			int next_line = basic->expression->executeExp();

			// clear the expression and run the next expression
			// TODO: use a vector to store them or make a counter another way
			basic->expression->clearExp();

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
		}
		catch (...) {
			// ATTENTION! Internal Error!
			throw exceptions::unknown_error_internal();
		}
	}
}

void QBasicController::load() const {
	// TODO
}

void QBasicController::list() const {
	// TODO
}

void QBasicController::clear() const {
	basic->code.clearCode();

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

#include "QBasicController.h"

QBasicController::QBasicController(QObject* parent, QBasic* basic) 
	: QObject(parent), basic(basic) {
	/* there's nothing more to do */
}

QBasicController::~QBasicController() {
	/* there's nothing more to do */
}

void QBasicController::run() const {
	basic->ui->treeDisplay->clear();

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
				throw exceptions::expression_error();

			default:
				throw exceptions::impossible_arrival();
			}

			// load expression, there may be expression_error thrown
			basic->expression->loadExp(cur_cmd.getImplType(),
				cur_cmd.getExp(), cur_cmd.getLineNum());
			basic->expression->executeExp();

			// append the expression to the tree
			basic->ui->treeDisplay->append(basic->expression->getExpTree());

			basic->expression->clearExp();
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
	// TODO: clear the status of the program
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

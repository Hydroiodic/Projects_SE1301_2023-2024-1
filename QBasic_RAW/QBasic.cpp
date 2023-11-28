#include "QBasic.h"

QBasic::QBasic(QWidget* parent) : QMainWindow(parent), ui(new Ui::QBasic) {
	ui->setupUi(this);

	controller = new QBasicController(nullptr, this);
	parser = new QBasicCmdParser();
	variables_list = new QBasicVarList();
	expression = new QBasicExpression(nullptr, this);

	// initialize slots for QBasic to use
	initSlots();
}

QBasic::~QBasic() {
	delete ui;
	delete controller;
	delete parser;
	delete variables_list;
	delete expression;
}

/************* Below are QBasic informing functions *************/

void QBasic::inform(const QString& str) const {
	qDebug() << str;
}

/************** Below are QBasic parsing functions **************/

Command QBasic::parseCommand(const QString& str) const {
	try {
		return parser->parseCommand(str);
	}
	catch (exceptions::line_number_out_of_range) {
		inform("The input line number must be positive and less then 1000000!");
	}
	catch (exceptions::unknown_command) {
		inform("The input command is unknown to us.");
	}
	catch (...) {
		// internal error occurred
		assert(0);
	}

	return Command(commands::TYPE::ERR);
}

/************* Below are QBasic execution functions *************/

void QBasic::executeCmd(int no) {
	controller->execute(no);
}

void QBasic::executeInstCmd(const Command& cmd) {
	Expression* exp = nullptr;

	try {
		// deal with different type the instant command holds
		switch (cmd.getInstType()) {
		case commands::INST::Let:
			exp = new let_expression(cmd.getExp(), this);
			break;

		case commands::INST::Print:
			exp = new print_expression(cmd.getExp(), this);
			break;

		case commands::INST::Input:
			exp = new input_expression(cmd.getExp(), this);
			break;

		default:
			throw exceptions::impossible_arrival();
		}
	}
	catch (exceptions::expression_error) {
		inform("The input instant expression is error!");
		return;
	}
	catch (exceptions::illegal_variable_name) {
		inform("The name of the variable is illegal!");
		return;
	}
	catch (...) {
		throw exceptions::unknown_error_internal();
	}

	// the operator new failed
	if (!exp) {
		throw exceptions::unknown_error_internal();
	}

	// execute the instant command and then release the memory
	try {
		exp->executeExpression();
	}
	catch (exceptions::unassigned_variable) {
		inform("An unassigned variable is used");
	}
	catch (...) {
		delete exp;
		throw exceptions::unknown_error_internal();
	}

	delete exp;
}

void QBasic::inputAssignVariable(const QString& str) {
	if (!variable_to_input && !is_input) {
		is_input = false;
		throw exceptions::unknown_error_internal();
	}

	// parse the input string
	QString trimmed_str = str.trimmed();
	if (trimmed_str.length() > 0 && trimmed_str[0] == '?') {
		trimmed_str = trimmed_str.mid(1).trimmed();
	}

	// if the input string is not a number
	if (!expressions::isNumber(trimmed_str)) {
		inform("The input string doesn't represent a number!");
		ui->cmdLineEdit->setText("? ");
		return;
	}

	// the sign bit
	int base = 1;
	if (trimmed_str[0] == '-') {
		base = -1;
		trimmed_str = trimmed_str.mid(1).trimmed();
	}

	// assign the variable
	variable_to_input->assign(base * trimmed_str.toInt());

	// reset the input state
	is_input = false;
	variable_to_input = nullptr;
}

void QBasic::setInputState(QBasicVar* var) {
	// this may never be reached
	if (!var) {
		throw exceptions::unknown_error_internal();
	}

	// be ready for the input
	is_input = true;
	variable_to_input = var;
	ui->cmdLineEdit->setText("? ");
}

/*************** Below are QBasic slots functions ***************/

void QBasic::initSlots() {
	connect(this->ui->btnClearCode, &QPushButton::pressed, 
		this, &QBasic::on_btnClearCode_pressed);
	connect(this->ui->btnLoadCode, &QPushButton::pressed,
		this, &QBasic::on_btnLoadCode_pressed);
	connect(this->ui->btnRunCode, &QPushButton::pressed,
		this, &QBasic::on_btnRunCode_pressed);
}

void QBasic::on_cmdLineEdit_returnPressed() {
	// fetch and clear the input text
	QString cmd_text = ui->cmdLineEdit->text();
	ui->cmdLineEdit->setText("");

	if (is_input) {
		inputAssignVariable(cmd_text);
		return;
	}
	
	// try parse and catch error
	Command cmd = parseCommand(cmd_text);

	switch (cmd.getType()) {
	case commands::COM:
		// execute the command input
		executeCmd(cmd.getCommType());
		break;

	case commands::IMP:
		// the input string is appended to the end of codes
		code.addCode(cmd);
		ui->CodeDisplay->setText(code.getCode());
		break;

	case commands::CLE:
		// delete the code according to the number of the line
		code.deleteCode(cmd.getLineNum());
		ui->CodeDisplay->setText(code.getCode());
		break;

	case commands::INS:
		// execute the instant command directly
		executeInstCmd(cmd);
		break;

	case commands::ERR:
		// there's nothing to do here
		break;

	default:
		// no arrival forever
		throw exceptions::impossible_arrival();
	}
}

void QBasic::on_btnClearCode_pressed() {
	// clear the code and the status of the program
	controller->clear();
}

void QBasic::on_btnRunCode_pressed() {
	// run the input program
	controller->run();
}

void QBasic::on_btnLoadCode_pressed() {
	// load codes from an existing file
	controller->load();
}

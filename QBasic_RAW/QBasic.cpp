#include "QBasic.h"

QBasic::QBasic(QWidget* parent) : QMainWindow(parent), ui(new Ui::QBasic) {
	ui->setupUi(this);

	controller = new QBasicController(nullptr, this);
	parser = new QBasicCmdParser();
	expression = new QBasicExpression();

	// initialize slots for QBasic to use
	initSlots();
}

QBasic::~QBasic() {
	delete ui;
	delete controller;
	delete parser;
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
	
	// try parse and catch error
	Command cmd = parseCommand(cmd_text);

	switch (cmd.getType()) {
	case commands::COM:
		// execute the command input
		executeCmd(cmd.getCommType());
		return;

	case commands::IMP:
		// the input string is appended to the end of codes
		code.addCode(cmd);
		ui->CodeDisplay->setText(code.getCode());
		break;

	case commands::CLE:
		// TODO
		break;

	case commands::INS:
		// TODO
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

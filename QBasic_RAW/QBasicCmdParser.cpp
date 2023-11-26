#include <sstream>
#include <string>
#include "QBasicCmdParser.h"

namespace commands {
	/************** Below are QBasic Command functions **************/

	void Command::type_error() const {
		throw exceptions::command_type_unmatch_internal();
	}

	int Command::getLineNum() const {
		if (type != TYPE::IMP && type != TYPE::CLE) {
			type_error();
		}

		return line_num;
	}

	COMM Command::getCommType() const {
		if (type != TYPE::COM) {
			type_error();
		}

		return comm_type;
	}

	IMPL Command::getImplType() const {
		if (type != TYPE::IMP) {
			type_error();
		}

		return impl_type;
	}

	INST Command::getInstType() const {
		if (type != TYPE::INS) {
			type_error();
		}

		return inst_type;
	}

	QString Command::getRawStr() const {
		if (type != TYPE::IMP) {
			type_error();
		}

		return raw_str;
	}

	QString Command::getExp() const {
		if (type != TYPE::IMP && type != TYPE::INS) {
			type_error();
		}

		return exp;
	}

	STATE Command::getState() const {
		return state;
	}

	/************** Below are QBasicCmdParser functions *************/

	QBasicCmdParser::QBasicCmdParser(QObject* parent) : QObject(parent) {
		/* there's nothing to do */
	}

	QBasicCmdParser::~QBasicCmdParser() {
		/* there's nothing to do */
	}

	RESULT QBasicCmdParser::parseIMPL(const QString& str) const {
		// get the trimmed string
		QString trimmed_str = str.trimmed();

		// using stringstream to acquire the input command
		std::stringstream input(trimmed_str.toStdString());

		// the number of the current line
		long long line_number;
		input >> line_number;

		// the first part must be a number within a specific range
		if (!input) return RESULT(false, Command());
		if (line_number > max_line_num) throw exceptions::line_number_out_of_range();

		// the command to execute
		std::string impl_command = "", exp = "";
		input >> impl_command;

		// if there is no command input, clear the line
		if (!input) return RESULT(true, Command(CLE, line_number));

		// the command must be contained in the vector
		int impl_command_num = getIdxImpl(QString::fromStdString(impl_command));

		// all left expression are input into exp
		std::getline(input, exp);

		// no command found, return a broken command
		if (impl_command_num == -1)
			return RESULT(true, Command(IMP, IMPL(0), STATE::BROKEN, 
				line_number, QString::fromStdString(exp), trimmed_str));

		return RESULT(true, Command(IMP, IMPL(impl_command_num), STATE::UNCHECK, 
			line_number, QString::fromStdString(exp), trimmed_str));
	}

	RESULT QBasicCmdParser::parseCOMM(const QString& str) const {
		// get the trimmed string
		QString trimmed_str = str.trimmed();

		// judge whether the input string is control command
		int idx_comm = getIdxComm(trimmed_str);
		if (idx_comm != -1) {
			return RESULT(true, Command(TYPE::COM, COMM(idx_comm)));
		}

		// continue to parse the command
		return RESULT(false, Command());
	}

	RESULT QBasicCmdParser::parseINST(const QString& str) const {
		// get the trimmed string
		QString trimmed_str = str.trimmed();

		// using stringstream to acquire the input command
		std::stringstream input(trimmed_str.toStdString());

		// the command to execute
		std::string inst_command, exp;
		input >> inst_command;

		// if the stream gets bad, return false
		if (!input) return RESULT(false, Command());

		// the command must be contained in the vector
		int inst_command_num = getIdxInst(QString::fromStdString(inst_command));

		// all left expression are input into exp
		std::getline(input, exp);

		// no instant command found, return false
		if (inst_command_num == -1) return RESULT(false, Command());

		return RESULT(true, Command(INS, INST(inst_command_num), QString::fromStdString(exp)));
	}

	Command QBasicCmdParser::parseCommand(const QString& str) const {
		RESULT result_1 = parseIMPL(str);
		if (result_1.first) return result_1.second;

		RESULT result_2 = parseCOMM(str);
		if (result_2.first) return result_2.second;

		RESULT result_3 = parseINST(str);
		if (result_3.first) return result_3.second;

		throw exceptions::unknown_command();
	}
}

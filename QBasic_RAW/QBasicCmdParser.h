#pragma once

#include <QObject>
#include <QPair>
#include <QString>
#include "commands.h"
#include "exceptions.h"

namespace commands {
	const unsigned long long max_line_num = 1e6;

	// TODO: expression tree
	class Command
	{
	private:
		TYPE type = TYPE(0);
		STATE state = STATE::UNCHECK;
		IMPL impl_type = IMPL(0);
		COMM comm_type = COMM(0);
		INST inst_type = INST(0);

		// used for IMP and CLE
		int line_num = -1;

		// used for IMP
		QString exp = "";

		// save the raw string for printing
		QString raw_str = "";

		// the type is found to be wrong when the program runs
		void type_error() const;

	public:
		// default constructor
		Command() {}
		// ERROR constructor
		Command(TYPE t) : type(t) {
			if (t != TYPE::ERR) type_error();
		}
		// summon COM command
		Command(TYPE t, COMM c) : type(t), comm_type(c), state(STATE::OK) {
			if (t != TYPE::COM) type_error();
		}
		// summon IMP command
		Command(TYPE t, IMPL i, STATE s, int n, const QString& e, const QString& r)
			: type(t), impl_type(i), state(s), line_num(n), exp(e), raw_str(r) {
			if (t != TYPE::IMP) type_error();
		}
		// summon CLE command
		Command(TYPE t, int n) : type(t), line_num(n), state(STATE::OK) {
			if (t != TYPE::CLE) type_error();
		}
		// summon INS command
		Command(TYPE t, INST i, const QString& str)
			: type(t), inst_type(i), exp(str) {
			if (t != TYPE::INS) type_error();
		}

		// get the line number of the command
		int getLineNum() const;
		// get the type of this object
		TYPE getType() const { return type; };
		// get the type of the control command
		COMM getCommType() const;
		// get the type of the implement command
		IMPL getImplType() const;
		// get the type of the instant command
		INST getInstType() const;
		// get the raw string of the command
		QString getRawStr() const;
		// get the string to parse
		QString getExp() const;
		// get the state of this command
		STATE getState() const;
	};

	// definition of parsing result
	using RESULT = QPair<bool, Command>;

	class QBasicCmdParser : public QObject
	{
		Q_OBJECT

	public:
		QBasicCmdParser(QObject* parent = nullptr);
		~QBasicCmdParser();

	private:
		RESULT parseCOMM(const QString& str) const;
		RESULT parseIMPL(const QString& str) const;
		RESULT parseINST(const QString& str) const;

	public:
		Command parseCommand(const QString& str) const;
	};
}

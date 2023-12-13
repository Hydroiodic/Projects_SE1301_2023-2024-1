// commands.h
// namespace commands defined here, saving those commands to type in

#pragma once

#include <vector>
#include <QString>

namespace commands {
	const std::vector<QString> impl = {
		"REM",
		"LET",
		"PRINT",
		"INPUT",
		"GOTO",
		"IF",
		"END"
	};

	const std::vector<QString> comm = {
		"RUN",
		"LOAD",
		"LIST",
		"CLEAR",
		"HELP",
		"QUIT"
	};

	const std::vector<QString> inst = {
		"LET",
		"PRINT",
		"INPUT"
	};

	enum TYPE {
		IMP, COM, CLE, INS, ERR
	};

	enum STATE {
		UNCHECK, BROKEN, OK
	};

	enum IMPL {
		REM, LET, PRINT, INPUT, GOTO, IF, END
	};

	enum COMM {
		RUN, LOAD, LIST, CLEAR, HELP, QUIT
	};

	enum INST {
		Let, Print, Input
	};

	int getIdx(const std::vector<QString>& vec, const QString& str);
	int getIdxImpl(const QString& str);
	int getIdxComm(const QString& str);
	int getIdxInst(const QString& str);
}

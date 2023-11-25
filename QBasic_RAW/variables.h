#pragma once

#include <QObject>
#include <QString>
#include <vector>
#include "exceptions.h"

namespace variables {
	const std::vector<QString> key_words = {
		"REM",
		"LET",
		"PRINT",
		"INPUT",
		"GOTO",
		"IF",
		"THEN",
		"END",
		"RUN",
		"LOAD",
		"LIST",
		"CLEAR",
		"HELP",
		"QUIT"
	};

	enum STATE {
		UNDEF, DEF
	};
}

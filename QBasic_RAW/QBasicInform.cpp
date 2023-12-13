// QBasicInform.cpp
// this class provides a simple way to post error info to the main class

#include "QBasicInform.h"

void QBasicInform::sendInform(const QString& str) const {
	emit inform(str);
}

// here the "extern" variable is defined!
QBasicInform informer;

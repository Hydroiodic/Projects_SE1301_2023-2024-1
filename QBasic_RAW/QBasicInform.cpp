#include "QBasicInform.h"

void QBasicInform::sendInform(const QString& str) const {
	emit inform(str);
}

QBasicInform informer;

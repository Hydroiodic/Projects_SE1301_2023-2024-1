#pragma once

#include <QObject>

class QBasicInform : public QObject
{
	Q_OBJECT

public:
	void sendInform(const QString& str) const;

signals:
	void inform(const QString&) const;

};

extern QBasicInform informer;

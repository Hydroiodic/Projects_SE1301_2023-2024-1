// QLinkTest.h
// header file for QLinkTest.cpp

#pragma once

#include <QObject>
#include <QtTest/QtTest>
#include "QLink.h"

class QLinkTest : public QObject
{
	Q_OBJECT

public:
	QLinkTest();
	~QLinkTest();

private:
	QLink* _main_window = nullptr;

	using DATA = QPair<int, int>;
	using CASE = QPair<DATA, DATA>;

	const int _testcase_count = 10;
	CASE* testcase = nullptr;
	int* result = nullptr;

private slots:
	void test_data();
	void test();
};

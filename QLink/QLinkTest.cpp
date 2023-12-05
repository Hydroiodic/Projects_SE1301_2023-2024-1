// QLinkTest.cpp
// a test framework for QLink, to verifying the correctness of the linking function

#include "QLinkTest.h"

QLinkTest::QLinkTest() {
	_main_window = new QLink;
	_main_window->hide();
	_main_window->prepareGame();
	_main_window->initializeTest();
}

QLinkTest::~QLinkTest() {
	delete _main_window;
	delete testcase;
	delete result;
}

/**********************  Below are QLinkTest functions  **********************/

// QLinkTest::test_data, to prepare testcase for the test
void QLinkTest::test_data() {
	// testcases
	testcase = new CASE[_testcase_count]{
		CASE(DATA(1, 1), DATA(1, 4)),
		CASE(DATA(2, 1), DATA(2, 4)),
		CASE(DATA(6, 1), DATA(4, 6)),
		CASE(DATA(2, 1), DATA(5, 1)),
		CASE(DATA(3, 1), DATA(2, 2)),
		CASE(DATA(4, 5), DATA(4, 6)),
		CASE(DATA(1, 1), DATA(1, 3)),
		CASE(DATA(2, 3), DATA(4, 1)),
		CASE(DATA(3, 2), DATA(5, 5)),
		CASE(DATA(5, 2), DATA(5, 4))
	};

	// results
	result = new int[_testcase_count] {
		0,	// chosen buttons are of different types
			1,	// there's no path between chosen buttons
			2,	// the turning count of the path between chosen buttons exceeds limit
			3,	// the turning count of the path between chosen buttons is 2
			4,	// the turning count of the path between chosen buttons is 1
			5,	// the turning count of the path between chosen buttons is 0
			3,
			4,
			2,
			1
		};

	// prepare testcases
	QTest::addColumn<CASE>("TESTCASE");
	QTest::addColumn<int>("RESULT");
	for (int i = 0; i < _testcase_count; ++i) {
		char _data_name[10]{};
		std::sprintf(_data_name, "result %d", result[i]);
		QTest::newRow(_data_name) << testcase[i] << result[i];
	}
}

// QLinkTest::test, to execute the testing process
void QLinkTest::test() {
	QFETCH(CASE, TESTCASE);
	QFETCH(int, RESULT);

	QCOMPARE(_main_window->linkTwoButtonsTest(TESTCASE.first, TESTCASE.second), RESULT);
}

// QLinkIcon.h
// provides icons for QLinkButton to use

#pragma once

#include <QIcon>
#include <QVector>

// class QLinkIcon: provide a method to use the icons for each button
class QLinkIcon
{
private:
	QIcon* data;
	QVector<int> map;
	int _max_icon_num = 11;
	static const int _total_icon_num = 11;

	friend class QLink;

public:
	QLinkIcon() {
		data = new QIcon[_total_icon_num];

		// 0 represents pits
		data[0].addFile(":/icons/fruit/pit.png");
		data[1].addFile(":/icons/fruit/apple.png");
		data[2].addFile(":/icons/fruit/pineapple.png");
		data[3].addFile(":/icons/fruit/grape.png");
		data[4].addFile(":/icons/fruit/strawberry.png");
		data[5].addFile(":/icons/fruit/watermelon.png");
		data[6].addFile(":/icons/fruit/orange.png");
		data[7].addFile(":/icons/fruit/kiwi.png");
		data[8].addFile(":/icons/fruit/plum.png");
		data[9].addFile(":/icons/fruit/peach.png");
		data[10].addFile(":/icons/fruit/banana.png");
	}

	~QLinkIcon() { delete[] data; }

	// some basic operations
	QIcon& operator[](int pos) { return data[map[pos]]; }
	int getTotalNum() const { return _max_icon_num; }

	// methods in regard to the number and order of the selected icons
	void resize(int _used_kinds_num) {
		_max_icon_num = _used_kinds_num + 1;
		randomOrder();
	}
	void randomOrder() {
		map.clear();
		map.push_back(0);
		while (map.size() < _max_icon_num) {
			int temp = rand() % _total_icon_num;
			if (map.contains(temp)) continue;
			map.push_back(temp);
		}
	}
	void randomOrder(const int* _icon_, int size) {
		_max_icon_num = size;

		map.clear();
		for (int i = 0; i < size; ++i) {
			map.push_back(_icon_[i]);
		}
	}
};

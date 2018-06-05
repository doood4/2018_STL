#pragma once
#include <iostream>
#include <vector>

using namespace std;

class Note {
	int x, y;
	int speed = 10;
public:
	Note(int a, int b) : x(a), y(b) { }

	int getx() const { return x; }
	int gety() const { return y; }
	void down_note() { y += speed; }
};


void Init_Notes(vector<Note>& v) {
	v.reserve(100);

	v.emplace_back(Note(30, 100));
	v.emplace_back(Note(30, 0));
	v.emplace_back(Note(30, -100));
	v.emplace_back(Note(230, -200));
	v.emplace_back(Note(130, -300));
	v.emplace_back(Note(30, -400));
	v.emplace_back(Note(130, -600));
	v.emplace_back(Note(230, -800));
	v.emplace_back(Note(230, -1000));
	v.emplace_back(Note(230, -1200));
	v.emplace_back(Note(230, -1300));
	v.emplace_back(Note(130, -1400));

	v.emplace_back(Note(30, -1600));
	v.emplace_back(Note(230, -1700));
	v.emplace_back(Note(30, -1800));
	v.emplace_back(Note(230, -1900));
	v.emplace_back(Note(30, -2000));
	v.emplace_back(Note(230, -2100));

	v.emplace_back(Note(230, -2300));
	v.emplace_back(Note(130, -2400));
	v.emplace_back(Note(30, -2500));
	v.emplace_back(Note(130, -2600));
	v.emplace_back(Note(230, -2700));
	v.emplace_back(Note(130, -2800));

	v.emplace_back(Note(30, -2900));
	v.emplace_back(Note(230, -2900));
	v.emplace_back(Note(30, -3000));
	v.emplace_back(Note(230, -3000));
	v.emplace_back(Note(30, -3100));
	v.emplace_back(Note(230, -3100));
	v.emplace_back(Note(30, -3200));
	v.emplace_back(Note(230, -3200));
	;
	v.emplace_back(Note(130, -3400));


	v.emplace_back(Note(-3000, -4000));
}
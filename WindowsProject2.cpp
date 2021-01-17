#include "framework.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

void qsort(int* a, int l, int r) {
	int m = a[(l + r) / 2];
	int i = l;
	int j = r;
	do {
		while (a[i] < m)
			i++;
		while (a[j] > m)
			j--;
		if (i <= j) {
			int t = a[i];
			a[i] = a[j];
			a[j] = t;
			i++;
			j--;
		}
	} while (i <= j);
	if (i < r)
		qsort(a, i, r);
	if (j > l)
		qsort(a, l, j);
}

void writeToFile(int* a, int n, std::ofstream* ofs) {
	for (int i = 0; i < n; i++)
		*ofs << a[i] << " ";
	delete(a);
}

bool preliminarilySort(std::ifstream* ifsa) {
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	long long n = statex.ullAvailPhys / 8;
	std::ofstream* ofs = new std::ofstream("B");
	int c = 0;
	while (!ifsa->eof()) {
		int* a = new int[n];
		int i = 0;
		while (!ifsa->eof() && i < n) {
			int b;
			*ifsa >> b;
			if (!ifsa->eof())
				a[i++] = b;
		}
		qsort(a, 0, i - 1);
		if (i != 0) {
			writeToFile(a, i, ofs);
			c++;
		}
	}
	ofs->close();
	ifsa->close();
	ofs = new std::ofstream("A");
	ifsa = new std::ifstream("B");
	while (!ifsa->eof()) {
		int* buffer = new int[10000];
		int i = 0;
		while (!ifsa->eof() && i < 10000) {
			int b;
			*ifsa >> b;
			if (!ifsa->eof())
				buffer[i++] = b;
		}
		if (i != 0)
			writeToFile(buffer, i, ofs);
	}
	ofs->close();
	ifsa->close();
	ofs = new std::ofstream("B");
	ofs->close();
	if (c == 1)
		return true;
	return false;
}

int toInt(std::string s) {
	int a = 0;
	for (int i = 0; i < s.length(); i++)
		a = a * 10 + (int)s[i] - 48;
	return a;
}

void merge(std::ifstream* ifsb, std::ifstream* ifsc, std::ofstream* ofsa) {
	while (!ifsb->eof() && !ifsc->eof()) {
		std::string s1 = "";
		std::string s2 = "";
		if (!ifsb->eof() && !ifsc->eof()) {
			*ifsb >> s1;
			*ifsc >> s2;
		}
		while (!ifsb->eof() && !ifsc->eof() && (s1 != "|" || s2 != "|")) {
			if (s1 != "|" && s2 != "|") {
				int a = toInt(s1);
				int b = toInt(s2);
				if (a < b) {
					*ofsa << a << " ";
					*ifsb >> s1;
				} else {
					*ofsa << b << " ";
					*ifsc >> s2;
				}
			} else 
				if (s1 == "|") {
					*ofsa << toInt(s2) << " ";
					*ifsc >> s2;
				} else
					if (s2 == "|") {
						*ofsa << toInt(s1) << " ";
						*ifsb >> s1;
					}
		}
		if (!ifsb->eof() && ifsc->eof())
			*ofsa << toInt(s1) << " ";
		if (ifsb->eof() && !ifsc->eof())
			*ofsa << toInt(s2) << " ";
	}
	while (!ifsb->eof()) {
		std::string s = "";
		while (!ifsb->eof() && s != "|") {
			*ifsb >> s;
			if (!ifsb->eof() && s != "|")
				*ofsa << s << " ";
		}
	}
	while (!ifsc->eof()) {
		std::string s = "";
		while (!ifsc->eof() && s != "|") {
			*ifsc >> s;
			if (!ifsc->eof() && s != "|")
				*ofsa << s << " ";
		}
	}
	ifsb->close();
	ifsc->close();
	ofsa->close();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	std::clock_t cstart = std::clock();
	std::ifstream* ifsa = new std::ifstream("A");
	std::ofstream* ofsb = new std::ofstream("B");
	std::ofstream* ofsc = new std::ofstream("C");
	bool isSorted = preliminarilySort(ifsa);
	ifsa = new std::ifstream("A");
	int turn = 0;
	int lb = 0, lc = 0;
	int a, b;
	*ifsa >> a;
	while (!isSorted) {
		*ifsa >> b;
		while (!ifsa->eof() && a <= b) {
			if (turn == 0)
				*ofsb << a << " ";
			else
				*ofsc << a << " ";
			a = b;
			*ifsa >> b;
		}
		if (turn == 0) {
			*ofsb << a << " | ";
			lb++;
		} else {
			*ofsc << a << " | ";
			lc++;
		}
		turn = (turn + 1) % 2;
		if (ifsa->eof()) {
			ifsa->close();
			ofsb->close();
			ofsc->close();
			merge(new std::ifstream("B"), new std::ifstream("C"), new std::ofstream("A"));
			ifsa = new std::ifstream("A");
			ofsb = new std::ofstream("B");
			ofsc = new std::ofstream("C");
			isSorted = (lb <= 1) && (lc <= 1);
			*ifsa >> a;
			lb = 0;
			lc = 0;
		} else
			a = b;
	}
	std::clock_t cend = std::clock();
	long long timeElapsed = (cend - cstart) / CLOCKS_PER_SEC;
	std::ofstream* ifst = new std::ofstream("TimeElapsed", std::ios_base::app);
	long long hours = timeElapsed / 3600;
	long long mins = (timeElapsed - hours * 3600) / 60;
	long secs = (timeElapsed - hours * 3600 - mins * 60);
	*ifst << hours << "h " << mins << "m " << secs << "s\r\n";
	ifst->close();
	return 0;
}
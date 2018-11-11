#pragma once

#define PI 3.1415926535897932384626433832795

double sqrt(double x);

inline double abs(double x) {
	return x < 0 ? -x : x;
}

inline double fabs(double x) {
	return x < 0 ? -x : x;
}

double cos(double x);
double sin(double x);
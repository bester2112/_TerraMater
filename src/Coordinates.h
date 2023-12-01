#ifndef COORDINATES_H
#define COORDINATES_H

struct Coordinates {
	int x;
	int z;
};

inline bool operator< (Coordinates const &a, Coordinates const &b) {
	if (a.x < b.x) {
		return true;
	}
	else if (a.x == b.x) {
		if (a.z < b.z) {
			return true;
		}
	}

	return false;
}

#endif // !COORDINATES_H
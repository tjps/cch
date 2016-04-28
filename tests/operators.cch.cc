#include "tests/operators.cch.h"


	bool OverrideCity::operator==(const OverrideCity& o) const {
		return false;
	}

	OverrideCity& OverrideCity::operator<<=(size_t i) {
		return *this;
	}

	char OverrideCity::operator[](size_t i) const {
		return 'x';
	}

	void OverrideCity::operator()() {
		abort();
	}

#ifndef __tests_operators_cch__
#define __tests_operators_cch__

class OverrideCity {

public:

	bool operator==(const OverrideCity& o) const;

	OverrideCity& operator<<=(size_t i);

	char operator[](size_t i) const;

	void operator()();
};
#endif

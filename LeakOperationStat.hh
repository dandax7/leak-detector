#ifndef LEAK_OPERATION_STAT_HH
#define LEAK_OPERATION_STAT_HH

#include "SlidingAverageWindow.hh"

#include <iostream>

class LeakOperationStat
{
public:
	LeakOperationStat();

	void reset();
	friend std::ostream &operator<<(std::ostream &os, const LeakOperationStat &s);

	// returns number of deltas seen so far (inluding this one)
	uint64_t add_delta(ptrdiff_t delta);

	ptrdiff_t delta_average() const
	{ return deltas_.average(); }

	ptrdiff_t delta_delta_average() const
	{ return ddeltas_.average(); }

private:
	uint64_t requests_;

	enum {
		running_derivative_window = 5
	};

	SlidingAverageWindow<ptrdiff_t, running_derivative_window> deltas_;
	SlidingAverageWindow<ptrdiff_t, running_derivative_window> ddeltas_;
};

#endif

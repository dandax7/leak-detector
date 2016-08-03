#include "LeakOperationStat.hh"

#include <stddef.h>

using namespace std;

LeakOperationStat::LeakOperationStat()
{ reset(); }

void LeakOperationStat::reset()
{
	requests_ = 0;
	deltas_.reset();
	ddeltas_.reset();
}

std::ostream &operator<<(std::ostream &os, const LeakOperationStat &s)
{
	os << "Requests=" << s.requests_;
	os << " AvgDelta=" << s.deltas_.average();
	os << " AvgDeltaOfDelta=" << s.ddeltas_.average();
	return os;
}

uint64_t LeakOperationStat::add_delta(ptrdiff_t delta)
{
	requests_++;

	ptrdiff_t old_delta_avg = deltas_.average();
	deltas_.insert(delta);
	ptrdiff_t new_delta_avg = deltas_.average();
	ddeltas_.insert(new_delta_avg - old_delta_avg);

	return requests_;
}

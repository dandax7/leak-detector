#ifndef LEAK_DETECTOR_HH
#define LEAK_DETECTOR_HH

#include "LeakOperationStat.hh"

#include <map>
#include <sstream>
#include <string>
#include <stdexcept>

enum LeakMeasureResult
{
	DRY,
	TRICKLE,
	LEAK,
	FLOOD
};

class LeakDetectorImpl
{
public:
	LeakDetectorImpl(std::ostream &out);
	void set_options(const char *file, bool ignore_unknow_values = true);
	void set_option(const std::string &arg,
			const std::string &value);

	void print_options(std::ostream &os);

protected:
	bool start_stat(LeakOperationStat& ls);
	bool stop_stat();
	void write_results(const std::string &op_str);


private:
	LeakMeasureResult stop_stat_result();

	bool enabled_;
	uint64_t sample_freq_;
	uint64_t count_;
	uint64_t startup_forgiveness_;
	uint64_t dry_threshold_;
	uint64_t trickle_threshold_;
	uint64_t leak_threshold_;

	std::ostream &out_;
	LeakMeasureResult last_result_;
	LeakOperationStat *current_ls_;
	char *heap_;
};

template <class POLICY>
class LeakDetector : public LeakDetectorImpl
{
public:
	typedef typename POLICY::TYPE TYPE;
	LeakDetector(std::ostream &out) : LeakDetectorImpl(out) {}

	void start(const TYPE &operation)
	{
		if (!policy_.good(operation))
		{
			std::ostringstream os;
			os << "Illegal operation: "<<operation<< " in LeakDetector::start";
			throw std::overflow_error(os.str());
		}

		if (start_stat(policy_[operation]))
			current_op_ = operation;
	}

	void stop(const TYPE& operation)
	{
		if (stop_stat())
			return;

		std::ostringstream os;
		os << operation;
		write_results(os.str());
	}

	void reset_all()
	{
		typename POLICY::ITERATOR i = policy_.begin();
		while (i != policy_.end())
			policy_.valueof(i).reset();
	}

	void print_all()
	{
		typename POLICY::ITERATOR i = policy_.begin();
		while (i != policy_.end())
		{
			out_ << policy_.keyof(i) << ':' << std::endl;
			out_ << policy_.valueof(i) << std::endl;
		}
	}

private:
	POLICY policy_;
	TYPE current_op_;
};

template <int MAX, int MIN = 0>
class LeakDetectorPolicyInt
{
public:
	typedef int TYPE;
	typedef LeakOperationStat* ITERATOR;

	bool good(TYPE op) const
	{ return op >= MIN && op <= MAX; }

	LeakOperationStat& operator[](TYPE i)
	{ return stats_[i - MIN]; }

	ITERATOR begin()
	{ return &stats_[0]; }

	ITERATOR end()
	{ return &stats_[MAX - MIN + 1]; }

	TYPE keyof(ITERATOR i)
	{ return i - begin(); }

	LeakOperationStat& valueof(ITERATOR i)
	{ return *i; }

private:
	LeakOperationStat stats_[MAX - MIN + 1];
};

template <class KEY>
class LeakDetectorPolicyMap
{
public:
	typedef typename std::map<KEY, LeakOperationStat>::iterator ITERATOR;
	typedef KEY TYPE;

	bool good(TYPE op) const
	{ return true; }

	LeakOperationStat& operator[](TYPE i)
	{ return stats_[i]; }

	ITERATOR begin()
	{ return stats_.begin(); }

	ITERATOR end()
	{ return stats_.end(); }

	TYPE keyof(ITERATOR &i)
	{ return (*i).first; }

	LeakOperationStat valueof(ITERATOR &i)
	{ return (*i).second; }

private:
	std::map<KEY, LeakOperationStat> stats_;
};


#endif

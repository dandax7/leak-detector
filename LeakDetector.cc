#include "LeakDetector.hh"
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

LeakDetectorImpl::LeakDetectorImpl(std::ostream &out) :
	out_(out),
	enabled_(false),
	sample_freq_(0),
	count_(0),
	startup_forgiveness_(0),
	dry_threshold_(0),
	trickle_threshold_(0),
	leak_threshold_(0),
	last_result_(DRY),
	current_ls_(NULL),
	heap_(NULL)
{
}

void LeakDetectorImpl::print_options(std::ostream &os)
{
	os << "enabled=" << enabled_ << endl;
	os << "frequency=" << sample_freq_ << endl;
	os << "startup_forgiveness=" << startup_forgiveness_ << endl;
	os << "dry_threshold=" <<  dry_threshold_ << endl;
	os << "trickle_threshold=" << trickle_threshold_ << endl;
	os << "leak_threshold=" << leak_threshold_ << endl;
}

void LeakDetectorImpl::set_options(const char *file, bool ignore_unknow_values)
{
	ifstream conf(file);
	if (conf.bad())
		throw runtime_error("Can't open file LeakDetectorImpl::set_options");

	// format must be "option=value\n" for every line
	string line;
	while (getline(conf, line))
	{
		string::size_type eq = line.find('=');
		if (eq == string::npos)
			throw runtime_error("Bad parse in LeakDetectorImpl::set_options");

		string arg = line.substr(0, eq);
		string value = line.substr(0, eq + 1);
		try {
			set_option(arg, value);
		} catch (...) {
			if (ignore_unknow_values)
				cerr << "Can't understand " << line << endl;
			else
				throw;
		}
	}
}

void LeakDetectorImpl::set_option(const std::string &arg,
		 		  const std::string &value)
{
	if      (arg == "frequency")
		sample_freq_ = atoi(value.c_str());
	else if (arg == "enabled")
		enabled_ =  (value == "yes" || value == "true" || value == "1");
	else if (arg == "startup_forgiveness")
		startup_forgiveness_ = atoi(value.c_str());
	else if (arg == "dry_threshold")
		dry_threshold_ = atoi(value.c_str());
	else if (arg == "trickle_threshold")
		trickle_threshold_ = atoi(value.c_str());
	else if (arg == "leak_threshold")
		leak_threshold_ = atoi(value.c_str());
	else
		throw runtime_error(arg.c_str());
}

bool LeakDetectorImpl::start_stat(LeakOperationStat &ls)
{
	if (!enabled_) return false;

	if (current_ls_ != NULL)
		out_ << "Starting new operation w/o stopping old" << endl;

	count_++;

	if (sample_freq_ && count_ % sample_freq_ != 1)
		return false;

	// remember the heap, and the stat we have to add to
	current_ls_ = &ls;
	heap_ = static_cast<char*>(sbrk(0));
	return true;
}

bool LeakDetectorImpl::stop_stat()
{
	last_result_ = stop_stat_result();
	return last_result_ == DRY;
}

LeakMeasureResult LeakDetectorImpl::stop_stat_result()
{
	if (current_ls_ == NULL)
		return DRY;

	char *heap = static_cast<char*>(sbrk(0));
	ptrdiff_t diff = heap - heap_;
	cout << "Allocated " << diff << " bytes" << endl;

	uint64_t reqs = current_ls_->add_delta(diff);
	
	if (reqs < startup_forgiveness_)
		return DRY;

	ptrdiff_t delta_delta = current_ls_->delta_delta_average();

	// ready for next one
	current_ls_ = NULL;
	heap_ = NULL;

	if (delta_delta < trickle_threshold_)
		if (delta_delta < dry_threshold_)
			return DRY;
		else
			return TRICKLE;
	else
		if (delta_delta < leak_threshold_)
			return LEAK;
		else
			return FLOOD;
}

void LeakDetectorImpl::write_results(const std::string &op_str)
{
	out_ << "The operation " << op_str << " leaked " << last_result_ << endl;
}

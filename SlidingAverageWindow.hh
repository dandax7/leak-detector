#ifndef SLIDING_AVERAGE_WINDOW_HH
#define SLIDING_AVERAGE_WINDOW_HH

template <class T, int SIZE>
class SlidingAverageWindow
{
public:
	SlidingAverageWindow()
	{
		reset();
	}

	void reset()
	{
		i_=0;
		sum_ = T();
		for (int i = 0; i < SIZE; i++)
			history_[i] = T();
	}

	T average() const
	{ return sum_ / SIZE; }

	double average_fp() const
	{ return static_cast<double>(sum_) / SIZE; }

	void insert(T n)
	{
		sum_ -= history_[i_];
		history_[i_] = n;
		sum_ += n;
		i_ = (i_ + 1) % SIZE;
	}

private:
	int i_;
	T history_[SIZE];
	T sum_;
};

#endif

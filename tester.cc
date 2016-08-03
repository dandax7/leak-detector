#include "LeakDetector.hh"

#include <iostream>

using namespace std;

main()
{
	try {
		LeakDetector<LeakDetectorPolicyInt<20> > leak_detector(cout);

		leak_detector.set_option("enabled", "1");

		for (int i = 0; i <= 20; i++)
		{
			leak_detector.start(i);

			void *t = malloc(i * 1000);

			cout << "Operation " << i << endl;

			leak_detector.stop(i);
		}
	}
	catch (std::exception &e)
	{
		cerr << "Caught " << e.what() << endl;
	}
}

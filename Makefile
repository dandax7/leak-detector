SDK=/Developer//SDKs/MacOSX10.5.sdk/
CC=/Developer/usr/bin/g++ -g
AR=/Developer/usr/bin/ar
INCLUDE=-I$(SDK)/usr/include -I$(SDK)/usr/include/c++/4.0.0/ -I$(SDK)/usr/include/c++/4.0.0/i686-apple-darwin9
LINKPATH=-L$(SDK)/usr/lib


tester: libleak.a tester.cc
	$(CC) $(INCLUDE) $(LINKPATH) tester.cc libleak.a -o tester

libleak.a: LeakDetector.o LeakOperationStat.o
	test -f libleak.a && rm libleak.a
	$(AR) -r libleak.a LeakDetector.o LeakOperationStat.o

LeakDetector.o: LeakDetector.cc
	$(CC) $(INCLUDE) -c LeakDetector.cc

LeakOperationStat.o: LeakOperationStat.cc
	$(CC) $(INCLUDE) -c LeakOperationStat.cc

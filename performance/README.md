# Performace

This is maybe not perfect but can show something. All result are run using RELESE build.  
Why?

Sample DEBUG run:  
```commandline
Run on (8 X 3600 MHz CPU s)
2017-08-05 12:44:53
***WARNING*** Library was built as DEBUG. Timings may be affected.
---------------------------------------------------------------
Benchmark                        Time           CPU Iterations
---------------------------------------------------------------
checkSimpleNotification        293 ns        293 ns    2319171
```

Sample RELEASE run:  
```commandline
Run on (8 X 3600 MHz CPU s)
2017-08-05 12:45:43
---------------------------------------------------------------
Benchmark                        Time           CPU Iterations
---------------------------------------------------------------
checkSimpleNotification          6 ns          6 ns  116492914
```

So below all numbers are in release.

This library as you can read in main README.md was inspured by [CCNotificationCenter](https://github.com/cocos2d/cocos2d-x/blob/v2/cocos2dx/support/CCNotificationCenter.h) from cocos2d-x game engine.  
So i want to present comparision of performance of this two. Of course this is only showcase.


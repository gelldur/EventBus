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
I don't want to add submodule of cocos2d-x so simply i run it only and present results. Cocos2d-x was also build as release. If you want to repeat it here are steps i follow:
```commandline
cd performance # From root of this project
git clone -b v2 https://github.com/cocos2d/cocos2d-x.git #this can take some time :/ it need to download ~900 MB
# Uncomment line in CMakeLists.txt which INCLUDE(Cocos2dxCompare.cmake)
# Apply patch CCNotificationCenterPerformance.patch
cd cocos2d-x
git apply ../CCNotificationCenterPerformance.patch
``` 

```
Run on (8 X 3600 MHz CPU s)
2017-08-05 19:30:38
-----------------------------------------------------------------------------------------------------------------
Benchmark                                                          Time           CPU Iterations UserCounters...
-----------------------------------------------------------------------------------------------------------------
checkSimpleNotification                                            6 ns          6 ns  109393759 sum=417.304M
check10Listeners                                                  22 ns         22 ns   32170743 sum=1.19845G
check100Listeners                                                213 ns        213 ns    3299613 sum=1.2292G
check1kListeners                                                2094 ns       2093 ns     333683 sum=1.24307G
-----------------------------------------------------------------------------------------------------------------
call1kLambdas_compare                                           2181 ns       2181 ns     308722 sum=1.15008G
-----------------------------------------------------------------------------------------------------------------
check10NotificationsFor1kListeners                               267 ns        267 ns    2607581 sum=-1.81421G
check100NotificationsFor1kListeners                              107 ns        107 ns    6373043 sum=1.8782G
check1kNotificationsFor1kListeners                               129 ns        129 ns    5497132 sum=1.11759G
check100NotificationsFor10kListeners                             314 ns        314 ns    2226597 sum=543.525M
-----------------------------------------------------------------------------------------------------------------
CCNotificationCenter_checkSimpleNotification                     168 ns        168 ns    4259511 sum=16.2487M
CCNotificationCenter_check10Listeners                            275 ns        275 ns    2484477 sum=94.7753M
CCNotificationCenter_check100Listeners                          1399 ns       1398 ns     514899 sum=196.418M
CCNotificationCenter_check1kListeners                          13124 ns      13123 ns      53271 sum=203.213M
CCNotificationCenter_check10NotificationsFor1kListeners        11379 ns      11378 ns      62138 sum=53.3993M
CCNotificationCenter_check100NotificationsFor1kListeners        9583 ns       9582 ns      64665 sum=61.3059M
CCNotificationCenter_check1kNotificationsFor1kListeners         9451 ns       9451 ns      64162 sum=61.5188M
CCNotificationCenter_check100NotificationsFor10kListeners     107082 ns     107074 ns       6603 sum=62.1745M
```

So comparing to CCNotificationCenter, EventBus is something like ~10x FASTER specially when we have more unique notifications.

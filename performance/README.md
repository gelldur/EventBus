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

This library as you can read in main README.md was inspired by [CCNotificationCenter](https://github.com/cocos2d/cocos2d-x/blob/v2/cocos2dx/support/CCNotificationCenter.h) from cocos2d-x game engine.  
So I want to present comparision of performance of this two. Of course this is only showcase.  
I don't want to add submodule of cocos2d-x so simply I run it only and present results. Cocos2d-x was also build as release. If you want to repeat it, here are steps I followed:
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
2017-08-06 17:03:13
------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                Time           CPU Iterations UserCounters     Faster
------------------------------------------------------------------------------------------------------------------------------------------
call1kLambdas_compare                                                 2256 ns       2256 ns     327462 sum=1.21989G     Compare with check1kListeners overhead is super small
------------------------------------------------------------------------------------------------------------------------------------------
checkSimpleNotification                                                 16 ns         16 ns   42451425 sum=161.939M     x 10.7
checkSimpleNotification_CCNotificationCenter                           172 ns        172 ns    4193935 sum=15.9986M
------------------------------------------------------------------------------------------------------------------------------------------
check10Listeners                                                        34 ns         34 ns   20564387 sum=784.469M     x 8.9
check10Listeners_CCNotificationCenter                                  305 ns        305 ns    2208720 sum=84.256M
------------------------------------------------------------------------------------------------------------------------------------------
check100Listeners                                                      208 ns        208 ns    3362931 sum=1.25279G     x 8.4
check100Listeners_CCNotificationCenter                                1758 ns       1758 ns     398100 sum=151.863M
------------------------------------------------------------------------------------------------------------------------------------------
check1kListeners                                                      2074 ns       2074 ns     340081 sum=1.2669G      x 8.1
check1kListeners_CCNotificationCenter                                17001 ns      16999 ns      41548 sum=158.493M
------------------------------------------------------------------------------------------------------------------------------------------
check10NotificationsFor1kListeners                                     263 ns        263 ns    2668786 sum=-1.76281G    x 53.1
check10NotificationsFor1kListeners_CCNotificationCenter              13987 ns      13986 ns      51560 sum=44.2743M
------------------------------------------------------------------------------------------------------------------------------------------
check100NotificationsFor1kListeners                                   UNKNOWN result available with EventBus 2.0.1
check100NotificationsFor1kListeners_CCNotificationCenter             12128 ns      12127 ns      54017 sum=51.181M
------------------------------------------------------------------------------------------------------------------------------------------
check1kNotificationsFor1kListeners                                    UNKNOWN result available with EventBus 2.0.1
check1kNotificationsFor1kListeners_CCNotificationCenter              11940 ns      11939 ns      57722 sum=55.2338M
------------------------------------------------------------------------------------------------------------------------------------------
check100NotificationsFor10kListeners                                  UNKNOWN result available with EventBus 2.0.1
check100NotificationsFor10kListeners_CCNotificationCenter           128244 ns     128233 ns       5297 sum=49.5221M
------------------------------------------------------------------------------------------------------------------------------------------
checkNotifyFor10kListenersWhenNoOneListens                              18 ns         18 ns   38976599 sum=0            x 7077 ;)
checkNotifyFor10kListenersWhenNoOneListens_CCNotificationCenter     127388 ns     127378 ns       5460 sum=0
```
So comparing to CCNotificationCenter, EventBus is something like ~10x FASTER especially when we have more unique notifications.

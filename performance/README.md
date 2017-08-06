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
I don't want to add submodule of cocos2d-x so simply I run it only and present results. Cocos2d-x was also build as release. If you want to repeat it here are steps I followed:
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
2017-08-06 00:09:43
-----------------------------------------------------------------------------------------------------------------------
Benchmark                                                                Time           CPU Iterations UserCounters...
-----------------------------------------------------------------------------------------------------------------------
check100Listeners                                                      234 ns        234 ns    3006299 sum=1.11993G
check100Listeners_EventBus2                                            216 ns        216 ns    3238994 sum=1.20662G
check100Listeners_CCNotificationCenter                                1353 ns       1353 ns     535779 sum=204.383M
-----------------------------------------------------------------------------------------------------------------------
check100NotificationsFor10kListeners                                   315 ns        315 ns    2208261 sum=370.782M
check100NotificationsFor10kListeners_EventBus2                         307 ns        307 ns    2288071 sum=1122.83M
check100NotificationsFor10kListeners_CCNotificationCenter           109629 ns     109620 ns       6493 sum=61.1035M
-----------------------------------------------------------------------------------------------------------------------
check100NotificationsFor1kListeners                                    108 ns        108 ns    6351160 sum=1.85786G
check100NotificationsFor1kListeners_EventBus2                          102 ns        102 ns    6985920 sum=-1.55558G
check100NotificationsFor1kListeners_CCNotificationCenter              9684 ns       9683 ns      67812 sum=64.3065M
-----------------------------------------------------------------------------------------------------------------------
check10Listeners                                                        22 ns         22 ns   32136715 sum=1.19719G
check10Listeners_EventBus2                                              21 ns         21 ns   32519065 sum=1.21143G
check10Listeners_CCNotificationCenter                                  264 ns        264 ns    2572170 sum=98.1205M
-----------------------------------------------------------------------------------------------------------------------
check10NotificationsFor1kListeners                                     273 ns        273 ns    2609987 sum=-1.81219G
check10NotificationsFor1kListeners_EventBus2                           267 ns        267 ns    2652159 sum=-1.77676G
check10NotificationsFor1kListeners_CCNotificationCenter              11172 ns      11171 ns      62865 sum=54.023M
-----------------------------------------------------------------------------------------------------------------------
check1kListeners                                                      2158 ns       2158 ns     317734 sum=1.18365G
check1kListeners_EventBus2                                            2168 ns       2168 ns     326126 sum=1.21491G
check1kListeners_CCNotificationCenter                                12919 ns      12918 ns      54781 sum=208.973M
-----------------------------------------------------------------------------------------------------------------------
check1kNotificationsFor1kListeners                                     129 ns        129 ns    5459854 sum=1108.69M
check1kNotificationsFor1kListeners_EventBus2                           103 ns        103 ns    6867175 sum=-1.60612G
check1kNotificationsFor1kListeners_CCNotificationCenter               9711 ns       9710 ns      67611 sum=64.9171M
-----------------------------------------------------------------------------------------------------------------------
checkNotifyFor10kListenersWhenNoOneListens                               2 ns          2 ns  399367972 sum=0
checkNotifyFor10kListenersWhenNoOneListens_EventBus2                     2 ns          2 ns  400616370 sum=0
checkNotifyFor10kListenersWhenNoOneListens_CCNotificationCenter     125173 ns     125162 ns       5657 sum=0
-----------------------------------------------------------------------------------------------------------------------
checkSimpleNotification                                                  7 ns          7 ns  100833112 sum=384.648M
checkSimpleNotification_EventBus2                                        5 ns          5 ns  144959897 sum=552.978M
checkSimpleNotification_CCNotificationCenter                           172 ns        172 ns    4179021 sum=15.9417M
```
So comparing to CCNotificationCenter, EventBus is something like ~10x FASTER especially when we have more unique notifications.

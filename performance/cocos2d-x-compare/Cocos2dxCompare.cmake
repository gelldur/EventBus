
set(CCNOTIFICATION_CENTER_SRC
	cocos2d-x-compare/CCNotificationCenterPerformance.cpp
	cocos2d-x/cocos2dx/support/CCNotificationCenter.cpp
	cocos2d-x/cocos2dx/cocoa/CCObject.cpp
	cocos2d-x/cocos2dx/cocoa/CCArray.cpp
	cocos2d-x/cocos2dx/cocoa/CCInteger.h
	cocos2d-x/cocos2dx/cocoa/CCAutoreleasePool.cpp
	cocos2d-x/cocos2dx/support/data_support/ccCArray.cpp
	cocos2d-x/cocos2dx/cocoa/CCGeometry.cpp
	)
set(CCNOTIFICATION_CENTER_INCLUDE
	cocos2d-x/cocos2dx/
	cocos2d-x/cocos2dx/include
	cocos2d-x/cocos2dx/platform/linux
	cocos2d-x/cocos2dx/kazmath/include
	)

add_definitions(-DLINUX)

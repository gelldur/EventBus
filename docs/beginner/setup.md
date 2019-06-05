# Preparation phase

## Prepare for usage (not going to be a contributor)
 
1. First of all thank you! :)
2. Get the repository\
	- You can just download repository without Git.
	[(How)](https://help.github.com/en/articles/cloning-a-repository)
	- You can clone repository: `git clone git@github.com:gelldur/EventBus.git`
	[(How)](https://help.github.com/en/articles/cloning-a-repository)
	- TBC (package managers)
3. Link to library (TBC)
 
## Prepare for development (for console) users
1. First of all thank you!!! :) :)
2. Please make fork of this repository.\
2.1. How to make fork: https://help.github.com/en/articles/fork-a-repo
3. Clone repository (replace `${USERNAME}`) `git clone git@github.com:${USERNAME}/EventBus.git`
([How to clone](https://help.github.com/en/articles/cloning-a-repository))\
3.1. In project directory `git submodule update --init --recursive` in project directory
(If you did default clone then folder name should be `EventBus/` in place where you executed clone)\
3.2. Please install [Catch2](https://github.com/catchorg/Catch2) as it is needed for unit testing.\



# Setup

## [Clion](https://www.jetbrains.com/clion/) (corss platform IDE)
1. Just open in Clion main folder with CMakeLists.txt
2. You are ready to go ;)

## CodeBlocks
TBC

## General approach

### Linux
Build & Run tests
```bash
# We start at project folder: EventBus/
mkdir build
cmake ..
# This will build EventBus library + unit test executable (which uses Catch2 and EventBus library)
cmake --build . --target EventBusTest
# Run and pray :)
./test/EventBusTest
# You should see: (On the end)
# All tests passed (1828 assertions in 18 test cases)
```

### OSX
TBC but you can follow Linux I think ?


### Windows
TBC need to get Windows!




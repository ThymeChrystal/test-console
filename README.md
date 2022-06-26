# A test 'console/terminal'
For some of my other projects (currently private until they have a decent amount of functionality) I wanted a console for entering commands within the program.

Initially, `std::cin` was used to get the input, but this requires the user to press *<Enter>* before the command can be processed. This means that you can't retrieve a history with up/down arrows, and you can't implement auto completion with *<Tab>*.
  
Therefore, I wanted to implement a console that can handle one character at a time, and this is a testbed for that console. Hopefully, something useful comes out of it!
  
I intend to implement Windows, Linux, MacOS and Qt based versions.

The aim is to make sure all platform decisions are pushed to the configuration (CMake) rather than needing lots of `#if` statements throughout the C++ code.
  
## Getting the code
You can get the code with:
```
git clone git@github.com:ThymeChrystal/test-console.git
```
or
```
git clone https://github.com/ThymeChrystal/test-console.git
```
or you can download the code as a [zip file](https://github.com/ThymeChrystal/test-console/archive/refs/heads/main.zip)
  
## Build the code
The code uses [CMake](https://cmake.org/), and so can be built by creating a build directory alongside the cloned repository and running CMake commands:
```
mkdir build
cd build
cmake -G "Ninja" -D CMAKE_BUILD_TYPE=Release ../test-console
cmake --build .
```
> Note: The above example uses [Ninja](https://ninja-build.org/), but you can use another generator.

### Building with Qt
Coming soon!
 
## Running the code
Once built, the executable will be in the *build/bin* directory (possibly in a *Release* sub-directory on Windows).
  
> Note: This code is only for me to test a console implementation, so isn't properly tested here. It may or not work on your system! However, if it doesn't work, let me know the problem so I can improve it!

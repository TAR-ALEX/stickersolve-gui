# stickersolve-gui

Stickersolve-gui is a GUI application that is capable of finding solutions to the Rubik's cube at fast speeds. It is also capable of rating solution based on speed optimality (for fingertricks).
### The latest release can be found here for windows and linux: [1.0.0](https://github.com/TAR-ALEX/stickersolve-gui/releases/tag/1.0.0)

Stickersolve-gui uses the [stickersolve](https://github.com/TAR-ALEX/stickersolve.git) library to solve the cube. This solver is comparable to the legendary [CubeExplorer](http://kociemba.org/cube.htm) (even beating it in some circumstances on modern hardware). However, the hardware requirements for this solver are quite demanding to achieve its full potential.
The recommended specs for the solver are 9 GB of free memory (can be configured to use less, performance will suffer) and a good cpu with lots of cores. This application will utilize concurrency and memory bandwidth to the maximum. Unlike CubeExplorer, this solver is geared towards finding all solutions and not just one solution (although it can do this). For example: this solver can find all the solutions to the superflip that are 20 moves within less than a day (approx. 3 hours on a Ryzen 1700). In theory, this solver should run without any problems on the major operating systems (Mac, Linux, Windows) although a MacOS build is not yet available.

![image](https://github.com/TAR-ALEX/stickersolve-gui/assets/71244213/11bcc018-b9e2-4482-983c-d01ca39492c4)

# Usage
## 3x3x3 Painter widget
- the color selector is the 1x1x1 cube at the bottom, simply left click on the color to select it.
- for color scheme changes, right click on the 1x1x1 cube to bring up a color selector.
- on the 3x3x3 cube left click to paint a sticker, right or middle click to set the sticker to the grey/(dont care) color.

## UI Controlls
- solved state - the state you wish to reach at the end of a solution
- initial state - the state you start with
- clean - clears the puzzle to the gray color
- swap - swaps the final and initial state
- reset - resets the puzzle to the default state
- apply - applies listed in the box above (does not support parentheses or commutator notation for now)
- allowedMoves - a list of moves separated by spaces that are allowed in the solutions (can be wide or slice moves, however, I dont recommend rotations)
- Max Memory (Gb) - the max amount of memory allowed to use for the pruning tables (this does not limit other consumers of memory, such as redundancy tables or UI usage) The proper table will be generated and used that will fit this criteria (9 Gb is ideal, and is what the solver was tuned to use, however, more is better!)
- Search Depth - the number of moves allowed in the final solutions.
- Num Solutions - the number of solutions to get (-1 or 0 is get them all)
- Sorting - has three options, sorting will be done after the search was finished or canceled, sorting can be applied in post (If there are many solutions, sorting fingertricks can take some time)
    - disabled - print the solutions as they are found
    - length - sort the solutions based on the number of moves taken
    - fingertricks - rate the solutions based on human fingertricks (BETA, does not support slice or wide moves, except wide r and l)
- solve - Starts the solver
- cancel - Aborts the solver. Can also abort the table generation phase, the button may need to be pressed several times to cancel table generation.
# Building From Source On Debian Based Linux

Install a package that has a c++ compiler and cmake on a debian system. qt5 is also needed. I recommend this command to get started: 

```
sudo apt install clang build-essential cmake qt5-default
```

in order to not pollute the system with dev headers for boost and qt5 I recommend using [dep-pull](https://github.com/TAR-ALEX/Cpp-Dependency-Manager.git) to pull the headers locally. Simply `cd` into the project directory and run [dep-pull](https://github.com/TAR-ALEX/Cpp-Dependency-Manager.git)

Alternatively, the packages can be installed systemwide (untested):
```
sudo apt install qtbase5-dev libgl-dev libboost-all-dev
```

I recommend building the project with visual studio code in RELEASE mode, the release mode is critical to the performance of the solver.

However, a standard command line build should work just fine: 

```
mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . --target stickersolve-gui --config Release && cd ../
```

# Building From Source On Windows

On windows I recommend using the [dep-pull](https://github.com/TAR-ALEX/Cpp-Dependency-Manager.git) command to install only the git dependencies. (deb will not work on windows) Alternatively, files could be copied over manually from each git repo into the `./vendor/` directory. A build of Qt must be put into the vendor directory, and the same is true for the boost library.

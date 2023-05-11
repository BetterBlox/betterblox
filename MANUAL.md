# Manual
## Building
Building is a bit tricky as we don't really know how to do it without the IDE so we have uploaded pre-compiled versions of the game just in case.  
### CLion Building
1. Clone the repo and its submodules: `git clone git@github.com:Betterblox/betterblox --recursive`
2. Build vcpkg package manager, this is OS specific:
   1. On Windows: `./vcpkg/bootstrap-vcpkg.bat`
   2. On macOS: `./vcpkg/bootstrap-vcpkg.sh`
3. In CLion, right click `CMakeLists.txt` and click `Reload CMake Project`
4. Build or Run the game using the buttons on the top left of the CLion UI, this is OS specific.
   1. On Windows: Select `Windows Optimized Debug` as the configuration.
   2. On macOS: Select `Debug` as the configuration.
### Pre-Built
1. Go to the releases section on the repo.
2. Download the OS-specific files and extract them.
3. Run the executable to play.

## How to Play the Game
### Movement
- W - Forward
- A - Left
- S - Back
- D - Right
- E - Up
- Q - Down

### Interaction
- Left Click - Place Block
- Right Click - Delete Block

### Block Selection
- 1 - Diamond Ore
- 2 - Wood
- 3 - Face
- 4 - Bedrock
- 5 - Grass
- 6 - Water

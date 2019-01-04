### 0.3.2-alpha Maroon

**23.11.2018 - `(unstable)`**

- **User Part**
  - Fixed issue with downloading big files
  - Fixed bug with video looping
  - Fixed bug with black bars that should appear during proportional scaling
  - Fixed issue when audio and video in the same region led to the crash

- **Developer Part**
  - Fixed bug with Gtk-CRITICAL for drawing area


### 0.3.1-alpha Maroon

**05.11.2018 - `(unstable)`**

- **User Part**
  - `--example-dir` added again to simplify testing
  - Added layout scaling support from scratch (old system contained a lot of bugs)
  - Added snap package: users can easily download and update player from snapstore
  - Size setting unified and less error-prone now
  - Region looping fixed
  - Region ordering fixed
  - Fixed window resize during runtime
  - Fixed bug with image size
  - Background color is checked and parsed properly now
  
- **Developer Part**
  - A lot of new unit-tests that covers main player classes (Layout, MediaContainer, all media, some utils)
  - Tests moved to subproject and splitted into folders
  - Classes depends on interfaces instead of real objects now to simplify future changes in the project
  - All interaction with GTK+ moved to adaptors
  - A lot of minor improvements and refactoring
  - Media splitted into visible and invisible media to simplify handling
  - Parsing and creation (using Builder) logic was splitted
  - Libs updated to the newer ones
  - Added code coverage report
  - camelCase is used everywhere now
  - std::filesystem is used instead of boost::filesystem

### 0.3-alpha Maroon

**27.08.2018 - `(unstable)`**

- **User Part**
  - `--example-dir`, `--log-level` options removed
  - Now player can be launched using only CMS
  - `--host`, `--server-key`, `--hardware-key` options are used to establish connection with CMS
  - Default resources folder (cannot be changed for now) is `resources`. Downloaded files are placed there.
  - Implemented basic collection interval (2 requests for now: RegisterDisplay and RequiredFiles)
  - Only HTTP download type is supported for now in RequiredFiles
  - Video/audio content cannot be downloaded for now due to size limitations (will be fixed in the next release)
- **Developer Part**
  - Implemented all XMDS requests/responses (except for Schedule response)
  - CommandLineParser was refactored
  - spdlog updated to 1.1.0
  - boost updated to 1.68.0
  - Build is cached now
  - Moved to new CI
  - Added DownloadManager, SOAPManager, XMDSManager
  - Small refactorings in other parts of the project

### 0.2.2-alpha Navy

**12.06.2018 - `(unstable)`**

- **User Part**
  - Audio restart was fixed
  - Region and media resizes correctly now
- **Developer Part**
  - Docker build was fixed
  - Running player from command-line using Docker
  - Unit-tests for parsers and layout using GTest library
  - Large scale parsers refactoring
  - Background is a separate class now
  - C++ wrapper for GStreamer
  - Check for memory leaks
  - Video sink and GStreamer wrapper are separate libs now

### 0.2.1-alpha Navy (hotfix)

**18.04.2018 - `(unstable)`**

- **User Part**
  - Fixed media duration handling
  

### 0.2.0-alpha Navy

**15.04.2018 - `(unstable)`**

- **User Part**
  - Layout scaling (If the Player is 1920x1080 also, then a 1:1 rendering can occur, but if the player is 1280x720 or even 1080x1920, scaling must occur)
  - Audio module
     - Independent media
     - Additional module for other media
  - Duration handling was improved in media
  - Image rendering options (hortizontal align, vertical align, stretching/preserving aspect ration)
  - Fixed video blinking during scaling
- **Developer Part**
  - Media handling refactoring
  - Parsers refactoring
  - Added classes-creators (factory and builders)
  

### 0.1.1-alpha Aqua

**31.03.2018 - `(unstable)`**

- **User Part**
  - Duration fixed for `html` rendering media
  - WebView page reloading fixed
  - Added logging level in command line options
  - Binaries can be downloaded from `releases` on Github
  - Updated README, added CHANGELOG and VERSIONING
- **Developer Part**
  - CommandLineParser added
  - XlfParser refactored
  - Added factory for media, layout and region
  - Build process moved to CMake with Docker
  - Added C++17 support
  - Added continuous integration (Travis CI)
  - Removed `gstreamermm`, `gstreamer` is used now

### 0.1.0-alpha Aqua

**09.03.2018 - `(unstable)`**

- Initial Player release
- Supported media: video, webview, images
- Video
  - can be looped
  - can be muted
- WebView
  - can render `html` files
  - supports `js` scripts
- Layout
  - support for background image
  - support for background color
- Simple parsing of `.xlf` layout file
- Z-ordering for regions
- Timer for changing media after some interval
- Logging in console
- Some options for main window that are avaialble from source code but not from command line:
  - run Player always on top
  - run in Full Screen mode
  - optionally disable mouse pointer
  - no windowing/no "chrome" mode
- Version can be known using `--version` command line option
- Example can be run using `--example-dir=/path/to/dir` option
  - Folder should contain all media files
  - Folder also should contatin .XLF file
Building for OS X    {#breadboard_guide_building_osx}
=================

You can use [cmake][] to generate an [Xcode][] project for Breadboard on [OS X][].

# Version Requirements

These are the minimum required versions for building Breadboard on OS X:

  * [OS X][]: Mavericks 10.9.1.
  * [Xcode][]: 5.1.1 or newer
  * [cmake][]: 2.8.12 or newer
  * [Python][]: 2.7.x

# Before Building    {#building_osx_prerequisites}

Prior to building, install the following software:

  * [cmake][]: You can also manually install from [cmake.org](http://cmake.org).
  * [Python][]: 2.7.x

# Creating the Xcode project using cmake

The [Xcode][] project is generated using [cmake][].

For example, the following generates the [Xcode][] project in the `breadboard`
directory.

~~~{.sh}
    cd breadboard
    cmake -G "Xcode"
~~~

# Building with Xcode

  * Double-click on `breadboard/breadboard.xcodeproj` to open the project in
    [Xcode][].
  * Select "Product-->Build" from the menu.

You can also build the game from the command-line.

  * Run `xcodebuild` after generating the Xcode project to build all targets.
  * You may need to force the `generated_includes` target to be built first.

For example, in the breadboard directory:

~~~{.sh}
    xcodebuild -target generated_includes
    xcodebuild
~~~

<br>

  [cmake]: http://www.cmake.org
  [OS X]: http://www.apple.com/osx/
  [Breadboard]: @ref breadboard_guide_overview
  [Python]: http://www.python.org/download/releases/2.7/
  [Xcode]: http://developer.apple.com/xcode/

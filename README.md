Breadboard   {#breadboard_readme}
==========

## Overview

The Breadboard scripting library is a graph based scripting system designed with
games in mind. Complex behvaiors for game entities can be organized by
interlinking nodes representing game actions.

## Features

  * Graph based scripting: Set up complex game behaviors without needing to use
    code. Just arrange nodes in a graph and connect them together.
  * Extensible: Breadboard can be extended with user-defined Nodes that interact
    with your game
  * Cross-platform: Breadboard can be incorperated in to projects on all major
    platforms.

## Dependencies

Breadboard depends on the following library:

  * [fplutil][]

fplutil provides a number of useful utilities. Breadboard makes use of
`fplutil::intrusive_list` internally to build lists to track various structures
without doing unnecessary allocations.

Additionally, Breadboard ships with some example Modules. These additional
modules in the `module_library` folder depend on the following libraries:

  * [Bullet Physics][]
  * [CORGI][]
  * [Flatbuffers][]
  * [MathFu][]
  * [Pindrop][]

These libraries are all optional and can be conditionally compiled based on
your needs. See the options at the top of the root level CMakeLists.txt to see
how to configure the module library to include just what you need.

## Notes

For applications on Google Play that integrate this tool, usage is tracked.
This tracking is done automatically using the embedded version string (see
`src/breadboard/version.cpp`), and helps us continue to optimize it.  Aside from
consuming a few extra bytes in your application binary, it shouldn't affect your
application at all.  We use this information to let us know if Pindrop is useful
and if we should continue to invest in it. Since this is open source, you are
free to remove the version string but we would appreciate if you would leave it
in.

  [Bullet Physics]: http://bulletphysics.org/
  [CORGI]: http://google.github.io/corgi/
  [fplutil]: http://google.github.io/fplutil/
  [FlatBuffers]: http://google.github.io/flatbuffers/
  [MathFu]: http://google.github.io/mathfu/
  [Pindrop]: http://google.github.io/pindrop/

Breadboard    {#breadboard_index}
==========

[Breadboard][] is a scripting system designed to be easy to use for both
programmers and game designers. Once a library of nodes has been defined, anyone
can take those nodes and connect them together to form more complex behaviors
for their game.

## Features

  * Graph based scripting: Set up complex game behaviors without needing to use
    code. Just arrange nodes in a graph and connect them together.
  * Extensible: Breadboard can be extended with user-defined Nodes that interact
    with your game
  * Cross-platform: Breadboard can be incoperated in to projects on all major
    platforms.

## Dependencies

Breadboard depends on the following libraries:

  * [FPLUtil][]

Additionally, Breadboard ships with some example Modules. These additional
modules in the `module_library` folder depend on the following libraries:

  * [Bullet Physics][]
  * [CORGI][]
  * [Flatbuffers][]
  * [Mathfu][]
  * [Pindrop][]

## Download

[Breadboard][] can be downloaded from:

   * [GitHub][] (source)
   * [GitHub Releases Page][] (source)

**Important**: Breadboard uses submodules to reference other components it
depends upon, so download the source from [GitHub][] using:

~~~{.sh}
    git clone --recursive https://github.com/google/breadboard.git
~~~

## Feedback and Reporting Bugs

   * Discuss Breadboard with other developers and users on the
     [Breadboard Google Group][].
   * File issues on the [Breadboard Issues Tracker][].
   * Post your questions to [stackoverflow.com][] with a mention of
     **fpl breadboard**.

<br>

  [Breadboard]: http://github.com/google/breadboard
  [Breadboard Google Group]: http://groups.google.com/group/breadboard-scripting
  [Breadboard Issues Tracker]: http://github.com/google/breadboard/issues
  [Bullet Physics]: http://bulletphysics.org/
  [CORGI]: http://google.github.io/corgi/
  [FPLUtil]: http://google.github.io/fplutil/
  [FlatBuffers]: http://google.github.io/flatbuffers/
  [GitHub Releases Page]: http://github.com/google/breadboard/releases
  [GitHub]: http://github.com/google/breadboard
  [Mathfu]: http://google.github.io/mathfu/
  [Pindrop]: http://google.github.io/pindrop/
  [stackoverflow.com]: http://stackoverflow.com/search?q=fpl+breadboard

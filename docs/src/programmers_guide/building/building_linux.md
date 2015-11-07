Building for Linux    {#breadboard_guide_building_linux}
==================

## Version Requirements

Following are the minimum required versions for the tools and libraries you
need for building [Breadboard][] for Linux:

  * [cmake][]: 2.8.12 or newer
  * [Python][]: 2.7.x

## Before Building    {#building_linux_prerequisites}

Prior to building, install the following components using the [Linux][]
distribution's package manager:

  * [cmake][]: You can also manually install from [cmake.org](http://cmake.org).
  * [Python][]: 2.7.x

For example, on Ubuntu:

    sudo apt-get install cmake python

## Building

When building Breadboard, you can either build the library as part of another
project, or you can build it as a stand-alone library. To simply build the
library (and the sample project) do the following:

  * Generate makefiles from the [cmake][] project in the `breadboard` directory.
  * Execute `make` to build the library and sample.

For example:

    cd breadboard
    cmake -G'Unix Makefiles' .
    make

To build Breadboard from another project, add the following to your project's
CMakeLists.txt:

    add_subdirectory("${path_to_breadboard}" breadboard)
    include_directories(${path_to_breadboard}/include)

Additionally, if you are making use of the `module_library` packaged with
[Breadboard][], you will need to include additional directories, both the
`module_library`'s include folder as well as the generated include folder if you
are making use of the Flatbuffer-based GraphFactory.

    include_directories(${path_to_breadboard}/module_library/include)
    get_property(BREADBOARD_MODULE_LIBRARY_FLATBUFFERS_GENERATED_INCLUDES_DIR
                 TARGET breadboard_module_library_generated_includes
                 PROPERTY GENERATED_INCLUDES_DIR)
    include_directories(${BREADBOARD_MODULE_LIBRARY_FLATBUFFERS_GENERATED_INCLUDES_DIR})

Additionally, ensure that you are linking the library and it's dependencies when
running `target_link_libraries`:

    target_link_libraries(breadboard)

<br>

  [Breadboard]: @ref breadboard_guide_overview
  [Linux]: http://en.wikipedia.org/wiki/Linux
  [Python]: http://www.python.org/download/releases/2.7/
  [cmake]: http://www.cmake.org/


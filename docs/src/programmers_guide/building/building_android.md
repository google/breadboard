Building for Android    {#breadboard_guide_building_android}
====================

## Version Requirements

Following are the minimum tested versions for the tools and libraries you
need for building [Breadboard][] for Android:

  * [Android SDK][]: Android 4.1 (API Level 16)
  * [ADT][]: 20140702
  * [Android NDK][]: android-ndk-r10e
  * [cmake][]: 2.8.12 or newer
  * [Python][]: 2.7.x

## Before Building

  * Install prerequisites for the developer machine's operating system.
      * [Linux prerequisites](@ref building_linux_prerequisites)
      * [OS X prerequisites](@ref building_osx_prerequisites)
      * [Windows prerequisites](@ref building_windows_prerequisites)
  * Install [fplutil prerequisites][]
  * Install the [Android SDK][].
  * Install the [Android NDK][].

## Building

To include [Breadboard][] in your project you will need to make the following
changes to your Android.mk

  * Ensure that the variable `DEPENDENCIES_BREADBOARD_DIR` is defined and points
    to the directory containing the Breadboard library.
  * Add `libbreadboard` to your list of `LOCAL_STATIC_LIBRARIES`
  * Run `$(call import-add-path,$(DEPENDENCIES_BREADBOARD_DIR)/..)` near the end
    of Android.mk, after you call `include $(BUILD_SHARED_LIBRARY)`
  * Lastly, run `$(call import-module,breadboard/jni)`

Your project should now build and link against Breadboard.

<br>

  [ADT]: http://developer.android.com/tools/sdk/eclipse-adt.html
  [Android NDK]: http://developer.android.com/tools/sdk/ndk/index.html
  [Android SDK]: http://developer.android.com/sdk/index.html
  [cmake]: http://www.cmake.org/
  [fplutil prerequisites]: http://google.github.io/fplutil/fplutil_prerequisites.html
  [fplutil]: http://google.github.io/fplutil
  [Breadboard]: @ref breadboard_guide_overview
  [Python]: http://www.python.org/download/releases/2.7/

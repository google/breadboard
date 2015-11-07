Overview    {#breadboard_guide_overview}
========

## Downloading

[Breadboard][] can be downloaded from [GitHub][] or the [Releases Page][].

~~~{.sh}
    git clone --recursive https://github.com/google/breadboard.git
~~~

## Important Concepts

The Breadboard API contains the following important concepts:
  * [Nodes][] - Individual bits of logic or behavior that can be activated.
  * [Modules][] - Collections of related nodes.
  * [Graphs][] - An arrangement of nodes to form more complex behaviors.
  * [Events][] - How graph actions are triggered.

## Source Layout

The following bullets describe the directory structure of the library.

| Path                          | Description                                  |
|-------------------------------|----------------------------------------------|
| `breadboard` base directory   | Project build files and run script.          |
| `jni`                         | The android makefile for the library.        |
| `samples`                     | Sample projects to demonstrate usage.        |
| `src/breadboard`              | The library's souce code.                    |
| `src/modules`                 | The source code for the default modules.     |
| `module_library`              | Additional modules using external libraries. |

<br>

  [Breadboard]: @ref breadboard_guide_overview
  [Events]: @ref breadboard_guide_events
  [GitHub]: http://github.com/google/breadboard
  [Graphs]: @ref breadboard_guide_graphs
  [Modules]: @ref breadboard_guide_modules
  [Nodes]: @ref breadboard_guide_nodes
  [Releases Page]: http://github.com/google/breadboard/releases

Modules    {#breadboard_guide_modules}
=======

## Overview

Modules are collections of related nodes. Modules exist primarily as a
convenient way to organize nodes and also help avoid name collisions.

## Creating a Module

Modules are created by calling `ModuleRegistry::RegisterModule(name)`. Once a
module is registered, any number of nodes may be added to it.

A number of examples can be seen in the `src/modules` directory. Here is a
typical example of the process of registering a Module as well as the nodes that
go with it:

~~~{.cpp}
    void InitializeStringModule(ModuleRegistry* module_registry) {
      Module* module = module_registry->RegisterModule("string");
      module->RegisterNode<EqualsNode>("equals");
      module->RegisterNode<IntToStringNode>("int_to_string");
      module->RegisterNode<FloatToStringNode>("float_to_string");
      module->RegisterNode<ConcatNode>("concat");
    }
~~~

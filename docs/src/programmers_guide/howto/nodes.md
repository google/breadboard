Nodes    {#breadboard_guide_nodes}
=====

## Overview

Nodes are the basic building blocks of a graph. Most nodes define an `Execute`
function in C++ code which can take any number of arguments, process them, and
then provide any number of outputs which other nodes may then consume.

## Basic Example

As a simple example, see `src/modules/string.cpp`. This file defines the node
ConcatNode, shown below:

~~~{.cpp}
    // Concatenates the given strings.
    class ConcatNode : public BaseNode {
     public:
      virtual ~ConcatNode() {}

      static void OnRegister(NodeSignature* node_sig) {
        node_sig->AddInput<std::string>();
        node_sig->AddInput<std::string>();
        node_sig->AddOutput<std::string>();
      }

      virtual void Execute(NodeArguments* args) {
        auto str_a = args->GetInput<std::string>(0);
        auto str_b = args->GetInput<std::string>(1);
        args->SetOutput(0, *str_a + *str_b);
      }
    };
~~~

When this node is registered with Breadboard, it's OnRegister function is run.
OnRegister is given a NodeSignature object that is used to define the number of
inputs and outputs to this node. In this example, two `std::string` inputs and a
single string output are declared.

Execute is where the behavior for this node is run. In this example, our node
takes its two inputs by calling `args->GetInput<std::string>(index);`. This
function returns the value in the input edge by pointer. When writing nodes you
must take care to match the expected return type for the given edge index with
the signature defined in OnRegister or else the call to `GetInput` will assert.

Finally, the result of the operation (in this case, concatenating two strings)
is passed to `args->SetOutput`. In this case there is only a single output, so
we pass 0 as the first argument, but if there were additional outputs we might
also pass values to other indexes as well. As with input edges, make sure that
the type being passed in matches the type declared in the NodeSignature in
OnRegister.

In some cases you don't need to pass data around but you still want to trigger
execution of a node. This can be achieved by assigning inputs or outputs
with the type `void`. Rather than getting a pointer to the value using
`args->GetInput<void>(index)`, you would simply call `args->IsInputDirty(index)`
(Note: This function can be used on input edges of any type). For output edges
that are declared void, simple call `args->SetOutput(index)` without a value
argument to mark it as dirty.

## Node Inputs

Node inputs can be specified in two ways: Either by pointing at the output of
some other node, or by supplying a default value.

## Node Execution

Nodes have three main functions declared on BaseNode. You will always need to
implement OnRegister to define your node's interface. You may additionally
override the `Initialize` and `Execute` functions. Without implementing at least
one of these functions, your node will do nothing.

Nodes execute if one of two conditions is met.

  * The node has a NodeEventListener associated with it, and an event that the
    node is listening for has been broadcast.
  * As a result of another executing, one of the input edges on this node has
    been updated.

Any time an input edge of a node is changed as the result of a call to SetOutput
it is considered dirty, and will have its Evaluate function called.

## BaseNode interface

### OnRegister

OnRegister is a static method, but it must be defined for your type. You do not
technically override it, but you must still supply a copy of this function.
OnRegister is called automatically when your node is registered with a Module.

### Initialize

Initialize is called once when the GraphState itself is initialized. This can be
a useful place to do any special set up for your node, or to set the value of
output edges that only need to be set once and will never change. Initialize
recieves a NodeArguments structure containing the default values for the inputs.
You may set outputs as well from Initialize, but during the initialization step
nodes that depend on those outputs are not marked as dirty.

### Execute

Execute is where most of the heavy lifting will be done. This would be where you
implement most behaviors. Using the NodeArguments structure you can get any
values your node needs to operate and perform actions such as updating UI,
playing audio, printing debug logs to the console, or responding to phsyics
events. Nodes can be set up for just about any type of behavior that would want
to script.

## Node Registration

To register a node so that it can be used by Breadboard, simply call
`Module::RegisterNode<NodeType>(name)`. You must specify a node name that is
unique to this module.

By default, when constructing nodes the default constructor is used. However,
this is not always ideal (or possible). In these cases, you may additionally
provide custom constructor and destuctor callbacks.

For example, if you are writing a PlaySound node that requires a pointer to the
audio engine, you could write it like this:

~~~{.cpp}
    class PlaySoundNode : public BaseNode {
     public:
      PlaySoundNode(AudioEngine* audio_engine) : audio_engine_(audio_engine) {}
      // ...

     private:
      AudioEngine* audio_engine_;
    };

    // ...

    void InitializeAudioModule(ModuleRegistry* module_registry,
                               AudioEngine* audio_engine) {
      Module* module = module_registry->RegisterModule("audio");
      // Custom constructor for PlaySoundNode since it cannot be constructed
      // with the default constructor.
      auto play_sound_ctor = [audio_engine]() {
        return new PlaySoundNode(audio_engine);
      };
      module->RegisterNode<PlaySoundNode>("play_sound", play_sound_ctor);
    }
~~~

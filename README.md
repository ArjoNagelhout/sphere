# sphere

## ARCHIVED:

Note that this repository is archived and not actively maintained. It serves purely as reference material for how to write a simple hello world rendering experiment using Vulkan, using MoltenVK to enable Vulkan development on macOS and Metal. I never got around to implementing the design goals that are listed below. 

![img](https://github.com/ShapeReality/sphere/assets/16051555/41679e91-c7b0-4f1e-8aad-0cbea2e4adf4)

Sphere is an experimental 3D game engine for creating and authoring AR / VR experiences.

Its main innovation goal is in enabling **full live editing on the target device** (probably at reduced visual quality).

## Iterative approach
Building a game engine is hard, so we employ the software development model of: 
- don't try to write everything perfectly from the start
- build abstractions and refactor as you go
- make small, measurable goals that can be done in a day

## Design goals
- **Cross-platform**, in order of importance: Meta Quest, visionOS, PCVR, iOS, Android
- Written in **C++** and using **Vulkan**
- **No framework dependencies** (e.g. Unity or RealityKit), built from first principles.
  - This means no visionOS passthrough support at first as we use Vulkan with MoltenVK to render to the screen, and apple has not yet enabled the API to use passthrough while rendering to the screen using Metal. 
- Highest possible **visual quality**, focused on performance constrained mobile platforms such as the Meta Quest
  - Light baking
  - Mesh baking
- Live editing support on target device using an **on-device editor**.
- Import from CAD / BIM
- **Cross-platform collaboration**; when someone edits a scene on the target device, the changes should be reflected on desktop as well.  

## Business model

Because a game engine itself doesn't initially make for a sustainable income stream we employ the following business model: 
- Primary income streams:
    - Client projects
    - Consumer facing AR / VR experiences 
- Secondary income stream: 
  - License the engine to other design agencies and studios (on the long term this is the goal)

This model allows for true dogfooding, which is desirable in determining which features are important and essential.

## Why build a custom game engine

1. For fun and educational purposes.
2. **Performance**. The ability to fully control every single line of code that is being run. On performance constrained devices such as the Quest, you can only go so far with an existing game engine. There's always overhead. Now the only limiting factor for something not running smoothly is you.  
3. **Stability**. More stable and well-documented APIs. The lower you go in the technology stack, the more things become an engineering discipline, which should result in better APIs and less breaking changes. 
4. Fuck Unity [1]

## Structure
Follows the [Pitchfork specification](https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs)

- `/libs`: internal modules
- `/external`: external modules
- `/src`: main source code
- `/tests`: tests

[1]: I have grown incredibly discontent with Unity and all of its stupid engineering decisions; 
- inability to support advanced graphics programming techniques due to URP obscuring a lot of the true potential of GPU hardware. 
- ever breaking dependencies between different packages; e.g. XR Interaction Toolkit only works with Input System 1.3
- XR Interaction Toolkit, MRTK, Oculus SDK are all simply not meant for Unity's scene hierarchy and component system.  
- bad and low performance UI systems and components, there are three different systems which are all bad in their own way; Unity UI, Immediate UI, UI Toolkit / Elements
- bad text rendering (using TextMeshPro)
- problematic axis conversions from different software applications
- structuring code dependencies using prefabs (never again) and other ways it promotes bad software engineering practices
- extremely slow shader variants compilation
- incomplete and poorly written documentation
- Unity is an opaque black box. No source code is for the core engine is available unless you're an enterprise willing to pay for it.  
- Unity Hub is an electron application and takes forever to start up, sometimes breaks. 
- no easy runtime importing of assets, AddressableAssets and AssetBundles are a mess and break between Unity versions
- coroutines are slow but required because the Unity API does not support multithreading and async await therefore does not provide any benefit. Because of this you also need to use UnityMainThreadDispatcher to go back to the main thread on completing asynchronous code. 
- Unity treats developers like they're dumb by making all the classes (e.g. from UI Toolkit / UI Elements) you "shouldn't" have to use internal, resulting in having to copy over the entire codebase of the package into your own codebase, and fixing the bug there.

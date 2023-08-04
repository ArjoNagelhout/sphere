# sphere

Sphere is an experimental 3D game engine specifically for creating and authoring AR / VR experiences.

Its main innovation goal is in enabling **full live editing on the target device** (probably at reduced visual quality).  

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
4. Fuck Unity[^1]

## Structure
Follows the [Pitchfork specification](https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs)

- `/libs`: internal modules
- `/external`: external modules
- `/src`: main source code
- `/tests`: tests

[^1]: I have grown incredibly discontent with Unity and all of its stupid engineering decisions; 
- inability to support advanced graphics programming techniques
- ever breaking dependencies between different packages
- XR Interaction Toolkit is incredibly inefficient with an insane amount of overhead. 
- bad and low performance UI systems, there are three different systems which are all bad in their own way. Unity UI, Immediate UI, UI Toolkit / Elements
- problematic axis conversions
- structuring code dependencies using prefabs (never again) and other ways it promotes bad software engineering practices
- extremely slow shader variants compilation
- incomplete and poorly written documentation
- opaque black box; no source code
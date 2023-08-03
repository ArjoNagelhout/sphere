# sphere
Let's reinvent the wheel. 

# Structure
Follows the [Pitchfork specification](https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs)

- `/libs`: internal modules
- `/external`: external modules
- `/src`: main source code
- `/tests`: tests

Uses GLFW and OpenGL to start. 

# Log

### 2023-08-02: Refactored code into separate classes 

I managed to get a triangle on the screen. 
Now I want to try to get a 3d model onto the screen from an obj. 
Tutorials won't help at this point, specifications, books, libraries and repositories
with reference implementations will help.

- [ ] Supply vertex buffers in code, instead of hardcoding them into the shader
- [ ] Load obj using something like assimp. 
- [ ] Add Imgui so that parameters can be quickly changed.
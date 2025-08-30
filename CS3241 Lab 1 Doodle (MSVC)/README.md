
# Lab 1
**Only the related methods within `display` that were used to generate the image were written by me. The rest was provided by [Prof. Cheng](https://www.comp.nus.edu.sg/cs/people/hcheng/)**

## Comments
- The eyes were generated using the intersection of two circles. `glStencil` related methods were used.
- The iris was inspired by The Chaos Game with the algorithm [adapted from here](https://fronkonstin.com/tag/fractals/)
  - If there was more time, color gradients on the eyes itself would have been nice.
- The mouth was hardcoded with polygons one at a time...

## Final doodle
![Final Doodle](Final.jpg)

## Running (Windows)
- Run `x64 Native Tools Command Prompt for VS 2019` as administrator
- Run the following:
```
cl main.cpp /I"include" /link lib/freeglut.lib opengl32.lib glu32.lib
```
- Lastly run `main.exe`

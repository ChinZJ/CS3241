# Lab 2
**Refer to the initial commit history for code that was provided by [Prof. Cheng](https://www.comp.nus.edu.sg/cs/people/hcheng/)**

## Comments
- Circle segments for Saturn and Jupiter are not perfect and was done shoddily due to time constraints.
- Rocket around Earth rotates clockwise by default and flips when toggled to time mode (t).
- Planets rotate anticlockwise by default and toggles to clockwise in time mode.
- Ufo will always jump on top of Neptune with respect to the screen itself.

## Final doodle
![Final Product](Final.mp4)

## Running (Windows)
- Run `x64 Native Tools Command Prompt for VS 2019` as administrator
- Run the following:
```
cl main.cpp /I"include" /link lib/freeglut.lib opengl32.lib glu32.lib
```
- Lastly run `main.exe`

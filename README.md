*Project 1 - A first person a third person camera revolving around an array of 3D spaceships in OpenGL*

- Key input is not reliant on the window in focus - 
  all input is reflected in both windows

- Right/Left/Up/Down keys control the camera's position, PageUp/
  PageDown control the FOV
  
- The user can exit the program by hitting the 'x' or espace keys
- Wireframe mode is easily toggled on and off with the 'w' key
- Help text is displayed onscreen in a stationary fashion
- The third person view shows the ships from both the X and Z axes
- The far plane is shown graphically in the third person view,
  reflecting where the ships will stop rendering in real time
- The sphere on which the camera orbits and a representation of the camera
  can be seen in the third person view
- Modified ship body made of a sphere, cylinder, and scaled sphere
  (For a sleek and different model)
- gl_Lighting is enabled, giving very simple, smooth shading model
  (no spectral highlights)
- the near and far planes can be adjusted and reset to default using
  the keyboard
- Full mouse control in the first person window, movement and fov

================

*Project 2 - Renders a spaceship and mars model(imported from heightmap) in various camera modes*

Controls:
- arrow keys or mouse move the camera in most modes (mouse wheel changes fov)
- arrow keys control ship in 3rd person mode
- Controls as in spec: F1 to go through modes, w to toggle wireframe, s to toggle starfield, x/esc to quit
- n toggles normals (debugging feature)
- f2 toggles between our phong and cel shader

Features:
- Cel shader in addition to phong shader
- Different phong shader (without specular) used for mars
- Stars randomized in size as well and have a parralaz effect when moving
- Can look up and down in first person mode
- Control handled in timer function to minimize delay

================

*Project 3 - A simple music visualizer that changes dynamically to any song loaded into it*

Controls:
- Q to toggle between post-processed background and screen space shader
- PgUp/PgDown to speed up/slow down the action
- P to pause
- Esc to quit

Features:
- A dynamic music visualizer rendered in the back ground using a post-processing method
- Very long, transparent ribbons formed from splines
- Spheres with different shader examples: 
	- Model space texture: based on the "energy" from the music, the bouncing spheres change color
	-  Eye space texture: the sphere in the front displays a texture created from its position within a 3D tecture
	- Post process texture: The sphere in front of the blue planet gives a raindrop effect by rendering the background onto it
	- multi-light ADS shader: The blue planet has two specular highlights
	- Screen space shader by pressing Q, changes based on screen position

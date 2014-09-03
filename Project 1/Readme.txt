CS559 - Fall 213 Semester - Project 1
Jackson Reed and Zhouyang Michael Ye


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
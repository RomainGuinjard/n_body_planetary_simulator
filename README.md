# Planetary simulation

This project aims to simulate the movements of multiple bodies in 2D interacting with each other through gravitational forces.
The graphic interface generated using raylib will allow the user to stop or change the pace of the simulation, modify body properties...
The user will also be able to load an initial configuration through a text file.

The simulation core of this project uses Newton's second law of motion, and Euler's method to determine velocity and position.  
This simulation also handles the collisions between bodies. When a collision occurs, the two bodies merge.  
Additionally, when a body is on his way to infinity and beyond (detected when a body is going *too far* from the heaviest body), it is deleted to prevent overflow errors.

Main features (and their [keyboard shortcuts]):  
- Pause the simulation [SPACE] and adjust its speed with dt
- For each body : modify the mass, position, velocity and radius
- Draw trails [T] and ajust the number of previous positions used to draw these
- Draw velocity vectors [V] and adjust their length
- Load a new file [L]
- Reset the simulation [R]
- Reset the camera [R]
- Center the camera on a body by clicking on it
- Pan the camera [→ ↓ ↑ ←]
- Zoom in and out with the mouse wheel
- Adjusting the distance radius and AU scales (to effectively change the AU parameter, you need to change it and reset the simulation)

Known limitations :  
- Accelerating too much the simulation (ie. setting dt too high) makes the simulation become incoherent
- Resizing the window too much may hide some buttons

**IMPORTANT:**  
This project requires the raylib library, that can be found here:  
https://github.com/raysan5/raylib (github project)  
https://www.raylib.com/ (official website)

To install and run our project :  
```bash
cd ~
git clone https://github.com/RomainGuinjard/n_body_planetary_simulator.git
cd n_body_planetary_simulator
make 2d
make 3d
./2d_model/main.x
./3d_model/main3d.x
```

To install raylib :  
```bash
sudo apt update
sudo apt install build-essential git libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev libxinerama-dev libxcursor-dev
cd ~
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo cp ~/raylib/src/libraylib.a /usr/local/lib/
sudo cp ~/raylib/src/raylib.h /usr/local/include/
sudo ldconfig
```

This project also uses the tinyfiledialogs program, but this one is directly incorporated in the project so it installs automatically with the planetary simulation project.  
The official version is hosted at https://sourceforge.net/projects/tinyfiledialogs/


The simulation files use this syntax: 
```text
<number_of_bodies>
<name> <mass_kg> <position_x> <position_y> <velocity_x> <velocity_y> <radius> <r> <g> <b> <a>  
<name> <mass_kg> <position_x> <position_y> <velocity_x> <velocity_y> <radius> <r> <g> <b> <a>  
...
```  

Example :
```text
2  
sun 1.989e30 0 0 0 0 696340000 255 255 0 255 
earth 5.972e24 1.496e11 0 0 29780 6371000 0 100 255 255
```  

Nicolas LE BOUAR, Romain GUINJARD


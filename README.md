# Thescent

Thescent is an OpenGL game prototype based on Descent(1995). It was originally developed as the final project for the Game Development: Advanced Programming post-graduate at Fanshawe College.
This illustrates a complex use case of C/C++ in a game engine (BellEngine).
The engine also supports Lua for scripted animations and the game itself is packaged in a installer written in Wix.
More information is available in the documentation folder. 

Developer: Felipe Da Silva Bellini

# A - PLAYING THE GAME:
1.	An installer is available at this repository, at FOL and at Itch.io.
2.	Install the game. Since the installer is not signed, some antivirus programs might block it.
	If that is the case, just disable the antivirus and try again.
3.	Launch Thescent from the shortcut created on your desktop.

# B - HOW TO BUILD AND RUN:

1.	Clone the source code from this page.
2.	Run BellEngine.sln, this solution was created with VS2019 and VC++ 142.
3.	Rebuild the solution.
	This should work for all combinations of configuration (Debug/Release) and platform (x86/x64). Release/x64 preferably.
4.	Run Game to load the simulation.
5. 	Read the documentation files, for the paper on Thescent $(SolutionDir)Documentation\Thescent.pdf, for controls $(SolutionDir)Documentation\Controls.docx and for architecture $(SolutionDir)Documentation\Architecture.pptx.
6.	Check the assets folder for dynamic loaded data.
7.	Check assets/config/level.xml, the layout descriptor of the simulation, and assets/config/game.cfg, the configuration 
	file that holds control bindings and video properties. A description of the shader programs used is at assets/config/shader_programs.xml
	Level.xml is the only file needed for changing the content that will be loaded into the game.

# C - PROJECTS:

1.	BellEngine.GL:
	Provides access to OpenGL, GLAD, GLFW, etc.
2.	BellEngine.Common:
	Provides access to GLM, etc.
3.	BellEngine.Interfaces:
	Provides generic interfaces used across all projects.
4.	BellEngine.Utilities:
	Contains utilities and helpers for the whole solution.
5.	BellEngine.Core:
	Base project engine.
6.	BellEngine.Sound.OpenAL:
	Wrapper project containing the Sound System running on OpenAL.
7.	BellEngine.Physics.Custom:
	Wrapper project containing Physics system with custom implementation.
8.	BellEngine.Physics.Bullet:
	Wrapper project containing Physics system with Bullet engine.
9.	BellEngine.Mesh.Assimp:
	Provides a mesh system based on Assimp.
10.	BellEngine.Mesh.PlyReader:
	Provides a mesh system based on a custom ply reader.
11.	BellEngine.AI.Custom:
	Provides an AI system for games.
12.	BellEngine.VertexAnimation.Animation:
	Contains the SkinnedMeshComponent for vertex animations. Will be merged in the future.	
13.	BellEngine.Scripting.Lua:
	Will contain a scripting system based on Lua.
14.	Game.DataPersistency.XML:
	Reads objects stored in XML.
15.	Game:
	This project produces the application for Thescent.
16. Game.Setup: 
	This project produces a WiX MSI installer package for the binaries.
17. Game.Setup.Full: 
	This project produces a WiX EXE bundle installer package for the MSI and the runtimes of Visual C++ 2019 and OpenAL.

# License
Licensed under AGPL-3.0 License

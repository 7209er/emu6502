## Update
I added the DrawingDevice class, which enables the CPU to draw quadrilaterals.
DrawingDevice lives on the bus and functions as a tiny drawing device. Under the
hood it uses OpenGL to draw the figures. To access it, simply write data into 0x0500
to 0x0501. If 0x1 is written into 0x0502, the DrawingDevice will read the new data and
updates a vertex (consisting of to BYTES/ a x- and y-coordinate). As OpenGL works with 
a range from -1 to 1, the BYTES now have to be interpreted as signed. You can simply
put in a coordinate from -128 to 127 and it will get converet automaticaly to a -1 to 1 float.
*For OpenGL to work, you have to set up MESA, GLFW, GLAD and GLEW accordingly*
*and include glad.h/glfw3.h and have glad.c in your repository*
Also there is a little OpenGL-setup debug mode by using DEBUG=-DDEBUG with the make command.

## Original Text
Reccently I got interested in CPU emulation. Some friends suggested that I should look 
into the 6502 because of its simple design. So I tried a few approches, guided by the
docs. Then I came accross Javidx9's video series of NES emulation.
I found his approach quite nice, and thus decided to implement it myself.
Here is a link to his channel: https://www.youtube.com/javidx9 

I made some minor changes, like using Bitfields and an array instead of vectors.
Additionally, I created a mock device that outputs a text if its memory address
gets new data. I plan on integrating a line drawing device using OpenGL next.
~7209er
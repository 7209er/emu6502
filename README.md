Reccently I got interested in CPU emulation. Some friends suggested that I should look 
into the 6502 because of its simple design. So I tried a few approches, guided by the
docs. Then I came accross Javidx9's video series of NES emulation.
I found his approach quite nice, and thus decided to implement it myself.
Here is a link to his channel: https://www.youtube.com/javidx9 

I made some minor changes, like using Bitfields and an array instead of vectors.
Additionally, I created a mock device that outputs a text if its memory address
gets new data. I plan on integrating a line drawing device using OpenGL next.
~7209er
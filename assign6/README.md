Include information for grader about your assign6 here
The assignment is attached. ps2_FSM.jpeg is the finite state machine diagram.

Anti-aliasing lines ----------------

The extension is attached. For drawing anti-aliased lines, I based my design upon Xiaolin Wu's algorihtm - https://www.youtube.com/watch?v=f3Rs20k-hcI&ab_channel=NoBSCode. A line is drawn from endpoint to endpoint. We then look at the pixels above and below the line. The idea is to vary the brightness of a single pixel based on the vertical distance to the line. To place this line, we follow Bresenham's algorithm, described here: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm. 

For a detailed design, here it is. The idea is to first determine if we are going to step by dy/dx or dx/dy depending on if it is steeper in the x and y direction. For simplcity, let us pretend we are moving in dy/dx phase, where we step by x and find a resulting y. Everything is just inverted for dx/dy. 

We first choose to step along the line by a single pixel. We continously store the previous pixel we stepped on.

Case 1: We plot two points, one at and (below/above) the previous pixel.

Case 2: If the current position's y-value is more than 0.5 away from the y of the previous, we consider redifining the previous point. We then also shift the two points we originally planned on plotting.

With two points to plot, we define their distance from the current position (y-direction). Their brightness is an inverse of their distance.

Repeat again and again, but do note some exceptions. We need to be handy on always moving from left to right or lowest to highest. We also plot the last point and first point separately. 

Triangles ----------------

Drawing a triangle was based upon the steps detailed in the article that was shared with us: https://gabrielgambetta.com/computer-graphics-from-scratch/07-filled-triangles.html#drawing-filled-triangles. The idea is to classify the three vertices into distinctions of low, middle, and high. We can then interpolate the points between the three vertcies to represent each side in an array, going from lowest to highest y. 

If we visualize the opposite sides as an interpolation of points, we can see our triangle as a stack of lines, with the end points determined by the arrays we created. We thus call drawline to connect the endpoints of opposite side arrays, incrementing from lowest to highest y.

Some smaller details are checking which side is left and right, and the use of anti-aliasing for the sides.

Splash-screen ----------------

The startup screen slowly fills the area with white, line by line. It then removes the white area line by line (now larger lines to speed it up).

Floating-point operations ----------------

Floating point operations are enabled by changing the 13th bit of the mstatus register to be 1, representing us turning on the FPU. This can only happen once in the code.

The makefile was modified to change the march command to rv64imafdc.

I measured the time to draw a large triangle on the screen.

New time:  772488 us.
Old time: 3034083 us.

About a 4x speed increase.



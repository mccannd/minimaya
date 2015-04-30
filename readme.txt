MiniMaya

======================
Deformations (Dan)
=====================
Additional Features/Extra Credit: Move light, colored light, smooth vertex normals

>>>"Keys" Tab <<<
I'm not quite sure how this works - Ellen

>>>"Light" Tab<<<
[ ] Check Smooth Vertex Normals to show smooth vertex normals.
Adjust X,Y,Z values to adjust position of light
Adjust RGB values to adjust color of light

======================
RayTracing (Michael)
=====================
Additional Features/Extra Credit:  Toon Shader

R - renders to file

Also not quite sure what else to do - Ellen

=====================
Deformations (Ellen) - All under "Deformation" Tab
=====================
Additional Features/Extra Credit: Multiple vertices selection in DFF, Excess credit on onscreen controls, commit feature in deformations

Hotfixes:
N - Recalculates boundaries
M - Recreates the lattice (may need to press this at the start


GUI
[ ] Lattice is Active - Check this to get the global deformations working
Choose a type of global deformation, axis, and deformation intensity to do global deformations. Show lattice to see the deformation clearer.

[ ] Show Lattice - Shows the lattice if lattice is active and allows you do to free form deformations
Click to select points of the lattice. 
SHIFT + Click to add to your selection.
Click empty space to deselect everything.

Move these points using
U - up
J - down
K - right
H - left
Y - forward
G - backward

Change the number of subdivisions in your lattice using:
W - increase in y
S - decrease in y
A - decrease in x
D - increase in x
E - increase in z
Q - decrease in z

Commit Deformations - Like your deformations? SAVE THEM!

Onscreen Controls ***(MAY need to click blank space for it to update the display)
Lattice needs to be active and Show Onscreen Controls needs to be active.
Choose Translate, Rotate, or Scale.
Translate - Shows up as just three lines
Rotate - Shows up as three lines with circles
Scale - Shows up as three lines and squares around it
The center of the onscreen control will highlight to the color of the lattice




=====================




Homework 5 and 6: by Dan McCann


Control the camera with the arrow keys to change the angle, and zoom In and Out with I and O.


The user interface is split into four tabs: Edges, Faces, Vertices, and Skeleton

The new vertex shader is skeleton.vert.glsl


-----
Edges
-----

HW5:

Click on an edge in the list to select it. 
A selected edge will be drawn over the mesh.
When you select something that isn't an edge, the edge is considered deselected.

Click the Divide Edge button to bisect the edge with a new vertex.

HW 6:

Click the Select Next Edge button to move the selection to the next edge on the face. 
The direction of the edge flow is shown by the gradient on the edge.

Click the Select Sym Edge button to select the opposite half edge to the current one.
You should see the gradient flip, and selecting the next edge will move along a different face than the first.

-----
Faces
-----

HW 5:

Click on a face in the list to select it.
A selected face will be represented by a ring of edges surrounding it.

Three sliders can be used to determine the red, green, and blue levels of the color of the face.
You must press the Set Color button to apply the color on the sliders.

Click Triangulate Face to turn the selected face into a fan of triangles. This works on convex Ngons.

--------
Vertices
--------

HW5:

Click on a vertex in the list to select it.

When a vertex is selected, it will be displayed on screen and the text fields with its coordinates will update.
You can change the position of this vertex by changing these coordinates and pressing the Reposition Vertex button.

Press delete vertex to destroy the vertex and all incident edges and faces.

----
Main Window -- Repositioned elements in HW 7
----

HW5:

Click on the Reset Cube button to return the mesh to its original state.

HW6:

Click on the Smooth Mesh button to apply Catmull-Clark Subdivision.

Click on the Import OBJ button to open a file selection dialog. Navigate to a .obj file you wish to open and select it.
The obj file will be displayed on the screen. Larger files may take a few seconds.

HW7: 

Click Reset Skeleton to remove the current skeleton and revert it to a single joint at the origin.

Click Bind Skeleton to mesh to create a linear binding between the mesh and the existing skeleton.
This will switch the mesh's vertex shader to the skeleton shader. The fragment shader is still lambert, so there will not be any immediate visual changes.
Adding joints, resetting the skeleton, and importing a new one will revert the mesh to a lambert shader.

Click Import Skeleton JSON to open a file selection dialog. Navigate to a json containing a skeleton and open it.

--------
Skeleton
--------

HW7:

Click on a joint in the tree to select it. This will update any relevant fields in the tab.

You can rotate a joint using Quaternion angles or Euler angles. Each has its own tab and updating one will update the other. Press the button to apply your changes.

Change the position in the x, y, and z spinners then press the Reposition Joint button to update it.

Press Add Child to Joint to create a joint labeled "New Joint" on top of the selected one. You will have to click the arrow in the tree widget to find the new joint.

Finally, you can uncheck "display skeleton" to hide it.

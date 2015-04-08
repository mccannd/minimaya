Homework 5 and 6: by Dan McCann


Control the camera with the arrow keys to change the angle, and zoom In and Out with I and O.


The user interface is split into four tabs: Edges, Faces, Vertices, and Mesh


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
Mesh
----

HW5:

Click on the Reset Cube button to return the mesh to its original state.

HW 6:

Click on the Smooth Mesh button to apply Catmull-Clark Subdivision.

Click on the Import OBJ button to open a file selection dialog. Navigate to a .obj file you wish to open and select it.
The obj file will be displayed on the screen. Larger files may take a few seconds.



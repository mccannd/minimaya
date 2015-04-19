#version 150
// ^ Change this to version 130 if you have compatibility issues

// Given:
//      each vertex
//      a list of joints
//      pairs of joint ids for each vertex
//      pairs of joint weights for each vertex
//      bind matrices for each joint
//      current transformations for each joint
//
// This shader will calculate new, transformed vertices using deformations in a skeleton


uniform mat4 u_Model;       // The matrix that defines the transformation of the
                            // object we're rendering. In this assignment,
                            // this will be the result of traversing your scene graph.

uniform mat4 u_ModelInvTr;  // The inverse transpose of the model matrix.
                            // This allows us to transform the object's normals properly
                            // if the object has been non-uniformly scaled.

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself

// ---- Skeleton Uniform Features -------------------------------------------------------------------------

uniform mat4[100] u_BindMatrix;        // The array of mat4 that represents the bind matrix of joints
                                    // this converts a position in world space to joint local space
                                    // which gives the displacement, in local space, 
                                    // the index of this array is the "ID" of the joint

uniform mat4[100] u_TransformMatrix;   // transformation array of each joint
                                    // joint local space -> world space

// --------------------------------------------------------------------------------------------------------


in vec4 vs_Pos;  // ---------->The array of vertex positions passed to the shader

in vec4 vs_Nor;  // ---------->The array of vertex normals passed to the shader

in vec4 vs_Col;  // ---------->The array of vertex colors passed to the shader.



// ---- Skeleton In Features ------------------------------------------------------------------------------

in ivec2 vs_JID; // the array of vec2s pairing joint IDs influencing this vertex

in vec2 vs_JWeight; // the array of floats determining the influence of each joint

// --------------------------------------------------------------------------------------------------------



out vec4 fs_Nor;  // --------->The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_LightVec;  // ---->The direction in which our virtual light lies, relative to each vertex. This is implicitly passed to the fragment shader.
out vec4 fs_Col;  // --------->The color of each vertex. This is implicitly passed to the fragment shader.

const vec4 lightPos = vec4(5, 5, 3, 1);  // The position of our virtual light, which is used to compute the shading of
                                        // the geometry in the fragment shader.

void main()
{
    
    // find the local DISPLACEMENTS of the vertex for each joint ------------------------------------------
    vec4 a_pos = u_BindMatrix[vs_JID[0]] * vs_Pos;
    vec4 b_pos = u_BindMatrix[vs_JID[1]] * vs_Pos;

    // transform these local displacements by the transformation matrix of each joint
    a_pos = u_TransformMatrix[vs_JID[0]] * a_pos;
    b_pos = u_TransformMatrix[vs_JID[1]] * b_pos;

    // now these are transformed points in world space
    // multiply them by the weights and add together, finalizing the transformation
    a_pos = vs_JWeight[0] * a_pos;
    b_pos = vs_JWeight[1] * b_pos;

    a_pos = a_pos + b_pos;


    // find the local normals of the vertex for each joint ------------------------------------------------
    vec4 a_norm = u_BindMatrix[vs_JID[0]] * vs_Nor;
    vec4 b_norm = u_BindMatrix[vs_JID[1]] * vs_Nor;

    // transforma these local normals by the transformation matrix of each joint
    a_norm = u_TransformMatrix[vs_JID[0]] * a_norm;
    b_norm = u_TransformMatrix[vs_JID[1]] * b_norm;

    // multiply them by the weights and add together
    a_norm = vs_JWeight[0] * a_norm;
    b_norm = vs_JWeight[1] * b_norm;

    a_norm = a_norm + b_norm;


    fs_Col = vs_Col;  //                          Pass the vertex color positions to the fragment shader
    fs_Nor = u_ModelInvTr * a_norm;  //           Transform the geometry's normals

    vec4 modelposition = u_Model * a_pos;  //    Temporarily store the transformed vertex positions for use below

    fs_LightVec = lightPos - modelposition;  //   Compute the direction in which the light source lies

    gl_Position = u_ViewProj * modelposition;  // gl_Position is a built-in variable of OpenGL which is used to render the final positions
                                             // of the geometry's vertices
}

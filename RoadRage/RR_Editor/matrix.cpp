
#include "matrix.h"

D3DMATRIX IdentityMatrix(void)       // initializes identity matrix
{
    D3DMATRIX ret;
    for (int i=0; i<4; i++)
	{
        for (int j=0; j<4; j++)
		{
            ret(i, j) = (i==j) ? 1.0f : 0.0f;
		}
	}
    return ret;
}    // end of IdentityMatrix()


D3DMATRIX ZeroMatrix(void)  // initializes matrix to zero
{
    D3DMATRIX ret;
    for (int i=0; i<4; i++)
	{
        for (int j=0; j<4; j++)
		{
            ret(i, j) = 0.0f;
		}
	}
    return ret;
}    // end of ZeroMatrix()


D3DMATRIX MatrixMult(const D3DMATRIX a, const D3DMATRIX b)  
{
    D3DMATRIX ret = ZeroMatrix(); // shown below

    for (int i=0; i<4; i++) 
	{
        for (int j=0; j<4; j++) 
		{
            for (int k=0; k<4; k++) 
			{
                ret(i, j) += a(k, j) * b(i, k);
            }
        }
    }
    return ret;
}    // end of MatrixMult()


D3DMATRIX Translate(const float dx, const float dy, const float dz)
{
    D3DMATRIX ret = IdentityMatrix();
    ret(3, 0) = dx;
    ret(3, 1) = dy;
    ret(3, 2) = dz;
    return ret;
}    // end of Translate()


D3DMATRIX RotateX(const float rads)
{
    float    cosine, sine;

    cosine = (float)cos(rads);
    sine   = (float)sin(rads);
    D3DMATRIX ret = IdentityMatrix();
    ret(1,1) = cosine;
    ret(2,2) = cosine;
    ret(1,2) = -sine;
    ret(2,1) = sine;
    return ret;
}   // end of RotateX()


D3DMATRIX RotateY(const float rads)
{
    float    cosine, sine;

    cosine = (float)cos(rads);
    sine   = (float)sin(rads);
    D3DMATRIX ret = IdentityMatrix();
    ret(0,0) = cosine;
    ret(2,2) = cosine;

    ret(0,2) = sine;
    ret(2,0) = -sine;
    return ret;
}   // end of RotateY()


D3DMATRIX RotateZ(const float rads)
{
    float    cosine, sine;

    cosine = (float)cos(rads);
    sine   = (float)sin(rads);
    D3DMATRIX ret = IdentityMatrix();
    ret(0,0) = cosine;
    ret(1,1) = cosine;
    ret(0,1) = -sine;
    ret(1,0) = sine;
    return ret;
}   // end of RotateZ()


D3DMATRIX Scale(const float size)
{
	D3DMATRIX ret = IdentityMatrix();
    ret(0, 0) = size;
    ret(1, 1) = size;
    ret(2, 2) = size;
    return ret;
}   // end of Scale()


D3DMATRIX SetProjectionMatrix(const float near_plane,     // distance to near clipping plane
                 const float far_plane,      // distance to far clipping plane
                 const float fov)            // field of view angle, in radians
{
    float    c, s, Q;

    c = (float)cos(fov*0.5);
    s = (float)sin(fov*0.5);
    Q = s/(1.0f - near_plane/far_plane);

    D3DMATRIX ret = ZeroMatrix();
    ret(0, 0) = c;
    ret(1, 1) = c;
    ret(2, 2) = Q;

    ret(3, 2) = -Q*near_plane;
    ret(2, 3) = s;
    return ret;
}   // end of ProjectionMatrix()


D3DMATRIX SetViewMatrix(const D3DVECTOR from,      // camera location
           const D3DVECTOR at,        // camera look-at target
           const D3DVECTOR world_up,  // world’s up, usually 0, 1, 0
           const float roll)          // clockwise roll around
                                      //    viewing direction, 
                                      //    in radians
{
    D3DMATRIX  view = IdentityMatrix();  // shown below
    D3DVECTOR  up, right, view_dir;

    D3DMATRIX viewMatrix;
    int moveForward, moveBackward, moveLeft, moveRight;

    moveForward = moveBackward = moveLeft = moveRight = 0;

    view_dir = Normalize(at - from);
    right = CrossProduct(world_up, view_dir);
    up = CrossProduct(view_dir, right);
    
    right = Normalize(right);
    up = Normalize(up);
    
    view(0, 0) = right.x;
    view(1, 0) = right.y;
    view(2, 0) = right.z;
    view(0, 1) = up.x;
    view(1, 1) = up.y;
    view(2, 1) = up.z;
    view(0, 2) = view_dir.x;
    view(1, 2) = view_dir.y;
    view(2, 2) = view_dir.z;
    
    view(3, 0) = -DotProduct(right, from);

    view(3, 1) = -DotProduct(up, from);
    view(3, 2) = -DotProduct(view_dir, from);
    
    if (roll != 0.0f) 
	{
         //MatrixMult function shown below
         //view = MatrixMult(RotateZ(-roll), view); //KH
    }

    return view;
}  // end of ViewMatrix()


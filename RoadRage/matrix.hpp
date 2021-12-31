
// Road Rage 1.1

// FILE : matrix.hpp
// This file was originally part of the Microsoft DirectX5 SDK
// Edited by William Chin
// Edited with MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Edited on:  3/7/99


#define D3D_OVERLOADS
#include <math.h>
#include <d3d.h>
#include "d3ddemo.hpp"
#include <stdio.h>

D3DMATRIX IdentityMatrix(void); 
D3DMATRIX ZeroMatrix(void);
D3DMATRIX MatrixMult(const D3DMATRIX a, const D3DMATRIX b);
D3DMATRIX Translate(const float dx, const float dy, const float dz);
D3DMATRIX RotateX(const float rads);  
D3DMATRIX RotateY(const float rads);
D3DMATRIX RotateZ(const float rads);
D3DMATRIX Scale(const float size);

D3DMATRIX SetProjectionMatrix(const float near_plane,    
                const float far_plane, const float fov);            

D3DMATRIX SetViewMatrix(const D3DVECTOR from,const D3DVECTOR at,
				const D3DVECTOR world_up, const float roll);          
                                     
                                     

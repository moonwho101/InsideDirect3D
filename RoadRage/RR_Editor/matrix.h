
#define D3D_OVERLOADS
#include <math.h>
#include <d3d.h>
#include "d3ddemo.h"
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
                                     
                                     


#ifndef __ROADRAGE_H
#define __ROADRAGE_H

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
 

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------

class CMyD3DApplication : public CD3DApplication
{

public:
	
    CMyD3DApplication();

	void	DisplayCredits(HWND hwnd);
	void	DisplayRRStats(HWND hwnd);
	void	DisplayLegalInfo(HWND hwnd);

	LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                  LPARAM lParam );

	static HRESULT hr;
			
	HINSTANCE hInstApp;
};

#endif //__ROADRAGE_H
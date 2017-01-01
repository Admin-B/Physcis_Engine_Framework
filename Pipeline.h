#pragma once

#include <DXUTcamera.h>
#include <algorithm>
#include <vector>
using namespace std;

#include "Particle.h"
//Pipeline is Singleton Class.
class Pipeline
{
public:
	static Pipeline * getInstance();

	void OnInit(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC * pBackBufferSurfaceDesc, void * pUserContext);
	void OnUpdate(double fTime, float fElapsedTime, void * pUserContext);
	void OnRender(double fTime, float fElapsedTime, void * pUserContext);
	void OnRelease(void * pUserContext);

	IDirect3DDevice9 * md3dDevice;

	void CALLBACK KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown, void *pUserContext);
	void CALLBACK MouseProc(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down,
							int nMouseWheelDelta, int xPos, int yPos, void * pUserContext);


	CModelViewerCamera * mCamera;
private:
	static Pipeline * Instance;
	Pipeline();
	~Pipeline();

	vector <Particle *> Particles;
};


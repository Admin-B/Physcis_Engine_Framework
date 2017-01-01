#include "DXUT.h"
#include "Pipeline.h"
#include "Collide.h"

Pipeline * Pipeline::Instance = nullptr;
Pipeline * Pipeline::getInstance() {
	if (Instance == nullptr) {
		Instance = new Pipeline();
	}
	return Instance;
}
Pipeline::Pipeline() {}
Pipeline::~Pipeline() {}


void Pipeline::OnInit(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC * pBackBufferSurfaceDesc, void * pUserContext) {
	md3dDevice = pd3dDevice;
	
	//메터리얼 라이트 기본 설정
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 205.f / 255.f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 92.f / 255.f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 92.f / 255.f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;

	pd3dDevice->SetMaterial(&mtrl);

	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(&light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;

	light.Ambient.r = 1.0f;
	light.Ambient.g = 1.0f;
	light.Ambient.b = 1.0f;
	
	D3DXVECTOR3 direction(-1,-1.5,-2);
	D3DXVec3Normalize((D3DXVECTOR3 *)&light.Direction, &direction);

	//light.Range = 100.0f;

	pd3dDevice->SetLight(0, &light);
	pd3dDevice->LightEnable(0, TRUE);

	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);


	// 입자 설정
	Particles.push_back(new Particle());
	Particles[0]->setPosition(D3DXVECTOR3(0,20.0f,0));
	Particles[0]->Width = 5.0f;
	Particles[0]->Depth = 5.0f;
	Particles[0]->Height = 5.0f;
	Particles[0]->setMesh();
	Particles[0]->setVelocity(D3DXVECTOR3(0, -0.05f, .1f));

	Particles[0]->setAcceleration(D3DXVECTOR3(0, -0.001f, 0));
	
	Particles.push_back(new Particle());
	Particles[1]->Width = 50.0f;
	Particles[1]->Depth = 50.0f;
	Particles[1]->Height = 10.0f;

	Particles[1]->setMesh();
	Particles[1]->setMass(999999999.0f);

	int index = 2;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {

			Particles.push_back(new Particle());
			Particles[index]->Width = 3.0f;
			Particles[index]->Depth = 3.0f;
			Particles[index]->Height = 3.0f;
			Particles[index]->setPosition(D3DXVECTOR3(j*3.0f,i*3.0f+5.0f,10.0f));
			Particles[index]->setAcceleration(D3DXVECTOR3(0, -0.001f, 0));

			Particles[index]->setMesh();


			index++;
		}
	}

}
void Pipeline::OnUpdate(double fTime, float fElapsedTime, void * pUserContext) {
	Collide::getInstance()->fTime=fTime;
	int Particles_Size = Particles.size();

	for (int i = 0; i < Particles_Size; i++) {
		Particles[i]->integrate(fTime);
	}

	for (int i = 0; i < Particles_Size-1; i++) {
		for (int j = i + 1; j < Particles_Size; j++) {
			if (Collide::getInstance()->Collision_Processing(Particles[i], Particles[j])) {
				printf("[%d 번째 파티클] 과 [%d 번째 파티클] 이 충돌.\n\n",i,j);
			}
		}
	}
}
void Pipeline::OnRender(double fTime, float fElapsedTime, void * pUserContext) {
	int Particles_Size = Particles.size();

	for (int i = 0; i < Particles_Size; i++) {
		Particles[i]->print();
	}
}
void Pipeline::OnRelease(void * pUserContext) {
	int Particles_Size = Particles.size();

	for (int i = 0; i < Particles_Size; i++) {
		delete Particles[i];
	}
}





//KEYBOARD EVENT
void CALLBACK Pipeline::KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown, void *pUserContext) {
	switch (nChar) {
	case VK_F1:
		Particles[0]->setVelocity(D3DXVECTOR3(0, 0.0f, 0));
		Particles[0]->setPosition(D3DXVECTOR3(0, 20.0f, 0));
	}
}
//MOUSE EVENT
void CALLBACK Pipeline::MouseProc(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down,
	int nMouseWheelDelta, int xPos, int yPos, void * pUserContext) {
}
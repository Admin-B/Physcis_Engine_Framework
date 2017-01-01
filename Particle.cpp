#include "DXUT.h"
#include "Particle.h"
#include "Pipeline.h"

D3DXMATRIX Identity_Matrix;
void * __temp=D3DXMatrixIdentity(&Identity_Matrix);

Particle::Particle()
{
	Mass = 2.0f;
	Inverse_Mass = 1 / Mass;

	Force = D3DXVECTOR3(0.f, 0.f, 0.f);
	Acceleration = D3DXVECTOR3(0.f, 0.f, 0.f);
	Velocity = D3DXVECTOR3(0.f, 0.f, 0.f);
	Position = D3DXVECTOR3(0.f, 0.f, 0.f);

	Transform_Matrix = Identity_Matrix;
	
	Width = 1.0f;
	Height = 1.0f;
	Depth = 1.0f;

	linearDamping = 0.999f;
}

Particle::~Particle()
{
	Mesh->Release();
}


void Particle::integrate(float duration) {
	if (Inverse_Mass <= 0) {
		return;
	}
	Position += Velocity*duration;

	D3DXVECTOR3 resultAcc = Acceleration+Force*Inverse_Mass;
	Velocity += resultAcc*duration;
	Velocity *= pow(linearDamping, duration);

	clearForce();

	D3DXMatrixTransformation(&Transform_Matrix, NULL, NULL, NULL, NULL, NULL, &Position);
	updateMeshVertices();
}

void Particle::setMesh() {
	D3DXCreateBox(Pipeline::getInstance()->md3dDevice, Width, Height, Depth, &Mesh, NULL);

	Initial_Mesh_Vertices.push_back(D3DXVECTOR3(Width / 2, Height / 2, Depth / 2));
	Initial_Mesh_Vertices.push_back(D3DXVECTOR3(-Width / 2, Height / 2, Depth / 2));
	Initial_Mesh_Vertices.push_back(D3DXVECTOR3(-Width / 2, -Height / 2, Depth / 2));
	Initial_Mesh_Vertices.push_back(D3DXVECTOR3(-Width / 2, -Height / 2, -Depth / 2));
	Initial_Mesh_Vertices.push_back(D3DXVECTOR3(Width / 2, -Height / 2, Depth / 2));
	Initial_Mesh_Vertices.push_back(D3DXVECTOR3(Width / 2, -Height / 2, -Depth / 2));
	Initial_Mesh_Vertices.push_back(D3DXVECTOR3(Width / 2, Height / 2, -Depth / 2));
	Initial_Mesh_Vertices.push_back(D3DXVECTOR3(-Width / 2, -Height / 2, -Depth / 2));

	Mesh_Vertices.resize(Initial_Mesh_Vertices.size());
}

void Particle::updateMeshVertices() {
	for (int i = 0; i < Initial_Mesh_Vertices.size(); i++) {
		D3DXVec3TransformCoord(&Mesh_Vertices[i], &Initial_Mesh_Vertices[i], &Transform_Matrix);
	}
}

void Particle::print() {
	IDirect3DDevice9 * & d3dDevice=Pipeline::getInstance()->md3dDevice;
	
	//d3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	d3dDevice->SetTransform(D3DTS_WORLD,&Transform_Matrix);

	Mesh->DrawSubset(0);
	
	d3dDevice->SetTransform(D3DTS_WORLD, &Identity_Matrix);
}




void Particle::setMass(const float pMass) {
	Mass = pMass;
	Inverse_Mass = Mass <= 0 ? 0 : 1 / Mass;
}
void Particle::setInverseMass(const float pInverseMass) {
	Inverse_Mass = pInverseMass;
	Mass = Inverse_Mass <= 0 ? 0 : 1 / Inverse_Mass;
}
void Particle::setAcceleration(const D3DXVECTOR3 pAcceleration) {
	Acceleration = pAcceleration;
}
void Particle::setVelocity(const D3DXVECTOR3 pVelocitiy) {
	Velocity = pVelocitiy;
}
void Particle::setPosition(const D3DXVECTOR3 pPoistion) {
	Position = pPoistion;

	D3DXMatrixTransformation(&Transform_Matrix, NULL, NULL, NULL, NULL, NULL, &Position);
	updateMeshVertices();
}
void Particle::addForce(const D3DXVECTOR3 pForce) {
	Force += pForce;
}
void Particle::clearForce() {
	Force.x = 0;
	Force.y = 0;
	Force.z = 0;
}
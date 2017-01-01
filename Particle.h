#pragma once

#include <algorithm>
#include <vector>
using namespace std;

class Particle
{
public:
	Particle();
	~Particle();

	void integrate(float duration);

	void setMesh();

	void updateMeshVertices();
	void print();

	//get values
	float getMass() const				{ return Mass;}
	float getInverseMass() const		{ return Inverse_Mass; }
	D3DXVECTOR3 getForce() const		{ return Force; }
	D3DXVECTOR3 getAcceleration() const	{ return Acceleration; }
	D3DXVECTOR3 getVelocity() const		{ return Velocity; }
	D3DXVECTOR3 getPosition() const		{ return Position; }

	//set values
	void setMass		(const float pMass);
	void setInverseMass	(const float pInverseMass);
	void setAcceleration(const D3DXVECTOR3 pAcceleration);
	void setVelocity	(const D3DXVECTOR3 pVelocity);
	void setPosition	(const D3DXVECTOR3 pPosition);
	void addForce		(const D3DXVECTOR3 pForce);
	void clearForce		();


	vector<D3DXVECTOR3> * getMeshVertices() {return &Mesh_Vertices;}
	
	float Width;
	float Height;
	float Depth;

protected:
	float Mass;
	float Inverse_Mass;

	D3DXVECTOR3 Force;
	
	//linear
	D3DXVECTOR3 Acceleration;
	D3DXVECTOR3 Velocity;
	D3DXVECTOR3 Position;

	//angle
	D3DXQUATERNION orientation;
	D3DXVECTOR3 rotation;

	float linearDamping;
private:
	LPD3DXMESH Mesh;

	vector<D3DXVECTOR3> Initial_Mesh_Vertices;
	/*Mesh_Vertices*/
	vector<D3DXVECTOR3> Mesh_Vertices;

	D3DXMATRIX Transform_Matrix;

};


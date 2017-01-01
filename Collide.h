#pragma once
#include "Particle.h"
#include <list>

class Face {
public:
	union {
		struct {
			unsigned int a, b, c;
		};
		struct {
			unsigned int idx[3];
		};
	};
	D3DXVECTOR3 normal;
	float distance;

	// it initialize normal and distance as ccw order of given a,b,c.
	Face(unsigned int _a, unsigned int _b, unsigned int _c,
		const D3DXVECTOR3& _normal, const float& _distance)
		: a(_a), b(_b), c(_c), normal(_normal), distance(_distance) {}
	Face(unsigned int _a, unsigned int _b, unsigned int _c,
		vector <D3DXVECTOR3> * simplex)
		: a(_a), b(_b), c(_c)
	{

		D3DXVec3Cross(&normal, &((*simplex)[b] - (*simplex)[a]), &((*simplex)[c] - (*simplex)[a]));
		D3DXVec3Normalize(&normal, &normal);

		// compute distance
		distance=D3DXVec3Dot(&normal, &((*simplex)[a]));
	}
}; typedef list<Face> FaceList;
struct Collision_Data {
	float depth;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 points[3];
};

class Collide
{
public:
	static Collide * getInstance();

	BOOL Collision_Processing(Particle * p1, Particle * p2);

	//collision detection
	BOOL Collision_Detection(Particle * p1, Particle * p2, Collision_Data * pCdata);

	//colision processing
	void Collision_Resolution(Particle * p1, Particle * p2, Collision_Data * pCdata);

	BOOL GJK(vector <D3DXVECTOR3> * pS1, vector <D3DXVECTOR3> * pS2);
	BOOL EPA(vector <D3DXVECTOR3> * pS1, vector <D3DXVECTOR3> * pS2, Collision_Data * pCdata);

	void EPA_Expand(FaceList * face, vector<FaceList::iterator> * targets);

	D3DXVECTOR3 Support(vector <D3DXVECTOR3> * pS1, D3DXVECTOR3 * d);
	D3DXVECTOR3 M_Support(vector <D3DXVECTOR3> * pS1, vector <D3DXVECTOR3> * pS2, D3DXVECTOR3 * d) {
		return Support(pS1, d) - Support(pS2, &(-(*d)));
	}
	BOOL UpdateSimplex(D3DXVECTOR3 * d);

	float fTime;
private:
	vector <D3DXVECTOR3> Simplex;

	static Collide * instance;
	Collide();
	~Collide();
};

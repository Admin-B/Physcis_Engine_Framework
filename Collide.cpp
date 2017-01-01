#include "DXUT.h"
#include "Collide.h"

#define MAX_INTERATION 20

Collide * Collide::instance = nullptr;
Collide * Collide::getInstance() {
	if (instance == nullptr) {
		instance = new Collide();
	}
	return instance;
}
Collide::Collide(){}
Collide::~Collide(){}


BOOL Collide::Collision_Processing(Particle * p1, Particle * p2) {
	BOOL result;
	Collision_Data Cdata;

	result = Collision_Detection(p1, p2, &Cdata);

	if (result) {
		printf("%f,%f,%f", Cdata.normal.x, Cdata.normal.y, Cdata.normal.z);
		Collision_Resolution(p2, p1, &Cdata);
	}
	
	Collide::getInstance()->Simplex.clear();
	return result;
}

BOOL Collide::Collision_Detection(Particle * p1, Particle * p2, Collision_Data * pCdata) {
	vector <D3DXVECTOR3> * s1 = p1->getMeshVertices();
	vector <D3DXVECTOR3> * s2 = p2->getMeshVertices();

	if (GJK(s1,s2)) {
		return EPA(s1, s2, pCdata);
	}

	return false;
}
void Collide::Collision_Resolution(Particle * p1, Particle * p2, Collision_Data * pCdata) {
	//겹쳐진 부분 처리
	if(pCdata->depth<=0){
		return;
	}
		
	float totalInverseMass = p1->getInverseMass() + p2->getInverseMass();
	if (totalInverseMass <= 0) { return; }

	D3DXVECTOR3 movePerIMass = pCdata->normal*(pCdata->depth / totalInverseMass);
	D3DXVECTOR3 particleMovement[2] = {
		movePerIMass*p1->getInverseMass(),
		movePerIMass*-p2->getInverseMass()
	};

	p1->setPosition(p1->getPosition() + particleMovement[0]);
	p2->setPosition(p2->getPosition() + particleMovement[1]);
	//분리속도 처리
	float separatingVelocity = D3DXVec3Dot(&(p1->getVelocity() - p2->getVelocity()), &(pCdata->normal));
	if (separatingVelocity > 0) {
		return;
	}
	float restitution = 0.3f; //반발계수 (완전탄성 운동일경우 1.0f)
	float newSeparatingVelocity = -separatingVelocity*restitution;

	//가속도에 의해 속도가 변할 경우, 새로이 적용되는 분리속도에서 이 가속도를 빼준다.
	float accCauseSepVelocity = D3DXVec3Dot(&(p1->getAcceleration() - p2->getAcceleration()), &pCdata->normal)*fTime;
	if (accCauseSepVelocity < 0) {
		newSeparatingVelocity += restitution*accCauseSepVelocity;
		if (newSeparatingVelocity < 0) {
			newSeparatingVelocity = 0;
		}
	}

	float deltaVelocity = newSeparatingVelocity - separatingVelocity;

	float impulse = deltaVelocity / totalInverseMass;
	D3DXVECTOR3 impulsePerIMass = pCdata->normal*impulse;

	p1->setVelocity(p1->getVelocity() + impulsePerIMass*p1->getInverseMass());
	p2->setVelocity(p2->getVelocity() + impulsePerIMass*-p2->getInverseMass());

}
BOOL Collide::GJK(vector <D3DXVECTOR3> * s1, vector <D3DXVECTOR3> * s2) {
	D3DXVECTOR3 D = D3DXVECTOR3(1.0f, 0, 0);
	
	Simplex.push_back(M_Support(s1, s2, &D));
	if (D3DXVec3Dot(&Simplex[0], &D) < 0) {
		return false;
	}

	D = -Simplex[0];
	Simplex.push_back(M_Support(s1, s2, &D));
	if (D3DXVec3Dot(&Simplex[1], &D) < 0) {
		return false;
	}
	
	D3DXVec3Cross(&D, &(Simplex[0] - Simplex[1]),&(-Simplex[1]));
	if (D.x == 0 && D.y == 0 && D.z == 0) {
		return true;
	}
	Simplex.push_back(M_Support(s1, s2, &D));

	int i;
	for (i = 0; i < MAX_INTERATION; i++) {
		D3DXVec3Cross(&D, &(Simplex[1] - Simplex[2]), &(Simplex[0] - Simplex[1]));
		float loc = D3DXVec3Dot(&(-Simplex[2]), &D);
		if (loc == 0) {
			Simplex[2] = M_Support(s1, s2, &D);
			continue;
		}
		if (loc < 0) {
			D *= -1;
		}
		break;
	}
	
	D3DXVECTOR3 temp;
	for (i = 0; i < MAX_INTERATION; i++) {
		temp = M_Support(s1, s2, &D);
		Simplex.push_back(temp);
		if (D3DXVec3Dot(&temp, &D) < 0) {
			return false;
		}
		if (UpdateSimplex(&D)) {
			return true;
		}
	}
	return false;
}
BOOL Collide::EPA(vector <D3DXVECTOR3> * s1, vector <D3DXVECTOR3> * s2 , Collision_Data * pCdata) {
		if (Simplex.size() != 4) {
		return false;
	}
	FaceList faces;
	{
		const D3DXVECTOR3& A = Simplex[3];
		const D3DXVECTOR3& B = Simplex[2];
		const D3DXVECTOR3& C = Simplex[1];
		const D3DXVECTOR3& D = Simplex[0];
		D3DXVECTOR3 BCD, T;
		D3DXVec3Cross(&BCD, &(C - B), &(D - B));

		float loc = D3DXVec3Dot(&(A - B), &BCD);
		if (loc > 0) {
			BCD = -BCD;
			D3DXVec3Normalize(&BCD, &BCD);
			float dist_BCD = D3DXVec3Dot(&BCD, &B);
			faces.push_back(Face(3, 2, 1, &Simplex));
			faces.push_back(Face(3, 1, 0, &Simplex));
			faces.push_back(Face(3, 0, 2, &Simplex));
			faces.push_back(Face(2, 0, 1, BCD, dist_BCD));
		}
		else if (loc < 0) {
			D3DXVec3Normalize(&BCD,&BCD);
			float dist_BCD = D3DXVec3Dot(&BCD,&B);
			
			faces.push_back(Face(3, 1, 2, &Simplex));
			faces.push_back(Face(3, 0, 1, &Simplex));
			faces.push_back(Face(3, 2, 0, &Simplex));
			faces.push_back(Face(2, 1, 0, BCD, dist_BCD));
		}
		else {
			return false;
		}
	}
	FaceList::iterator itr;
	FaceList::iterator closest;
	FaceList::iterator end = faces.end();
	for (int i = 0; i < MAX_INTERATION; i++) {
		itr = faces.begin();
		closest = itr;
		for (; itr != end; itr++) {
			if (itr->distance < closest->distance) {
				closest = itr;
			}
		}
		D3DXVECTOR3 P = M_Support(s1,s2,&closest->normal);

		float d = D3DXVec3Dot(&P,&closest->normal);
		if (d - closest->distance < 0.00001f) {
			// closest face is really closest and we're done.
			pCdata->normal = closest->normal;
			pCdata->depth = closest->distance;
			return true;
		}

		// add point and make sure that face list to be convex.
		// note that added point p can't be placed on the any tip of voronoi regions
		// because the points of simplex are already farthest points of
		// minkowski set.
		Simplex.push_back(P);
		// find visible faces
		vector<FaceList::iterator> targets;
		for (itr = faces.begin(); itr != end; itr++) {
			if (D3DXVec3Dot(&(P-Simplex[itr->a]),&(itr->normal)) > 0) {
				targets.push_back(itr);
			}
		}
		// expand polytope
		EPA_Expand(&faces, &targets);
	}
	return false;
}
void Collide::EPA_Expand(FaceList * faces, vector<FaceList::iterator> * targets) {
	unsigned int pidx = Simplex.size() - 1;
	unsigned int szTarget = targets->size();
	if (szTarget == 0) {

	}
	else if (szTarget == 1) {
		// point is in a certain face side of voronoi regions.
		// there are 3 faces should be added and a face should be removed.
		faces->push_back(Face(pidx, (*targets)[0]->a, (*targets)[0]->b, &Simplex));
		faces->push_back(Face(pidx, (*targets)[0]->b, (*targets)[0]->c, &Simplex));
		faces->push_back(Face(pidx, (*targets)[0]->c, (*targets)[0]->a, &Simplex));
		faces->erase((*targets)[0]);
	}
	else if (szTarget == 2) {
		// point is in a certain edge side of voronoi regions.
		// there are 4 faces should be added and two faces should be removed.
		// find unique and one way ordered indices of two target faces.
		unsigned int uniqIdxs[4];
		for (unsigned int i = 0; i < 3; i++) {
			unsigned int j = 0;
			for (; j < 3; j++) {
				if ((*targets)[0]->idx[i] == (*targets)[1]->idx[j])
					break;
			}
			if (j != 3) {
				// it can be guaranteed that the target faces are orderd by ccw.
				// (they were sorted when compute those face normals)
				// so, (i+1)---(j+2) or (i+2)---(j+1) is the common edge.
				if ((*targets)[0]->idx[(i + 1) % 3] == (*targets)[1]->idx[(j + 2) % 3]) {
					// (i)--(j+1)--(i+1)--(i+2) order
					uniqIdxs[0] = (*targets)[0]->idx[i];
					uniqIdxs[1] = (*targets)[1]->idx[(j + 1) % 3];
					uniqIdxs[2] = (*targets)[0]->idx[(i + 1) % 3];
					uniqIdxs[3] = (*targets)[0]->idx[(i + 2) % 3];
					break;
				}
				else { //if (i+2)---(j+1)
					   // (i)--(i+1)--(i+2)--(j+2)
					uniqIdxs[0] = (*targets)[0]->idx[i];
					uniqIdxs[1] = (*targets)[0]->idx[(i + 1) % 3];
					uniqIdxs[2] = (*targets)[0]->idx[(i + 2) % 3];
					uniqIdxs[3] = (*targets)[1]->idx[(j + 2) % 3];
					break;
				}
			}
		}

		faces->push_back(Face(pidx, uniqIdxs[0], uniqIdxs[1], &Simplex));
		faces->push_back(Face(pidx, uniqIdxs[1], uniqIdxs[2], &Simplex));
		faces->push_back(Face(pidx, uniqIdxs[2], uniqIdxs[3], &Simplex));
		faces->push_back(Face(pidx, uniqIdxs[3], uniqIdxs[0], &Simplex));
		faces->erase((*targets)[0]);
		faces->erase((*targets)[1]);
	}
	else {
		// unique index list
		std::vector<unsigned int> uniqIdxs;

		// 1. push indices of targets[0] into idxs.
		uniqIdxs.push_back((*targets)[0]->a);
		uniqIdxs.push_back((*targets)[0]->b);
		uniqIdxs.push_back((*targets)[0]->c);
		unsigned int szIdxs = uniqIdxs.size();

		// 2. gather the indices from targets uniquely.
		unsigned int i, j, k;
		for (i = 1; i < szTarget; i++) {
			// push indices from target[i] into idxs uniquely.
			for (j = 0; j < 3; j++) {
				bool unique = true;
				for (k = 0; k < szIdxs; k++) {
					if ((*targets)[i]->idx[j] == uniqIdxs[k]) {
						unique = false;
						break;
					}
				}
				if (unique) {
					uniqIdxs.push_back((*targets)[i]->idx[j]);
					szIdxs++;
				}
			}
		}

		// 3. compute the center point of unique vertices.
		D3DXVECTOR3 CP(0,0,0);
		for (i = 0; i < szIdxs; i++)
			CP += Simplex[uniqIdxs[i]];
		CP /= (float)szIdxs;

		// 4. project unique vertices into a certain plane.
		// at this point, i choose the plane which has CP-P as plane normal
		// and is passing the origin.
		D3DXVECTOR3 norm = Simplex[pidx] - CP;
		D3DXVec3Normalize(&norm, &norm);

		D3DXVECTOR3 projCP = CP - D3DXVec3Dot(&CP,&norm)*norm;
		std::vector<D3DXVECTOR3> projPoints(szIdxs);
		for (i = 0; i < szIdxs; i++) {
			const D3DXVECTOR3& Q = Simplex[uniqIdxs[i]];
			projPoints[i] = Q - D3DXVec3Dot(&Q,&norm)*norm;
		}

		// 5. compute angles of CP-projPoints axis as plane normal.
		std::vector<float> angles(szIdxs);
		D3DXVECTOR3 standAxis = projPoints[0] - projCP;
		D3DXVec3Normalize(&standAxis, &standAxis);
		angles[0] = 0;
		for (i = 1; i < szIdxs; i++) {
			D3DXVECTOR3 line;
			D3DXVec3Normalize(&line, &(projPoints[i] - projCP));
			D3DXVECTOR3 perp;
			D3DXVec3Cross(&perp,&standAxis,&line);
			angles[i] = acos(D3DXVec3Dot(&standAxis,&line));
			// if sin() <0 then angle is over PI
			if (D3DXVec3Dot(&perp,&norm) < 0) {
				angles[i] = 3.14159265358979323846f*2.f - angles[i];
			}
		}

		// 6. sort angles with indices ordered by angles ascent.
		// algorithm from wikipedia.
		float tmp;
		for (i = 1; i<szIdxs; i++) {
			j = i;
			tmp = angles[j];
			k = uniqIdxs[j];

			while (--j >= 0 && tmp < angles[j]) {
				angles[j + 1] = angles[j];
				uniqIdxs[j + 1] = uniqIdxs[j];
			}
			angles[j + 1] = tmp;
			uniqIdxs[j + 1] = k;
		}

		// 7. add and remove faces
		szIdxs -= 1;
		for (i = 0; i < szIdxs;) {
			faces->push_back(Face(pidx, uniqIdxs[i++], uniqIdxs[i], &Simplex));
		}
		faces->push_back(Face(pidx, uniqIdxs[szIdxs], uniqIdxs[0], &Simplex));

		for (i = 0; i < szTarget; i++) {
			faces->erase((*targets)[i]);
		}
	}
}
D3DXVECTOR3 Collide::Support(vector <D3DXVECTOR3> * pS1, D3DXVECTOR3 * d) {
	float max = -999999999999.0f, temp;
	int index = 0 , s=pS1->size();
	for (int i = 0; i < s; i++) {
		temp = D3DXVec3Dot(&(*pS1)[i], d);
		if (max < temp) {
			max = temp;
			index = i;
		}
	}
	return (*pS1)[index];
}
BOOL Collide::UpdateSimplex(D3DXVECTOR3 * d) {
	D3DXVECTOR3 V10 = -Simplex[3],
		V12 = Simplex[2] - Simplex[3],
		V13 = Simplex[1] - Simplex[3],
		V14 = Simplex[0] - Simplex[3];
	D3DXVECTOR3 T;
	D3DXVec3Cross(&T, &(Simplex[1] - Simplex[2]), &(Simplex[0] - Simplex[2]));
	if (D3DXVec3Dot(&T, d) > 0) {
		D3DXVec3Cross(d,&V12, &V13);
		if (D3DXVec3Dot(&V10, d) > 0) {
			Simplex.erase(Simplex.begin());
		}
		else {
			D3DXVec3Cross(d, &V14, &V12);
			if (D3DXVec3Dot(&V10, d) > 0) {
				Simplex.erase(Simplex.begin() + 1);
			}
			else {
				D3DXVec3Cross(d, &V13, &V14);
				if (D3DXVec3Dot(&V10, d) > 0) {
					Simplex.erase(Simplex.begin() + 2);
				}
				else {
					return true;
				}
			}
		}
	}
	else {
		D3DXVec3Cross(d, &V13, &V12);
		if (D3DXVec3Dot(&V10, d) > 0) {
			Simplex.erase(Simplex.begin());
		}
		else {
			D3DXVec3Cross(d, &V12, &V14);
			if (D3DXVec3Dot(&V10, d) > 0) {
				Simplex.erase(Simplex.begin() + 1);
			}
			else {
				D3DXVec3Cross(d, &V14, &V13);
				if (D3DXVec3Dot(&V10, d) > 0) {
					Simplex.erase(Simplex.begin() + 2);
				}
				else {
					return true;
				}
			}
		}
	}
	return false;
}
/***************************************************************************
                                    Mesh.h
                             -------------------
    update               : 2002-10-11
    copyright            : (C) 2002 by Micha�l ROY
    email                : michaelroy@users.sourceforge.net

    Edit Yohan Fougerolle
    Warning : handle only triangular faces!

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _NEIGHBORMESH_
#define _NEIGHBORMESH_

#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace Eigen;

#include <cassert>
#include <cstring>
#include <vector>
#include <set>
#include <map>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <GL/glut.h>
#include <algorithm>
#include "useful.h"
#include <iostream>
#include <cmath>

using namespace std;

#include "Mesh.h"

class NeighborMesh : public Mesh
{
public:
    //constructor and destructor
    NeighborMesh();
    virtual ~NeighborMesh();

    //returns the edge from the given indexes i1 and i2
    inline map < pair <int,int>,  set<int> > :: iterator Get_Edge(int i1, int i2)
    {
        pair <int, int> mypair;
        if (i1<i2) {mypair.first = i1; mypair.second = i2; return Edges.find(mypair);}
        else {mypair.first = i2; mypair.second = i1; return Edges.find(mypair);}
    }

    //construction of the previous attributes once the file is loaded
    bool Build_P2P_Neigh();
    bool Build_P2F_Neigh();
    bool Build_F2F_Neigh();
    bool Build_Edges();
    void BuildDistanceLabels(int A);

    vector<int> ShortestPath(int, int, bool buildlabels=false);
    void SetColorsFromLabels();
    void SetColorsFromKean(double n = 5);

    //compute extended neighborhoods
    set<int> GetP2P_Neigh( int, int );
    set<int> GetF2F_Neigh( int, int );

    //check if the triangle face is obtuse
    int IsObtuse( int face_index);

    int getP2P_NeighSize();
    int getP2F_NeighSize();

    int getF2F_NeighSize();
    set<int> getP2P_NeighSet(int i);
    set<int> getP2F_NeighSet(int i);
    set<int> getF2F_NeighSet(int i);
    vector< set<int> > getP2P_Neigh();
    vector< set<int> > getP2F_Neigh();
    vector< set<int> > getF2F_Neigh();

    map < pair <int,int>,  set<int> > getEdges() {return Edges;}

protected:
    //attributes
    vector < set<int> > P2P_Neigh;
    vector < set<int> > P2F_Neigh;
    vector < set<int> > F2F_Neigh;

    vector<double> Labels;
    map < pair <int,int>,  set<int> > Edges;

};

#endif // _NEIGHBORMESH_

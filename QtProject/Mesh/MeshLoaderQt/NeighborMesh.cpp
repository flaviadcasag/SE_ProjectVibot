/***************************************************************************
                                  NeighborMesh.cpp
                             -------------------
    copyright            : (C) 2010 by yohan FOUGEROLLE
    email                : yohan.fougerolle@u-bourgogne.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "NeighborMesh.h"
#include <iostream>

//constructor and destructor

NeighborMesh :: NeighborMesh()
{

}
NeighborMesh :: ~NeighborMesh(){}


// construction of the various neighborhoods
bool NeighborMesh :: Build_P2P_Neigh()
{
    if ( vertices.empty() || faces.empty() ) return false;

    //first build an array of empty sets

    for( int i=0; i<vertices.size(); i++)
    {
        set <int> myset;
        P2P_Neigh.push_back(myset);
    }

    // now span all the faces, and adds the points within the corresponding sets

    for( int i=0; i<faces.size(); i++)
    {
        Vector3i F(faces[i]);

        P2P_Neigh[ F[0] ].insert(F[1]);
        P2P_Neigh[ F[0] ].insert(F[2]);

        P2P_Neigh[ F[1] ].insert(F[0]);
        P2P_Neigh[ F[1] ].insert(F[2]);

        P2P_Neigh[ F[2] ].insert(F[0]);
        P2P_Neigh[ F[2] ].insert(F[1]);
    }

    return true;
}

//build P2f nigh
bool NeighborMesh :: Build_P2F_Neigh()
{
    if ( vertices.empty() || faces.empty() ) return false;

    //first build an array of empty sets

    for( int i=0; i<vertices.size(); i++)
    {
        set <int> myset;
        P2F_Neigh.push_back(myset);
    }

    // now span all the faces, and adds the faces within the corresponding sets

    for( int i=0; i<faces.size(); i++)
    {
        Vector3i F(faces[i]);
        for( int j=0; j<3; j++) P2F_Neigh[F[j]].insert(i);
    }
    return true;
}

//build f2f nigh
bool NeighborMesh :: Build_F2F_Neigh()
{
    if ( vertices.empty() || faces.empty() || P2F_Neigh.empty()) return false;

    for (int i=0; i<faces.size(); i++)
    {
        //get the face
        Vector3i F(faces[i]);
        set<int> myset;

        //for all the points, get the faces neighbors
        for( int j=0; j<3; j++)
        {
            set_union(

                P2F_Neigh[F[j]].begin(), P2F_Neigh[F[j]].end(),
                myset.begin(), myset.end(),
                insert_iterator< set<int> >(myset,myset.begin())
                );
        }

        //remove the current face
        myset.erase(i);

        //then store in the F2F array
        F2F_Neigh.push_back(myset);
    }

    return true;
}

//get p2p neigh
set<int> NeighborMesh :: GetP2P_Neigh( int p_index, int n)
{

    set <int> previous; previous.insert(p_index);
    set <int> new_ring = P2P_Neigh[p_index];

    if ( n == 0) return previous;
    if ( n == 1) return new_ring;

    set < int > myset;

    for(int i = 1; i<n; i++)
    {
        // compute the 1 neighbourhood of the previously computed ring

        myset.clear();

        for ( set <int> :: iterator it(new_ring.begin()); it != new_ring.end(); it++)
        {
            for (set<int> :: iterator it2(P2P_Neigh[*it].begin()); it2 != P2P_Neigh[*it].end(); it2++)
            {
            myset.insert(*it2);
            }
        }

        set <int> dum; //seems uneasy to remove elements while updating the set at the same time ==> dum set for performing the boolean difference

        //extract previous from my set
        set_difference( myset.begin(), myset.end(),
                        previous.begin(), previous.end(),
                        insert_iterator< set<int> >(dum,dum.begin())
                        );
        myset = dum; // copy dum result into myset... I whish I could have avoided this

        //previous = myset INTERSECTED with new ring
        previous.clear();
        set_intersection( myset.begin(), myset.end(),
                        new_ring.begin(), new_ring.end(),
                        insert_iterator< set<int> >(previous,previous.begin())
                );

        //new_ring = myset MINUS previous
        new_ring .clear();
        set_difference( myset.begin(), myset.end(),
                        previous.begin(), previous.end(),
                        insert_iterator< set<int> >(new_ring,new_ring.begin())
                        );

    }

    return new_ring;
}

//get f2f neigh
set<int> NeighborMesh :: GetF2F_Neigh( int f_index, int n)
{
    //same principle as for points, except that we are working with faces
    set <int> previous; previous.insert(f_index);
    set <int> new_ring = F2F_Neigh[f_index];

    if ( n == 0) return previous;
    if ( n == 1) return new_ring;

    set < int > myset;

    for(int i = 1; i<n; i++)
    {
        // compute the 1 neighbourhood of the previously computed ring

        myset.clear();

        for ( set <int> :: iterator it(new_ring.begin()); it != new_ring.end(); it++)
        {
            for (set<int> :: iterator it2(F2F_Neigh[*it].begin()); it2 != F2F_Neigh[*it].end(); it2++)
            {
            myset.insert(*it2);
            }
        }
        set <int> dum; //seems uneasy to remove elements while updating the set at the same time ==> dum set for performing the boolean difference

        //extract previous from my set
        set_difference( myset.begin(), myset.end(),
                        previous.begin(), previous.end(),
                        insert_iterator< set<int> >(dum,dum.begin())
                        );
        myset = dum;


        //previous = myset INTER new ring
        previous.clear();
        set_intersection( myset.begin(), myset.end(),
                        new_ring.begin(), new_ring.end(),
                        insert_iterator< set<int> >(previous,previous.begin())
                );

        //new_ring = myset MINUS previous
        new_ring .clear();
        set_difference( myset.begin(), myset.end(),
                        previous.begin(), previous.end(),
                        insert_iterator< set<int> >(new_ring,new_ring.begin())
                        );
    }


    return new_ring;
}


bool NeighborMesh :: Build_Edges()
{
    if (P2F_Neigh.empty() || faces.empty() || vertices.empty() ) return false;

    for(int i=0; i<faces.size(); i++)   // for all the faces
    {
        Vector3i F(faces[i]);           // get the face

        for( int j=0; j<3; j++)         // for all the points
        {
            //get index of extremities
            int i1 = F[j];
            int i2 = F[(j+1)%3];

            //sort to guarantee that a same edge won't be represented twice when spanning adjacent face
            if( i1 > i2 ) {int dum(i1); i1=i2; i2=dum;}

            //intersection of both sets of surrounding faces of both extremities
            set <int> myset;

            set_intersection(
                P2F_Neigh[i1].begin(), P2F_Neigh[i1].end(),
                P2F_Neigh[i2].begin(), P2F_Neigh[i2].end(),
                insert_iterator< set<int> >(myset,myset.begin())
                );

            // construct the key as a pair of point index
            pair<int,int> myedge(i1,i2);

            //construct the pair (key, set) and insert into the map
            Edges.insert( pair < pair<int,int>,set<int> > (myedge, myset));
        }
    }
    return true;

}

void NeighborMesh ::BuildDistanceLabels(int A)
{
    //first build an array to store the minimal distances to reach point i from A

    Labels.clear();
    for(int i=0; i<vertices.size();i++) Labels.push_back(1e30); // set dummy labels
    Labels[A] = 0; //initialize the distance to 0 for the starting point

    // compute the minimal distance for advancing front
    set<int> advancingfront=P2P_Neigh[A];
    set<int>::iterator it1,it2;
    for(it1 = advancingfront.begin(); it1!=advancingfront.end(); it1++)
        {
            Labels[*it1] = (vertices[A] - vertices[*it1] ).norm();
        }

    bool ok=true; double dist;
    set<int> dumset;
    while(ok)
    {   ok = false; // suppose algo should stop
        //compute the new points to be evaluated
        dumset = advancingfront;
        advancingfront.clear();

        //compute and update the distance from existing points to advancing front
        //add the P2P neigh, if required, of the advancing front to dumset
        for(it1 = dumset.begin(); it1!=dumset.end(); it1++)
        {
            for(it2=P2P_Neigh[*it1].begin(); it2 != P2P_Neigh[*it1].end(); it2++)
            {
            //compute the distance from it1 to it2. If inferior to D[it2], then update and insert
            dist = (vertices[*it1] - vertices[*it2]).norm();
            if(Labels[*it2] > dist + Labels[*it1]) // this point can be reached with a shorter path
                {
                Labels[*it2] = dist + Labels[*it1];
                advancingfront.insert(*it2);
                ok = true; // new path added, should continue working
                }
            }
        }
    }
}

//compute shortest path
vector<int> NeighborMesh :: ShortestPath(int A, int B,bool buildlabels)
{
    vector<int> ShortestPath; // set of traversed points

    if(buildlabels)  BuildDistanceLabels(A);

    //now unfold the path from the labels

    ShortestPath.push_back(B);
    int currentpoint = B;

    set<int>::iterator it1;
    // almost always works, except on the infamous dragon : enters an infinite loop
    // actually, using a "while" approach combined with an arithmetic test (if d = d1+d2)
    // is not the most robust way to unfold the shortest path...

    while(currentpoint != A) // while start not reached
    {
        //span the neigh of current point and seek for the previous point
        for(it1 = P2P_Neigh[currentpoint].begin(); it1!=P2P_Neigh[currentpoint].end(); it1++)
        {
            if(
                fabs(
                    (vertices[currentpoint] - vertices[*it1] ).norm() + Labels[*it1]
                        -
                     Labels[currentpoint]
                     ) <1e-12 )
            // avoid if d1 == d2 and replace by if (fabs(d1-d2)<1e-12) to avoid numerical accuracy issues
            {
                currentpoint=*it1;
                ShortestPath.push_back(*it1);
                break;
            }
        }
    }

    return ShortestPath;
}

//set colors from labels
void NeighborMesh :: SetColorsFromLabels()
{
    // brute force approach assuming that values are uniformly distributed (no outlier)
    if (Labels.empty()) return;

    // get min and max labels
    double Lmin(Labels[0]),Lmax(Labels[0]);
    for(int i=1; i<Labels.size();i++)
    {
        Lmin = min(Lmin, Labels[i]);
        Lmax = max(Lmax, Labels[i]);
    }

    colors.clear();
    //translate values into [0,1], then convert into color, and store
    for(int i=0; i<Labels.size();i++)
    {
        colors.push_back( DoubleToColor ( (Labels[i] - Lmin )/(Lmax-Lmin) ) );
    }
}



// returns the index of a vertex of a face for which the associated angle is obtuse
// returns -1 if the triangle is not obtuse
int NeighborMesh :: IsObtuse(int f_index)
{
    assert ( f_index >=0 && f_index<faces.size() );
    Vector3i F(faces[f_index]);

    for (int i=0; i<3; i++)
    {
        Vector3d V1(vertices[F[(i+1)%3]] - vertices[F[i]]);
        Vector3d V2(vertices[F[(i+2)%3]] - vertices[F[i]]);
        if( V1.dot(V2) < 0 )     return F[i];
    }

    return -1;
}

//get p2p neigh size
int NeighborMesh::getP2P_NeighSize()
{
    return P2P_Neigh.size();
}

//get p2f neigh size
int NeighborMesh::getP2F_NeighSize()
{
    return P2F_Neigh.size();
}

//get f2f neigh size
int NeighborMesh::getF2F_NeighSize()
{
    return F2F_Neigh.size();
}

//get p2p negh set according to the point index given
set<int> NeighborMesh::getP2P_NeighSet(int i)
{
    return P2P_Neigh[i];
}
//get p2f negh set according to the point index given
set<int> NeighborMesh::getP2F_NeighSet(int i)
{
    return P2F_Neigh[i];
}

//get f2f negh set according to the face index given
set<int> NeighborMesh::getF2F_NeighSet(int i)
{
    return F2F_Neigh[i];
}

//get p2p vector
vector<set<int> > NeighborMesh::getP2P_Neigh()
{
    return P2P_Neigh;
}
//get p2f vector
vector<set<int> > NeighborMesh::getP2F_Neigh()
{
    return P2F_Neigh;
}

//get f2f vector
vector<set<int> > NeighborMesh::getF2F_Neigh()
{
    return F2F_Neigh;
}

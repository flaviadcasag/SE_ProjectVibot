#include "LaplacianMesh.h"

//Default Constructor
LaplacianMesh::LaplacianMesh()
{
}

//Compute Laplacian Matrix
void LaplacianMesh :: laplacian()
{
    //Generates the A and D matrices
    set<int>::iterator it;
    int N=P2P_Neigh.size();
    MatrixXd A;
    MatrixXd D;
    A=MatrixXd::Zero(N,N);
    D=MatrixXd::Zero(N,N);

    //variable that stores the current P2P set
    set<int> neighp;

    //Iterates over all P2P
    for(int i=0;i<P2P_Neigh.size(); i++)
    {
        neighp = P2P_Neigh.at(i);
        for(it=neighp.begin(); it != neighp.end();it++)
        {
            //Putting all weight equal to 1
            A(i,(*it))=1;
        }
        //Put the number of neighbors in the D
        D(i,i)=neighp.size();
    }
    laplacianMatrix = D-A;
}

//Compute the cotangent
double LaplacianMesh::cot(double i)
{
    return(1 / tan(i));
}

//Compute the weight for the D matrix of the laplacian
MatrixXd LaplacianMesh :: computeWeight()
{
    //Initializes the W matrix
    int N = vertices.size();
    MatrixXd W = MatrixXd::Zero(N,N);
    int face[2];
    set<int> intersect;
    set<int>::iterator it;
    set<int>::iterator it2;

    //iterate for all vertices
    for (int i = 0; i < N; i++)
    {
        //get neighbors from the current vertex
        set<int> currentP2P = P2P_Neigh[i];

        //iterates for all neighbors
        for (it = currentP2P.begin(); it != currentP2P.end(); it++)
        {
            //get the current neighbor
            int j = (*it);

            //Get the faces intersection
            set<int> faces1 = P2F_Neigh[i];
            set<int> faces2 = P2F_Neigh[j];
            intersect.clear();
            set_intersection(faces1.begin(),faces1.end(),faces2.begin(),faces2.end(),
                              std::inserter(intersect,intersect.begin()));

            //if it intersects different than 2 faces, it puts the W = 0
            if (intersect.size() != 2)
            {
                W(i,j) = 0;
            }
            else
            {
                //put the faces index into variable face[]
                int cnt = 0;
                for (it2 = intersect.begin(); it2 != intersect.end(); ++it2)
                {
                    face[cnt++] = (*it2);
                }
                //gets the 2 correspondents vertices to compute alpha and beta
                int u = 0;
                int v = 0;
                for (int l = 0; l < 3; l++)
                {
                    if (faces[face[0]][l] != i && faces[face[0]][l] != j)
                    {
                        u = faces[face[0]][l];
                    }
                }
                for (int l = 0; l < 3; l++)
                {
                    if (faces[face[1]][l] != i && faces[face[1]][l] != j)
                    {
                        v = faces[face[1]][l];
                    }
                }

                //compute the vectors in order to compute the angles
                Vector3d vec1 = vertices[u] - vertices[i];
                Vector3d vec2 = vertices[u] - vertices[j];
                Vector3d vec3 = vertices[v] - vertices[i];
                Vector3d vec4 = vertices[v] - vertices[j];

                //compute alpha and beta
                double alpha = acos(vec1.dot(vec2)/(vec1.norm() * vec2.norm()));
                double beta = acos(vec3.dot(vec4)/(vec3.norm() * vec4.norm()));

                //Store the weight
                W(i,j) = cot(alpha) + cot(beta);
            }
        }
    }
    return W;
}

//Compute the Weignted laplacian
void LaplacianMesh :: weightedLaplacian()
{
    //Compute weight
    MatrixXd A = computeWeight();
    int N = P2P_Neigh.size();
    MatrixXd D;
    D=MatrixXd::Zero(N,N);

    //Iterates for all P2P_neigh
    set<int>::iterator it;
    for(int i=0;i<P2P_Neigh.size(); i++)
    {
        //Sum all the weights to compute D matrix
        D(i,i)=A.row(i).sum();
    }
    laplacianMatrix = D-A;
}

//Compute eigen values and
void LaplacianMesh :: spectralDecomposition()
{
    //find the eigen values and eiven vectors and store them
    SelfAdjointEigenSolver<MatrixXd> e(laplacianMatrix);
    eValues = e.eigenvalues();
    eVectors = e.eigenvectors();
}

//Smooths the mesh
//the the eiven vectors equal to zero from the given frequency
void LaplacianMesh :: smoothing(int frequency)
{
    //copy all the original vertices into the Eigen matrix structure
    MatrixXd verticesMat(originalVertices.size(),3);
    MatrixXd delta;
    for (int i = 0; i < originalVertices.size();i++)
    {
        for (int j = 0; j < 3;j++)
        {
            verticesMat(i,j)=originalVertices[i][j];
        }
    }

    //Compute the delta coordinates
    delta=eVectors.inverse()*verticesMat;

    //put the delta equal to 0 from the selected frequency
    int init = frequency;
    for (int i = init; i < delta.rows();i++)
    {
        for (int j = 0; j < delta.cols();j++)
        {
            delta(i,j) = 0;
        }
    }

    //compute the smoothed vertices
    MatrixXd smooth = eVectors*delta;

    //put the smoothed vertices into the vertices to be drawn
    for (int i = 0; i < vertices.size();i++)
    {
        for (int j = 0; j < 3;j++)
        {
            vertices[i][j] = smooth(i,j);
        }
    }

    //put the colors
    for (int i = 0; i < colors.size();i++)
    {
       colors[i]= globalColor;
    }
}

//Removes one specific frequency
void LaplacianMesh::frequencyRemoval(int frequency)
{
    //convert the vertices into the Matrix eigen structure
    MatrixXd verticesMat(originalVertices.size(),3);
    MatrixXd delta;
    for (int i = 0; i < originalVertices.size();i++)
    {
        for (int j = 0; j < 3;j++)
        {
            verticesMat(i,j)=originalVertices[i][j];
        }
    }

    //compute the delta coordinates
    delta=eVectors.inverse()*verticesMat;

    //set the given frequency to be 0
    for (int j = 0; j < delta.cols();j++)
    {
        delta(frequency,j) = 0;
    }

    //compute the real vertices
    MatrixXd removal = eVectors*delta;

    //put the computed values in the vertices
    for (int i = 0; i < vertices.size();i++)
    {
        for (int j = 0; j < 3;j++)
        {
            vertices[i][j] = removal(i,j);
        }
    }

    //paints the mesh with the global color
    for (int i = 0; i < colors.size();i++)
    {
       colors[i] = globalColor;
    }
}

//Edit the mesh given the axis and size
void LaplacianMesh::meshEditing(double rate, int axis)
{
    //set axis to be used
    Vector3d sizes(1,1,1);

    double size = vertices[maxs[axis]][axis] - vertices[mins[axis]][axis];

    sizes[axis] = size*rate;

    //get the maximum and minimum XYZ indexes
    int getMin = 0;
    int getMax = 0;
    getMax = maxs[axis];
    getMin = mins[axis];

    //convert the vertices into the eigen Matrix structure
    MatrixXd verticesMat(originalVertices.size(),3);
    MatrixXd delta;
    for (int i = 0; i < originalVertices.size();i++)
    {
        for (int j = 0; j < 3; j++)
        {
            verticesMat(i,j)=originalVertices[i][j];
        }
    }

    //compute the delta coodinates
    delta=laplacianMatrix*verticesMat;

    //add more 2 rows in the laplacianMatrix and add the 2 points to be anchor
    MatrixXd laplacianAnchors(laplacianMatrix.rows()+2, laplacianMatrix.cols());
    MatrixXd B;
    B = MatrixXd::Zero(2,laplacianMatrix.cols());
    B(0,getMin) = 1;
    B(1,getMax) = 1;
    laplacianAnchors << laplacianMatrix, B;

    //add more 2 rows in the delta coordinates
    MatrixXd deltaAnchors(delta.rows()+2, delta.cols());
    MatrixXd C(2,3);

    //fix the first point
    C(0,0) = verticesMat(getMin,0);
    C(0,1) = verticesMat(getMin,1);
    C(0,2) = verticesMat(getMin,2);

    //move the second point according to the parameter given
    C(1,0) = verticesMat(getMax,0)+sizes[0];
    C(1,1) = verticesMat(getMax,1)+sizes[1];
    C(1,2) = verticesMat(getMax,2)+sizes[2];

    deltaAnchors << delta,C;

    //compute the newvertices using the least squares method
    MatrixXd newVertices = (laplacianAnchors.transpose()*laplacianAnchors).inverse()*laplacianAnchors.transpose()*deltaAnchors;

    //put the vertices computed in vertices list
    for (int i = 0; i < vertices.size();i++)
    {
        for (int j = 0; j < 3;j++)
        {
            vertices[i][j] = newVertices(i,j);
        }
    }
}

void LaplacianMesh::setLaplacianMatrix(MatrixXd laplacianMatrix)
{
    this->laplacianMatrix = laplacianMatrix;
}
void LaplacianMesh ::BuildSpectralLabels(int i)
{
    for(int j=0;j<eVectors.rows();j++){
        Labels[j]=eVectors(j,i);
    }
}

#ifndef LAPLACIAN_H
#define LAPLACIAN_H

#include "NeighborMesh.h"

class LaplacianMesh : public NeighborMesh
{
public:
    //Default constructor
    LaplacianMesh();

    //Compute laplacian matrix
    void laplacian();

    //Compute weighted laplacian
    void weightedLaplacian();

    //Smooths the mesh according to the frequency
    //It puts all the eigenvectors equal to zero from the frequency given
    void smoothing(int frequency);

    //remove only one frequency of the mesh
    void frequencyRemoval(int frequency);

    //Computes the weight for the weightedLaplacian implementation
    MatrixXd computeWeight();

    //Mesh editting according to the size and the axis
    //0 = x, 1 = y, 2 = z
    //it uses the extreme points of the mesh (lowest and highest points)
    //as anchors, and move one of them to strech to mesh
    void meshEditing(double rate, int axis);

    //Calculate the spectral decomposition
    void spectralDecomposition();

    //Set laplacian Matrix
    void setLaplacianMatrix(MatrixXd laplacianMatrix);

    //Get EigenVectors
    MatrixXd getEigenVectors() {return eVectors;}

    //Build spectral labels
    void BuildSpectralLabels(int i);

private:
    //Computes the cotangent
    double cot(double i);

    //Eigen values
    MatrixXd eValues;

    //Eigen Vectors
    MatrixXd eVectors;

    //Laplacian Matrix
    MatrixXd laplacianMatrix;

};

#endif // LAPLACIAN_H

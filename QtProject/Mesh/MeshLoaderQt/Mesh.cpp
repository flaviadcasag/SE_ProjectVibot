/***************************************************************************
                                  Mesh.cpp
                             -------------------
    update               : 2002-12-05
    copyright            : (C) 2002 by Michaël ROY
    email                : michaelroy@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <fstream>
#include <iostream>
#include <GL/glut.h>

#include "Mesh.h"
#include "constante.h"
#include "useful.h"
using namespace std;

Mesh::Mesh()
{
    globalColor = Vector3d(0,1,0);
}

Mesh::~Mesh(){}

//--
//
// ClearAll
//
//--
void Mesh::ClearAll()
{
	// Initialize mesh
	// Clear all data
	ClearVertices();
	ClearFaces();
	ClearColors();
	ClearTextures();
	ClearFaceNormals();
	ClearVertexNormals();
	ClearTextureName();
}

//--
//
// ComputeFaceNormals
//
//--
// Compute unit normal of every faces
void Mesh::ComputeFaceNormals()
{
	// Resize face normal array
	face_normals.resize( FaceNumber() );

	// For every face
	int MaxFaceNumber = FaceNumber();
	for( int i=0; i<MaxFaceNumber; i++ )
	{
		// Compute unit face normal
		//FaceNormal(i) = ComputeFaceNormal(i);
		face_normals[i] = ComputeFaceNormal(i);
	}
}

//--
//
// ComputeVertexNormals
//
//--
// Compute unit normal of every vertex
void Mesh::ComputeVertexNormals()
{

	int i;

	// Assume that face normals are computed
	assert( FaceNormalNumber() == FaceNumber() );

	// Resize and initialize vertex normal array
	vertex_normals.assign( VertexNumber(), Vector3d(0,0,0) );

	// For every face
	for( i=0 ; i<FaceNumber() ; i++ )
	{
		// Add face normal to vertex normal
		VertexNormal(i,0) += FaceNormal(i);
		VertexNormal(i,1) += FaceNormal(i);
		VertexNormal(i,2) += FaceNormal(i);
	}

	// For every vertex
	for( i=0 ; i<VertexNumber() ; i++)
	{
		// Normalize vertex normal
		VertexNormal(i).normalized();
	//	logfile<<vertex_normals[i]<<endl;
	}


}

//--
//
// ReadFile
//
//--
// Read mesh from a file
bool Mesh::ReadFile( const string& file_name )
{
	FileFormat file_format(UNKNOWN_FILE);

	cout<<"READ FILE"<<endl;
	// Find file extension
	int pos = file_name.find_last_of('.');
	if( pos == -1 )
	{
		// File has no extension
		return false;
	}

    ClearAll();

	// Format extension string
	string extension = UpperCase( file_name.substr( ++pos ) );

	cout<<"Extension="<<extension.data()<<endl;

	//Point Cloud extension
	if( extension == "XYZ" )
	{
		file_format = CLOUD_FILE;
	}
	else
	// RANGE TXT extension
	if( extension == "TXT" )
	{
		file_format = RANGE_FILE;
	}
	else
	// WRL extension
	if( extension == "WRL" )
	{
		file_format = VRML_1_FILE;
	}
	// IV extension
	else if( extension == "IV" )
	{
		file_format = IV_FILE;
	}
	// SMF extension
	else if( extension == "SMF" )
	{
		file_format = SMF_FILE;
	}
	// STL extension
	else if( extension == "STL" )
	{
		ifstream file(file_name.c_str(), ios::binary);
		if( file.is_open() == false )
		{
			return false;
		}
		char header[5];
		file.read(header, 5);
		if( strcmp(header, "solid") == 0 )
		{
			file_format = STL_ASCII_FILE;
		}
		else
		{
			file_format = STL_BINARY_FILE;
		}
		file.close();
	}
	// STLA extension
	else if( extension == "STLA" )
	{
		file_format = STL_ASCII_FILE;
	}
	// STLB extension
	else if( extension == "STLB" )
	{
		file_format = STL_BINARY_FILE;
	}
	// Other extension
	else
	{
		// Unknown file format
		cout<<"Unknown file extension"<<endl;
		return false;
	}

	// Read file
	switch( file_format )
	{

		// OpenInventor file
		case IV_FILE :

		// VRML 1.0 file
		case VRML_1_FILE :
			return ReadIv( *this, file_name );

		// Other file
		default :{cout<<"Unknown file format"<<endl;
			// Unknown file format
			return false;
				 }
	}
    originalVertices = vector<Vector3d>(vertices);
}

//--
//
// WriteFile
//
//--
// Write mesh to a file
bool Mesh::WriteFile( const string& file_name, const FileFormat& file_format ) const
{
	// Write file
	switch( file_format )
	{
		// OpenInventor file
		case IV_FILE :
			return WriteIv( *this, file_name, false );
			break;

		// SMF file
		case SMF_FILE :
		//	return WriteSmf( *this, file_name );
			break;

		// STL ascii file
		case STL_ASCII_FILE :
		//	return WriteStlA( *this, file_name );
			break;

		// STL binary file
		case STL_BINARY_FILE :
		//	return WriteStlB( *this, file_name );
			break;

		// VRML 1.0 file
		case VRML_1_FILE :
			return WriteIv( *this, file_name, true );
			break;

		// Other file
		default :
			// Unknown file format
			return false;
	}
}

//--
//
// UpperCase
//
//--
// Upper case string of a given one
string UpperCase( const string& s )
{
	// Upper case string
	string us;

	// For every character in the string
	string::const_iterator it = s.begin();
	while( it != s.end() )
	{
		// Convert character to upper case
		us += toupper( *it );

		// Next character
		++it;
	}

	// Return upper case string
	return us;
}






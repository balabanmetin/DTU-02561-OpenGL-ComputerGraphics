#include "Teapot.h"
#include "vertices.h"
#include "patches.h"
#include <cmath>
#include <iostream>

using namespace std;

namespace Angel {

	int   Index = 0;
	vec4  *points;

	
	enum { X = 0, Y = 1, Z = 2 };

	// forward declaration
	void divide_curve( vec4 c[4], vec4 r[4], vec4 l[4] );
	void draw_patch( vec4 p[4][4] );
	void transpose( vec4 a[4][4] );
	void divide_patch( vec4 p[4][4], int count );


	GLuint LoadTeapotVAO(int &outNumVertices, GLuint positionAttribute, 	int NumTimesToSubdivide, int PatchesPerSubdivision){
		int NumQuadsPerPatch =
			(int) std::ceil(std::pow((float) PatchesPerSubdivision, (float)NumTimesToSubdivide ));
		int NumTriangles =
			( NumTeapotPatches * NumQuadsPerPatch * 4 /* triangles / quad */ );
		int NumVertices =
			( NumTriangles * 3 /* vertices / triangle */ );
		outNumVertices = NumVertices;
		cout << "Num of Vertices in teapot "<<NumVertices<<" Number of patches "<<NumTeapotPatches<<endl;

		points = new vec4[NumVertices];
		for (int zOffset = -1;zOffset<=1;zOffset +=2){
			for ( int n = 0; n < NumTeapotPatches; n++ ) {
				vec4  patch[4][4];

				// Initialize each patch's control point data
				for ( int i = 0; i < 4; ++i ) {
					for ( int j = 0; j < 4; ++j ) {
						vec3& v = vertices[indices[n][i][j]];
						patch[i][j] = vec4( v[X], v[Y], v[Z]*zOffset, 1.0 );
					}
				}

				// Subdivide the patch
				divide_patch( patch, NumTimesToSubdivide );
			}
		}
		
		// Create a vertex array object
		GLuint vao;
		glGenVertexArrays( 1, &vao );
		glBindVertexArray( vao );

		// Create and initialize a buffer object
		GLuint buffer;
		glGenBuffers( 1, &buffer );
		glBindBuffer( GL_ARRAY_BUFFER, buffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof(vec4)*NumVertices, points, GL_STATIC_DRAW );

		// set up vertex arrays
		glEnableVertexAttribArray( positionAttribute );
		glVertexAttribPointer( positionAttribute, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)0);
		delete [] points;

		outNumVertices = NumVertices;
		return vao;
	}

	//----------------------------------------------------------------------------

	void divide_curve( vec4 c[4], vec4 r[4], vec4 l[4] ) {
		// Subdivide a Bezier curve into two equaivalent Bezier curves:
		//   left (l) and right (r) sharing the midpoint of the middle
		//   control point
		vec4  t, mid = ( c[1] + c[2] ) / 2;

		l[0] = c[0];
		l[1] = ( c[0] + c[1] ) / 2;
		l[2] = ( l[1] + mid ) / 2;

		r[3] = c[3];
		r[2] = ( c[2] + c[3] ) / 2;
		r[1] = ( mid + r[2] ) / 2;

		l[3] = r[0] = ( l[2] + r[1] ) / 2;

		for ( int i = 0; i < 4; ++i ) {
			l[i].w = 1.0;
			r[i].w = 1.0;
		}
	}

	//----------------------------------------------------------------------------

	void draw_patch( vec4 p[4][4] )
	{
		// Draw the quad (as two triangles) bounded by the corners of the
		//   Bezier patch.
		points[Index++] = p[0][0];
		points[Index++] = p[3][0];
		points[Index++] = p[3][3];
		points[Index++] = p[0][0];
		points[Index++] = p[3][3];
		points[Index++] = p[0][3];
	}

	//----------------------------------------------------------------------------

	void transpose( vec4 a[4][4] )
	{
		for ( int i = 0; i < 4; i++ ) {
			for ( int j = i; j < 4; j++ ) {
				vec4 t = a[i][j];
				a[i][j] = a[j][i];
				a[j][i] = t;
			}
		}
	}

	void divide_patch( vec4 p[4][4], int count )
	{
		if ( count > 0 ) {
			vec4 q[4][4], r[4][4], s[4][4], t[4][4];
			vec4 a[4][4], b[4][4];

			// subdivide curves in u direction, transpose results, divide
			// in u direction again (equivalent to subdivision in v)
			for ( int k = 0; k < 4; ++k ) {
				divide_curve( p[k], a[k], b[k] );
			}

			transpose( a );
			transpose( b );

			for ( int k = 0; k < 4; ++k ) {
				divide_curve( a[k], q[k], r[k] );
				divide_curve( b[k], s[k], t[k] );
			}

			// recursive division of 4 resulting patches
			divide_patch( q, count - 1 );
			divide_patch( r, count - 1 );
			divide_patch( s, count - 1 );
			divide_patch( t, count - 1 );
		}
		else {
			draw_patch( p );
		}
	}

	//----------------------------------------------------------------------------

}
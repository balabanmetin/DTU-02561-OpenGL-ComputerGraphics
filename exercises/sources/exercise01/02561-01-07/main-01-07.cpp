// 02561-01-07

#include "Angel.h"
#include <vector>

using namespace std;

const int NumTimesToSubdivide = 3;

vector<vec2> points;
int Index = 0;

//----------------------------------------------------------------------------

void triangle(const vec2& a, const vec2& b, const vec2& c) {
    points.push_back(a);
	points.push_back(b);
	points.push_back(c);
}

//----------------------------------------------------------------------------

void square(const vec2& a, const vec2& b, const vec2& c, const vec2& d) {
    points.push_back(a);
	points.push_back(b);
	points.push_back(c);
	points.push_back(a);
	points.push_back(d);
	points.push_back(c);
}

//----------------------------------------------------------------------------

void divide_triangle(const vec2& a, const vec2& b, const vec2& c, int count) {
	    if (count > 0) {
        vec2 v0 = ( a + b ) / 2.0;
        vec2 v1 = ( a + c ) / 2.0;
        vec2 v2 = ( b + c ) / 2.0;
        divide_triangle( a, v0, v1, count - 1 );
        divide_triangle( c, v1, v2, count - 1 );
        divide_triangle( b, v2, v0, count - 1 );
    } else {
        triangle( a, b, c );    // draw triangle at end of recursion
		}
	//////
  
}

//----------------------------------------------------------------------------

void divide_square(const vec2& a, const vec2& b, const vec2& c, const vec2& d, int count) {

      if (count > 0) {
        vec2 v0 = ( 2*a + b ) / 3.0;
        vec2 v1 = ( a + 2*b ) / 3.0;
        vec2 v2 = ( 2*b + c ) / 3.0;
        vec2 v3 = ( b + 2*c ) / 3.0;
        vec2 v4 = ( 2*c + d ) / 3.0;
        vec2 v5 = ( c + 2*d ) / 3.0;
        vec2 v6 = ( 2*d + a ) / 3.0;
        vec2 v7 = ( d + 2*a ) / 3.0;

        vec2 w0 = ( 2*a + c ) / 3.0;
        vec2 w1 = ( 2*b + d ) / 3.0;
        vec2 w2 = ( a + 2*c ) / 3.0;
        vec2 w3 = ( b + 2*d ) / 3.0;
        divide_square( a, v0, w0, v7, count - 1 );
        divide_square( v0, v1, w1, w0, count - 1 );
        divide_square( v1, b, v2, w1, count - 1 );
        divide_square( w1, v2, v3, w2, count - 1 );
        divide_square( w2, v3, c, v4, count - 1 );
        divide_square( w3, w2, v4, v5, count - 1 );
        divide_square( v6, w3, v5, d, count - 1 );
        divide_square( v7, w0, w3, v6, count - 1 );

	  } else {
        square( a, b, c, d );    // draw triangle at end of recursion
    }
}

//----------------------------------------------------------------------------

void init( void ) {
    vec2 vertices[4] = {
        vec2( -1.0, -1.0 ), vec2( -1.0, 1.0 ), vec2( 1.0, -1.0 )
    };

    // Subdivide the original triangle
    divide_triangle( vertices[0], vertices[1], vertices[2],
                     NumTimesToSubdivide );

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vec2)*points.size(), &points[0], GL_STATIC_DRAW );

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "const-shader.vert", "const-shader.frag", "fragColor" );
    glUseProgram( program );

    // Initialize the vertex position attribute from the vertex shader    
    GLuint loc = glGetAttribLocation( program, "position" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    glClearColor( 1.0, 0.0, 1.0, 1.0 ); /* white background */
}

//----------------------------------------------------------------------------
void init_square() {
	    vec2 vertices[4] = {
        vec2( -1.0, -1.0 ), vec2( -1.0, 1.0 ), vec2(1.0, 1.0), vec2( 1.0, -1.0 )
    };

	divide_square(vertices[0], vertices[1], vertices[2], vertices[3], NumTimesToSubdivide);
    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vec2)*points.size(), &points[0], GL_STATIC_DRAW );

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "const-shader.vert", "const-shader.frag", "fragColor" );
    glUseProgram( program );

    // Initialize the vertex position attribute from the vertex shader    
    GLuint loc = glGetAttribLocation( program, "position" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    glClearColor( 1.0, 0.0, 1.0, 1.0 ); /* white background */
}

void display( void ) {
    glClear( GL_COLOR_BUFFER_BIT );
	glDrawArrays( GL_TRIANGLES, 0, points.size() );
    glFlush();
	Angel::CheckError();
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

int main( int argc, char **argv ) {
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA|GLUT_3_2_CORE_PROFILE );
    glutInitWindowSize( 512, 512 );
    glutInitContextVersion( 3, 1 );
    glutInitContextProfile( GLUT_CORE_PROFILE );
    glutCreateWindow( "02561-01-07" );

	Angel::InitOpenGL();
	
    init_square();

    glutDisplayFunc( display );

	Angel::CheckError();

    glutMainLoop();
    return 0;
}

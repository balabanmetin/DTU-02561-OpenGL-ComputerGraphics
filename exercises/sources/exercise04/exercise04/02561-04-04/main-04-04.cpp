// 02561-04-04
#include "Angel.h"
#include "ObjLoader.h"
#include <iostream>

using namespace std;

float aspect = 1;

int selectedModel = 0;
std::vector<int> meshIndices;

int lastUpdate = 0;

// Array of rotation angles (in degrees) for each coordinate axis
int      Axis = 0;
GLfloat  Theta[3] = { 0.0, 30.0, 30.0 };

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;

GLuint program;

//----------------------------------------------------------------------------
void loadModel(int modelIndex){
	char *modelName;
	switch (modelIndex){
	case 0:
		modelName = "sphere_lo-res.obj";
		break;
	case 1:
		modelName = "cube.obj";
		break;
	case 2:
		modelName = "sphere_hi-res.obj";
		break;
	default:
		modelName = "suzanne.obj";
		break;
	}
	cout << "Loading model " << modelName << endl;
	std::vector<vec3> outPositions;
	meshIndices.clear();
	std::vector<vec3> outNormal;
	std::vector<vec2> outUv;
	loadObject(modelName, outPositions, meshIndices, outNormal, outUv, 0.5f);

	// Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * outPositions.size() * 2,
		  NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vec3) * outPositions.size(), &outPositions[0] );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec3) * outPositions.size(),
		     sizeof(vec3) * outPositions.size(), &outNormal[0] );

	// set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vNormal = glGetAttribLocation( program, "vNormal" ); 
	if (vNormal != GL_INVALID_INDEX ){
		glEnableVertexAttribArray( vNormal );
		glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,
				   BUFFER_OFFSET(sizeof(vec3) * outPositions.size()) );
	}
    
}

void reloadShader(){
	// Load shaders and use the resulting shader program
    program = InitShader( "phong-shader.vert", "phong-shader.frag", "fragColor" );
    glUseProgram( program );
	loadModel( selectedModel );
	 
    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
}

// OpenGL initialization
void init() {
    reloadShader();
    glEnable( GL_DEPTH_TEST );
	
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); 
}

//----------------------------------------------------------------------------

void display( void ) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4  projection = Perspective( 45.0, aspect, 0.5, 3.0 );

    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );

    //  Generate tha model-view matrixn
	const vec3 viewer_pos( 0.0, 0.0, 2.0 );
    mat4  model_view = ( Translate( -viewer_pos ) *
			 RotateX( Theta[0] ) *
			 RotateY( Theta[1] ) *
			 RotateZ( Theta[2] ) );
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );

	// Initialize shader lighting parameters
    vec4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
	
	vec4 light1_position( 0.0, 0.0, -1.0, 0.0 );
    vec4 light1_diffuse( 1.0, 0.0, 0.0, 1.0 );
    vec4 light1_specular( 1.0, 0.0, 0.0, 1.0 );

    vec4 light2_position( -3.0, -3.0, -1.0, 0.0 );
    vec4 light2_diffuse( 0.0, 1.0, 0.0, 1.0 );
    vec4 light2_specular( 0.0, 1.0, 0.0, 1.0 );
 
	vec4 light3_position(3.0, 3.0, -1.0, 0.0 );
    vec4 light3_diffuse( 0.0, 0.0, 1.0, 1.0 );
    vec4 light3_specular( 0.0, 0.0, 1.0, 1.0 );

    vec4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
    vec4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
    vec4 material_specular( 1.0, 0.8, 0.0, 1.0 );
    float  material_shininess = 100.0;


    glUniform4fv( glGetUniformLocation(program, "LightAmbient"),
		  1, light_ambient );

    glUniform4fv( glGetUniformLocation(program, "Light1Position"),
		  1, light1_position );
    glUniform4fv( glGetUniformLocation(program, "Light1Diffuse"),
		  1, light1_diffuse );
    glUniform4fv( glGetUniformLocation(program, "Light1Specular"),
		  1, light1_specular );

	glUniform4fv( glGetUniformLocation(program, "Light2Position"),
		  1, light2_position );
    glUniform4fv( glGetUniformLocation(program, "Light2Diffuse"),
		  1, light2_diffuse );
    glUniform4fv( glGetUniformLocation(program, "Light2Specular"),
		  1, light2_specular );

    glUniform4fv( glGetUniformLocation(program, "Light3Position"),
		  1, light3_position );
    glUniform4fv( glGetUniformLocation(program, "Light3Diffuse"),
		  1, light3_diffuse );
    glUniform4fv( glGetUniformLocation(program, "Light3Specular"),
		  1, light3_specular );

    glUniform4fv( glGetUniformLocation(program, "MaterialAmbient"),
		1, material_ambient );
    glUniform4fv( glGetUniformLocation(program, "MaterialDiffuse"),
		1, material_diffuse );
    glUniform4fv( glGetUniformLocation(program, "MaterialSpecular"),
		1, material_specular );

    glUniform1f( glGetUniformLocation(program, "Shininess"),
		 material_shininess );

    glDrawElements( GL_TRIANGLES, meshIndices.size(), GL_UNSIGNED_INT, &meshIndices[0]);
    glutSwapBuffers();
	Angel::CheckError();
}

//----------------------------------------------------------------------------

void mouse( int button, int state, int x, int y ) {
    if ( state == GLUT_DOWN ) {
		switch( button ) {
			case GLUT_LEFT_BUTTON:    
				Axis = (Axis+1) % 4;  
				if (Axis == 3){
					cout << "stop rotation"<<endl;
				} else {
					cout << "change rotation to axis "<<Axis <<endl;
				}
				break;
			case GLUT_RIGHT_BUTTON:
				selectedModel = (selectedModel+1)%4;
				loadModel(selectedModel);
				break;
		}
    }
}

//----------------------------------------------------------------------------

void idle( void ) {
	int time = glutGet(GLUT_ELAPSED_TIME);
    int deltaTimeMs = time-lastUpdate;
    lastUpdate = time;
	float rotationSpeed = 0.02f;
	if (Axis < 3){
		Theta[Axis] += rotationSpeed * deltaTimeMs;

		if ( Theta[Axis] > 360.0 ) {
		Theta[Axis] -= 360.0;
		}
	}

    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void keyboard( unsigned char key, int x, int y ) {
    switch( key ) {
	case 033: // Escape Key
	case 'q': 
	case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
	case 'R': 
	case 'r':
		cout << "Reload shaders" << endl;
		reloadShader();
		break;
    }
}

//----------------------------------------------------------------------------

void reshape( int width, int height ) {
    glViewport( 0, 0, width, height );

    aspect = float(width)/height;
}


int main(int argc, char* argv[]) {
	cout << "Left mouse to change rotation" << endl;
	cout << "Right mouse to change model" << endl;
	cout << "Press 'r' to reload shaders" << endl;
    glutInit(&argc, argv);
	glutInitContextVersion(3, 1);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    
	glutSetOption(
        GLUT_ACTION_ON_WINDOW_CLOSE,
        GLUT_ACTION_GLUTMAINLOOP_RETURNS
    );

	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_3_2_CORE_PROFILE);
	glutCreateWindow("02561-04-04");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutReshapeWindow(512, 512);

	Angel::InitOpenGL();
	

    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutReshapeFunc( reshape );
    glutMouseFunc( mouse );
    glutIdleFunc( idle );

	Angel::CheckError();

    glutMainLoop();

}
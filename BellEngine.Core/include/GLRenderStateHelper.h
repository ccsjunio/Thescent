#ifndef _GLRenderStateHelper_HG_
#define _GLRenderStateHelper_HG_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class GLRenderStateHelper
{
public:

	GLRenderStateHelper();

	GLboolean GL_depth_test_state = GL_TRUE;
	GLint GL_polygon_mode_state = GL_FILL;
	GLboolean GL_cull_face_enabled_state = GL_TRUE;
	GLint GL_cull_face_mode_state = GL_BACK;
	
	void SaveCurrentGLState(void);
	// Returns false if there's an invalid state
	bool RestoreCurrentGLState(std::string &error);
};

#endif

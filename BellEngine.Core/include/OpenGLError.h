#ifndef _OpenGLError_HG_
#define _OpenGLError_HG_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "ErrorLog.h"

class OpenGLError
{
public:
	// Note: these return **false** if there **WASN'T** an error

	// This might be backwards, but returns an error if there IS an error
	// This way, you can write if ( bWasThereAnGLError ) { HandleError...}
	// Also note: this is a very expensive operation that you should 
	//	only really call during loading assets or perhaps during debug, and "never" 
	//	during the render loop (at least not often) since it causes a GPU stall on 
	// the entire pipeline.
	// Most (all?) OpenGL calls that return information or state cause GPU stalls.

	// The "prependText" is for if you want to output some inforamtion to the error log

	static bool bWasThereAnOpenGLError(void);
	static bool bWasThereAnOpenGLError(GLenum &errorEnum);
	static bool bWasThereAnOpenGLError(GLenum &errorEnum, std::string &errorText);
	static bool bWasThereAnOpenGLError(GLenum &errorEnum, std::string &errorText, std::string &errorDetails);
	static bool bWasThereAnOpenGLError(std::string &errorText, std::string &errorDetails);

	static std::string TranslateErrorEnum(GLenum &errorEnum);
	static std::string TranslateErrorEnumDetails(GLenum &errorEnum, std::string &errorDetails);

	// These require an error logger class to be passed, too
	static bool LogOpenGLErrorIfPresent(ErrorLog &errorLogger );
	static bool LogOpenGLErrorIfPresent(std::string prependText, ErrorLog &errorLogger );
	static bool LogOpenGLErorrIfPresent(const int line, const char* file, ErrorLog &errorLogger );
	static bool LogOpenGLErorrIfPresent(std::string prependText, const int line, const char* file, ErrorLog &errorLogger );
};

#endif


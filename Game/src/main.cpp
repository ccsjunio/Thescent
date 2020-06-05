/***********************************************************************
* Project           : Game
* Author            : Felipe Bellini
* Date created      : Oct. 12 - 2019
* Licensed under:	: MIT

************************************************************************/

#include "main.h"

int main()
{
	glfwSetErrorCallback(error_callback);
	if (glfwInit())
	{
		GLFWwindow* window;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		window = glfwCreateWindow(1024, 768, "Thescent", NULL /*glfwGetPrimaryMonitor()*/, NULL);
		if (window)
		{
			glfwMakeContextCurrent(window);
			gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
			glfwSwapInterval(1);

			//Loads the game world -- contains all 'rules' for the game
			GameWorld* world = (GameWorld*)GameWorld::GetInstance();
			world->SetWindow(window);
			world->Initialize();

			//Configure window		
			SystemVideo::CenterWindow(window, glfwGetPrimaryMonitor());

			/*GAME LOOP*/
			FrameTime frameTime;
			while (!glfwWindowShouldClose(window) && !world->IsShuttingDown())
			{
				UpdateFrameTime(frameTime);

				glfwPollEvents();

				/*Calls update on the game world.*/
				world->Update(frameTime);

				glfwSwapBuffers(window);
			}

			glfwDestroyWindow(window);
			glfwTerminate();

			if (world) GameWorld::StaticDispose();
		}
		else
		{
			glfwTerminate();
		}
	}

	exit(EXIT_SUCCESS);
}

void UpdateFrameTime(FrameTime& frameTime)
{
	double currentTime = glfwGetTime();

	frameTime.SetDeltaTime(currentTime - frameTime.GetRunTime());
	frameTime.SetRunTime(currentTime);
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}
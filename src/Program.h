#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <vector>
#include <math.h>

#include "Geometry.h"
#include "InputHandler.h"
#include "RenderEngine.h"

class Program {

public:
	Program();
	void start();

private:
	GLFWwindow* window;
	RenderEngine* renderEngine;
	int n;
	float R, r, rotation, scale, theta;
	bool animation;

	Geometry *h, *C, *c;
	std::vector<Geometry*> geometryObjects;

	bool show_test_window;
	ImVec4 clear_color;

	static void error(int error, const char* description);
	void setupWindow();
	void mainLoop();
	void drawUI();

	void createTestGeometryObject();
	void createTestGeometryObject1();
	Geometry* createHypocycloid(float R, float r, int n, float rotation, float scale, bool animate);
	Geometry* createCircle(float r, float center_x, float center_y,float rotation, float scale);
	//Geometry* Program::createLine(float x, float y, float rotation)
};

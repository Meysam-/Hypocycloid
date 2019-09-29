#include "Program.h"

Program::Program() {
	window = nullptr;
	renderEngine = nullptr;
}

// Error callback for glfw errors
void Program::error(int error, const char* description) {
	std::cerr << description << std::endl;
}

// Called to start the program
void Program::start() {
	setupWindow();
	GLenum err = glewInit();
	if (glewInit() != GLEW_OK) {
		std::cerr << glewGetErrorString(err) << std::endl;
	}

	/*
	bool err = gl3wInit() != 0;

	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
	}
	*/
	renderEngine = new RenderEngine(window);
	InputHandler::setUp(renderEngine);
	mainLoop();
}

// Creates GLFW window for the program and sets callbacks for input
void Program::setupWindow() {
	glfwSetErrorCallback(Program::error);
	if (glfwInit() == 0) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(1024, 1024, "589 Boilerplate", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // V-sync on

	glfwSetKeyCallback(window, InputHandler::key);
	glfwSetMouseButtonCallback(window, InputHandler::mouse);
	glfwSetCursorPosCallback(window, InputHandler::motion);
	glfwSetScrollCallback(window, InputHandler::scroll);
	glfwSetWindowSizeCallback(window, InputHandler::reshape);

	// Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

	const char* glsl_version = "#version 430 core";

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

// Creates an object from specified vertices - no texture. Default object is a 2D triangle.
void Program::createTestGeometryObject() {
	Geometry* testObject = new Geometry();

	testObject->verts.push_back(glm::vec3(-5.f, -3.f, 0.f));
	testObject->verts.push_back(glm::vec3(5.f, -3.f, 0.f));
	testObject->verts.push_back(glm::vec3(0.f, 5.f, 0.f));
	testObject->color = glm::vec3(1, 0, 0);
	renderEngine->assignBuffers(*testObject);
	renderEngine->updateBuffers(*testObject);
	geometryObjects.push_back(testObject);
}

void Program::createTestGeometryObject1() {
	Geometry* testObject = new Geometry(GL_POINTS);
	glPointSize(5);
	glColor3f(1, 0, 0);
	testObject->verts.push_back(glm::vec3(-5.f, -3.f, 0.f));
	//testObject->verts.push_back(glm::vec3(5.f, -3.f, 0.f));
	renderEngine->assignBuffers(*testObject);
	renderEngine->updateBuffers(*testObject);
	geometryObjects.push_back(testObject);
}

Geometry* Program::createCircle(float r, float center_x, float center_y, float rotation = 0, float scale = 1) {
	Geometry* circle = new Geometry(GL_LINE_STRIP);
	circle->color = glm::vec3(0, 0, 0);
	float epsilon = 0.01;
	const float PI = 3.141592;
	rotation = (rotation * PI) / 180.0; // Convert to radians

	circle->verts.push_back(glm::vec3(0.f, 0.f, 0.f));
	for (float theta = PI/2; theta <= 2 * PI + PI/2; theta += epsilon) {
		float x = r * sin(theta);
		float y = r * cos(theta);
		circle->verts.push_back(glm::vec3(x, y, 0.f));
	}
	circle->modelMatrix = glm::rotate(rotation, glm::vec3(0, 0, 1));
	circle->modelMatrix = glm::scale(circle->modelMatrix, glm::vec3(scale, scale, 1));
	circle->modelMatrix = glm::translate(circle->modelMatrix, glm::vec3(center_x, center_y, 0));
	renderEngine->assignBuffers(*circle);
	renderEngine->updateBuffers(*circle);
	geometryObjects.push_back(circle);
	return circle;
}
/**
Geometry* Program::createLine(float x, float y, float rotation) {
	Geometry* line = new Geometry(GL_LINES);
	line->color = glm::vec3(0, 1, 0);
	line->modelMatrix = glm::rotate(rotation, glm::vec3(0, 0, 1));
	line->verts.push_back(glm::vec3(center_x * scale, center_y * scale, 0.f));
	line->verts.push_back(glm::vec3((center_x + r) * scale, center_y * scale, 0.f));
	renderEngine->assignBuffers(*line);
	renderEngine->updateBuffers(*line);
	geometryObjects.push_back(line);
}
/**/
Geometry* Program::createHypocycloid(float R, float r, int n, float rotation = 0, float scale = 1, bool animate = false) {
	Geometry* hypocycliod = new Geometry(GL_LINE_STRIP);
	hypocycliod->color = glm::vec3(1, 0, 0);
	float epsilon = 0.01;
	const float PI = 3.141592;
	rotation = (rotation * PI) / 180.0; // Convert to radians

	if (!animate) {
		for (float theta = 0; theta <= 2 * PI * n; theta += epsilon) {
			float x = (R - r) * cos(theta) + r * cos(((R - r) * theta) / r);
			float y = (R - r) * sin(theta) - r * sin(((R - r) * theta) / r);
			hypocycliod->verts.push_back(glm::vec3(x, y, 0.f));
		}
	}
	hypocycliod->modelMatrix = glm::rotate(rotation, glm::vec3(0, 0, 1));
	hypocycliod->modelMatrix = glm::scale(hypocycliod->modelMatrix, glm::vec3(scale, scale, 1));
	renderEngine->assignBuffers(*hypocycliod);
	renderEngine->updateBuffers(*hypocycliod);
	geometryObjects.push_back(hypocycliod);
	return hypocycliod;
}

void Program::drawUI() {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{

		ImGui::Begin("Hypocycloid Control Panel");                          // Create a window called "Hello, world!" and append into it.

		//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		//ImGui::Checkbox("Test Window", &show_test_window);      // Edit bools storing our window open/close state

		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		//ImGui::SameLine();
		//ImGui::Text("counter = %d", counter);
		ImGui::SliderFloat("R (radius of larger circle)", &R, 1.0f, 20.0f);
		ImGui::SliderFloat("r (radius of smaller circle)", &r, 1.0f, 20.0f);
		ImGui::SliderFloat("rotation (degree)", &rotation, -180.f, 180.0f);
		ImGui::SliderFloat("scale", &scale, 0.0f, 2.0f);
		ImGui::SliderInt("n (Number of cycles)", &n, 1, 100);
		if (ImGui::Button("Draw")) {                            // Buttons return true when clicked (most widgets return true when edited/activated)
			geometryObjects.clear();
			theta = 0;
			h = createHypocycloid(R, r, n, rotation, scale, animation);
			C = createCircle(R, 0, 0, rotation, scale);
			c = createCircle(r, R - r, 0, rotation, scale);
		}
		ImGui::SameLine();
		ImGui::Checkbox("Animate", &animation);
		ImGui::End();
	}
	if (animation) {
		if (geometryObjects.size() == 1) {
			geometryObjects.clear();
			theta = 0;
			h = createHypocycloid(R, r, n, rotation, scale, animation);
			C = createCircle(R, 0, 0, rotation, scale);
			c = createCircle(r, R - r, 0, rotation, scale);
		}
	}
	else {
		if (geometryObjects.size() == 3) {
			geometryObjects.clear();
			h = createHypocycloid(R, r, n, rotation, scale, animation);
		}
	}
	// 3. Show another simple window.
	if (show_test_window)
	{
		ImGui::Begin("Another Window", &show_test_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("This is an example popup window.");
		if (ImGui::Button("Close Me"))
			show_test_window = false;
		ImGui::End();
	}
}

// Main loop
void Program::mainLoop() {
	//createTestGeometryObject1();
	n = 5;
	R = 19;
	r = 5;
	rotation = 0;
	scale = 0.4;
	theta = 0;

	animation = false;

	h = createHypocycloid(R, r, n, rotation, scale);
	C = createCircle(R, 0, 0, rotation, scale);
	c = createCircle(r, R - r, 0, rotation, scale);

	// Our state
	show_test_window = false;
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		drawUI();

		// Rendering
		_sleep(10);
		if (geometryObjects.size() == 3 && theta < 2 * 3.141592 * n) {
			c->modelMatrix = glm::rotate(0.01f, glm::vec3(0, 0, 1)) * c->modelMatrix * glm::rotate(-(0.01f * R / r), glm::vec3(0, 0, 1));
			float x = (R - r) * cos(theta) + r * cos(((R - r) * theta) / r);
			float y = (R - r) * sin(theta) - r * sin(((R - r) * theta) / r);
			h->verts.push_back(glm::vec3(x, y, 0.f));
			renderEngine->updateBuffers(*h);
			theta += 0.01;
		}
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		glPointSize(1);

		renderEngine->render(geometryObjects, glm::mat4(1.f));
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Clean up, program needs to exit
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	delete renderEngine;
	glfwTerminate();
}

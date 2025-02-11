#include <Application.hpp>

// Static declaration(s)
float Application::s_m_MouseScrollOffset = 0.f;

Application::Application(int _width, int _height) : m_scene(_width, _height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	m_window = glfwCreateWindow(_width, _height, "Threaded Resource Manager", NULL, NULL);
	if (m_window == NULL)
	{
		glfwTerminate();
		// Always assert I want the above  code to apply
		Assert(false, "Failed to create GLFW window");
	}
	glfwMakeContextCurrent(m_window);
	Assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize GLAD");
	Log::Print("'S' + 'I' Keys to hide/show controls");

	glViewport(0, 0, _width, _height);
	glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
	glEnable(GL_DEPTH_TEST);
	glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
	SetupImGui(m_window);
}

Application::~Application() {
	Destroy();
};

void Application::Destroy()
{
	m_scene.Destroy();

	// IMGUI Destroyed
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	glDisable(GL_DEPTH_TEST);
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Application::Update()
{
	float lastFrame = 0.f;
	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();
		StartImGuiFrame();
		float currentFrame = static_cast<float>(glfwGetTime());
		m_deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		ProcessInput(m_window);
		m_scene.Update(m_deltaTime, m_inputs);
		if (m_ShowControls)
			ShowImGuiControls();
		Render(m_window);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(m_window);
	}
}

void Application::ApplyChangeColor() {
	glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
}

void Application::ChangeColor(float _newcolor[4])
{
	for (int i = 0; i < 4; i++)
		m_ClearColor[i] = _newcolor[i];
	ApplyChangeColor();
}

void Application::ShowImGuiControls()
{
	if (ImGui::Begin("Config"))
	{
		ImGui::Text("'S' + 'I' Keys to hide/show controls");
		ImGui::Text("'R' to restart");
		if (ImGui::CollapsingHeader("Framebuffer", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("FPS : %f", 1.f / ImGui::GetIO().DeltaTime);
			if (ImGui::ColorEdit4("clearColor", m_ClearColor))
				ApplyChangeColor();
		}
		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
			m_scene.camera.ShowImGuiControls();
	}
	ImGui::End();
}

void Application::ProcessInput(GLFWwindow* _window)
{
	static double s_LastPressed = glfwGetTime();
	double timeToApply = .5;
	float mixValue = 0.2f;

	// App
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(_window, true);
	if ((glfwGetKey(_window, GLFW_KEY_S) & glfwGetKey(_window, GLFW_KEY_I)) == GLFW_PRESS && (glfwGetTime() - s_LastPressed) > timeToApply)
	{
		s_LastPressed = glfwGetTime();
		m_ShowControls = !m_ShowControls;
	}
	if (glfwGetKey(_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		m_scene.Restart();
	}
	// LearnOpenGL
	if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.001f; // Change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue >= 1.0f)
			mixValue = 1.0f;
	}

	if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.001f; // Change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue <= 0.0f)
			mixValue = 0.0f;
	}

	// Camera, should clean
	{
		double newMouseX, newMouseY;
		glfwGetCursorPos(_window, &newMouseX, &newMouseY);
		m_mouseDeltaX = (float)(newMouseX - m_mouseX);
		m_mouseDeltaY = (float)(newMouseY - m_mouseY);
		m_mouseX = newMouseX;
		m_mouseY = newMouseY;
	}

	// Update camera
	if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		m_inputs.deltaX = m_mouseDeltaX;
		m_inputs.deltaY = m_mouseDeltaY;
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else
	{
		m_inputs.deltaX = 0.f;
		m_inputs.deltaY = 0.f;
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	m_inputs.moveForward = glfwGetKey(_window, GLFW_KEY_W);
	m_inputs.moveBackward = glfwGetKey(_window, GLFW_KEY_S);
	m_inputs.moveRight = glfwGetKey(_window, GLFW_KEY_D);
	m_inputs.moveLeft = glfwGetKey(_window, GLFW_KEY_A);
	glfwSetScrollCallback(_window, Scroll_callback);

	if (s_m_MouseScrollOffset)
	{
		m_scene.camera.Zoom(s_m_MouseScrollOffset);
		s_m_MouseScrollOffset = 0.f;
	}
	// Cam end
}

void Application::Scroll_callback(GLFWwindow* _window, double _xoffset, double _yoffset) {
	s_m_MouseScrollOffset = (float)_yoffset;
}

void Application::framebuffer_size_callback(GLFWwindow* _window, int _width, int _height) {
	glViewport(0, 0, _width, _height);
}

void Application::SetupImGui(GLFWwindow* _window)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable viewports

	io.Fonts->AddFontDefault();

	// GL 3.0 + GLSL 130
	const char* const glslVersion = "#version 130";

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(_window, true);
	ImGui_ImplOpenGL3_Init(glslVersion);
}

void Application::StartImGuiFrame()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Application::Render(GLFWwindow* _window)
{
	ImGui::Render();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	m_scene.Draw();

	GLFWwindow* ctxBackup = glfwGetCurrentContext();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	glfwMakeContextCurrent(ctxBackup);
}
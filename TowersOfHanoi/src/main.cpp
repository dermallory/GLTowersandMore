#include <gl\glew.h>
#include <glfw\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <random>
#include <ctime>

#include <mesh.h>
#include <shader.h>
#include <model.h>
#include <camera.h>
#include <game.h>
#include <worldobject.h>
#include <movetowards.h>
#include <rotatetowards.h>

// opengl debug
void APIENTRY glDebugOutput(GLenum source, 
	GLenum type, 
	GLuint id, 
	GLenum severity, 
	GLsizei length, 
	const GLchar *message, 
	void *userParam);

// glfw callbacks
void error_callback(int error, const char* description);
//void window_close_callback(GLFWwindow* window, int focused);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
//void window_resize_callback(GLFWwindow* window, int width, int height);
//void frame_buffer_size_callback(GLFWwindow* window, int width, int height);

// my prototypes
void gl_init();
void shutdown();
void setup_meshes();
void setup_shaders();
void setup_models();
void setup_attributes();
void setup_camera();
void spawn_world();
void update(float deltaTime);
void render();

// utility prototypes
GLuint load_shaders(const char * vertex_file_path,const char * fragment_file_path);

// constants
const int WINDOW_WIDTH = 1080;
const int WINDOW_HEIGHT = 640;
const int FRAMES_PER_SECOND = 60;

// globals
GLFWwindow* g_window;

std::unordered_map<model_type, Mesh*> g_meshes;
std::unordered_map<model_type, Model*> g_models;
std::unordered_map<shader_type, Shader*> g_shaderPrograms;
PerspectiveCamera* g_perspective = nullptr;
OrthoCamera* g_ortho = nullptr;

Game* g_game = nullptr;

std::vector<int> g_keysPressed;

bool isOrtho = true;

float pressDelay = 0;

int main()
{
	gl_init();

	setup_meshes();
	setup_models();
	setup_shaders();
	setup_attributes();
	setup_camera();

	spawn_world();

	// main loop
	double frameDelta = (double)1/FRAMES_PER_SECOND;
	double accumulatedTime = 0;

	while (!glfwWindowShouldClose(g_window))
	{
		double timeSimulatedThisIteration = 0;
		double startTime = glfwGetTime();

		while ( accumulatedTime >= frameDelta )
		{
			update((float)frameDelta);
			accumulatedTime -= frameDelta;
			timeSimulatedThisIteration += frameDelta;


			if (pressDelay > 0)
			{
				pressDelay -= frameDelta;

			}
			if (pressDelay < 0)
			{
				pressDelay = 0;
			}
		}

		render();

		glfwSwapBuffers(g_window);
		glfwPollEvents();

		accumulatedTime += glfwGetTime() - startTime;
	}

	shutdown();	
	
	return 0;
}


/*
* SETUP & SHUTDOWN
*/
void gl_init()
{
	// initialization
	glfwSetErrorCallback(error_callback);

	glfwInit();
	if (!glfwInit())
	{
		fprintf(stderr, "GLFW initialization has failed.\n");
		exit(1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_DEPTH_BITS, GL_TRUE);

#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Towers Of Hanoi", NULL, NULL);
	if (!window)
	{
		fprintf(stderr, "GLFW initialization has failed.\n");
		exit(1);
	}
	g_window = window;

	//set_windowed(window, windowWidth, windowHeight);

	// set the current opengl context (that was previously created)
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "GLFW initialization has failed.\n");
		return exit(1);
	}

	GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		// initialize debug output 
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
		glDebugMessageCallback((GLDEBUGPROC)glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	//glfwSetWindowCloseCallback(window, GLFWwindowclosefun(window_close_callback));
	//glfwSetWindowSizeCallback(window, GLFWwindowsizefun(window_resize_callback));
	//glfwSetFramebufferSizeCallback(window, GLFWframebuffersizefun(frame_buffer_size_callback));
	glfwSetKeyCallback(window, GLFWkeyfun(key_callback));
	glfwSetMouseButtonCallback(window, GLFWmousebuttonfun(mouse_button_callback));

	// v-sync
	if (glfwExtensionSupported("WGL_EXT_swap_control"))
	{
		glfwSwapInterval(1);
	}
	else {
		printf("V-Sync is not supported.\n");
	}
}

void shutdown()
{
	// destroy shaders, buffers, etc. here (before gl-context is destroyed)
	// ...

	// destroy "game"
	delete g_game;

	// destroy camera
	if (g_perspective)
	{
		delete g_perspective;
	}
	if (g_ortho)
	{
		delete g_ortho;
	}

	// destroy shaders
	for each (auto program_pair in g_shaderPrograms)
	{
		Shader* shader = program_pair.second;
		glDeleteProgram(shader->program);

		for each (auto uniform in shader->uniforms)
		{
			delete uniform.second;
		}

		delete shader;
	}

	g_shaderPrograms.clear();
	
	// destroy models
	for (auto iter = g_models.begin(); iter != g_models.end(); ++iter)
	{
		Model* model = iter->second;

		if (model)
		{
			glDeleteVertexArrays(1, &model->vao);
			delete model;
		}
	}
	
	g_models.clear();

	// destroy meshes
	for (auto iter = g_meshes.begin(); iter != g_meshes.end(); ++iter)
	{
		Mesh* mesh = iter->second;

		if (mesh)
		{
			delete mesh;
		}
	}

	g_meshes.clear();

	// TODO:
	// delete any other buffers here...

	// shutdown gl context
	glfwDestroyWindow(g_window);
	glfwTerminate();
}

void setup_meshes()
{
	Mesh* ringMesh = new Mesh();
	bool result = ringMesh->loadMesh("torus.obj");
	if (!result)
	{
		printf("Failure to load ring mesh.\n");
		delete ringMesh;		
	}
	else
	{
		g_meshes.emplace(std::make_pair(model_type::RING, ringMesh));
	}

	Mesh* pegMesh = new Mesh();
	result = pegMesh->loadMesh("cube.obj");
	if (!result)
	{
		printf("Failure to load ring mesh.\n");
		delete pegMesh;		
	}
	else
	{
		g_meshes.emplace(std::make_pair(model_type::PEG, pegMesh));
	}
}

void setup_models()
{
	// ring
	Model* model = new Model();
	model->shader = shader_type::GENERAL_DIFFUSE;
	model->type = model_type::RING;
	
	glCreateVertexArrays(1, &model->vao);

	g_models.emplace(model->type, model);

	// peg
	model = new Model();
	model->shader = shader_type::GENERAL_DIFFUSE;
	model->type = model_type::PEG;

	glCreateVertexArrays(1, &model->vao);

	g_models.emplace(model->type, model);
}

void setup_shaders()
{
	// setup for diffuse shader
	GLuint diffuse_program = load_shaders("diffuse.vs", "diffuse.fs");
	GLuint diffuse_color = glGetUniformLocation(diffuse_program, "color");
	GLuint diffuse_mvp = glGetUniformLocation(diffuse_program, "mvp");

	Shader* diffuse_shader = new Shader();
	diffuse_shader->program = diffuse_program;
	diffuse_shader->type = shader_type::GENERAL_DIFFUSE;
	diffuse_shader->uniforms.emplace(shader_uniform::COLOR, new GLuint(diffuse_color));
	diffuse_shader->uniforms.emplace(shader_uniform::MVP, new GLuint(diffuse_mvp));

	g_shaderPrograms.emplace(diffuse_shader->type, diffuse_shader);
}

void setup_attributes()
{
	for each(auto mesh_pair in g_meshes)
	{
		const Mesh* mesh = mesh_pair.second;
		const Model* model = g_models[mesh_pair.first];
		
		Mesh::MeshAttributeFlags flags = mesh->flags();
		
		glVertexArrayVertexBuffer(model->vao, 0, mesh->vertexBuffer(), NULL, mesh->stride());
		glVertexArrayElementBuffer(model->vao, mesh->indexBuffer());

		if (model->shader == shader_type::GENERAL_DIFFUSE)
		{
			if (flags & ATTRIB_POS)
			{				
				glVertexArrayAttribBinding(model->vao, 0, 0);
				glVertexArrayAttribFormat(model->vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Mesh::MeshVertex, position));
				glEnableVertexArrayAttrib(model->vao, 0);
			}
			if (flags & ATTRIB_NORM)
			{
				glVertexArrayAttribBinding(model->vao, 1, 0);
				glVertexArrayAttribFormat(model->vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Mesh::MeshVertex, normal));
				glEnableVertexArrayAttrib(model->vao, 1);
			}
		}
	}
}

void setup_camera()
{
	// setup the perspective camera using default settings
	auto perspectiveSettings = PerspectiveCameraSettings((float)WINDOW_WIDTH/WINDOW_HEIGHT);

	Transform perspectiveTransform;
	perspectiveTransform.position = glm::vec3(0, 0, 5);

	g_perspective = new PerspectiveCamera(perspectiveTransform, perspectiveSettings);

	// setup the ortho camera using default settings
	auto orthoSettings = OrthoCameraSettings();
	orthoSettings.zFar = 100.0f;

	Transform orthoTransform;
	orthoTransform.position = glm::vec3(0, 0, 50.f);

	g_ortho = new OrthoCamera(orthoTransform, orthoSettings);
}

void spawn_world()
{
	g_game = new Game();
}

/*
* UPDATING
*/
void update(float deltaTime)
{
	//g_perspective->rotateBy(glm::vec3(0, 0.05f, 0));
	//g_perspective->rotateBy(glm::vec3(0, 0.05f, 0));
	//g_perspective->m_settings.m_zoom = sin(zoom+=5.f);
	//g_ortho->rotateBy(0, 0.005f, 0);

	int latestKeyPress = g_keysPressed.size() > 0 ? g_keysPressed[g_keysPressed.size()-1] : -1;
	g_keysPressed.clear();
	g_game->update(deltaTime, latestKeyPress);
}

void render()
{
	GLfloat clearColor[3] = {0.0f, 0.0f, 0};
	glClearBufferfv(GL_COLOR, 0, clearColor);

	// get the projection-view matrix
	glm::mat4 projection_view = isOrtho ? g_ortho->viewProjection() : g_perspective->viewProjection();

	// render each object in the "world"
	for each(auto obj in g_game->m_worldObjects)
	{
		Transform& transform = obj->getTransform();
		model_type type = obj->getModelType();

		Model* model = g_models[type];
		Mesh* mesh = g_meshes[type];
		Shader* shader = g_shaderPrograms[model->shader];

		// set the mvp
		glm::mat4 model_scale = glm::scale(glm::mat4(), transform.scale);
		glm::quat model_rotation = transform.rotation();
		glm::mat4 model_translation = glm::translate(glm::mat4(), transform.position);

		glm::mat4 model_mat = model_translation*glm::mat4_cast(model_rotation)*model_scale;
		
		glm::mat4 mvp = projection_view*model_mat;

		glUseProgram(shader->program);

		// set the uniforms in the model's shader
		if (shader->type == shader_type::GENERAL_DIFFUSE)
		{
			const GLuint* color_uniform = shader->uniforms[shader_uniform::COLOR];
			const GLuint* mvp_uniform = shader->uniforms[shader_uniform::MVP];

			glUniform4fv(*color_uniform, 1, glm::value_ptr(obj->getColor()));
			glUniformMatrix4fv(*mvp_uniform, 1, GL_FALSE, glm::value_ptr(mvp));
		}

		glBindVertexArray(model->vao);
		glDrawElements(GL_TRIANGLE_STRIP, mesh->indicesCount(), GL_UNSIGNED_INT, NULL);
		glBindVertexArray(0);
	}
}


/*
* GLFW CALLBACKS
*/
void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	g_keysPressed.push_back(key);

	if (pressDelay == 0)
	{
		if (key == GLFW_KEY_SPACE)
		{
			isOrtho = !isOrtho;
			pressDelay = 0.25f;
		}

		// camera rotation
		if (key == GLFW_KEY_A && !isOrtho)
		{
			g_perspective->rotateBy(0, -0.05f, 0);
			pressDelay = 0.25f;
		}
		if (key == GLFW_KEY_D && !isOrtho)
		{
			g_perspective->rotateBy(0, 0.05f, 0);
			pressDelay = 0.25f;
		}

		// camera translation
		if (key == GLFW_KEY_S && !isOrtho)
		{
			g_perspective->translateBy(0, 0, 0.75f);
			pressDelay = 0.25f;
		}
		if (key == GLFW_KEY_W && !isOrtho)
		{
			g_perspective->translateBy(0, 0, -0.75f);
			pressDelay = 0.25f;
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
	{
		MoveTowards* move_towards = new MoveTowards(g_game->m_worldObjects[0], glm::vec3(3, 0, -1), 5.0f);

		g_game->m_worldObjects[0]->pushAction(move_towards);
	}
}


/*
* OPENGL DEBUG
*/
void APIENTRY glDebugOutput(GLenum source, 
	GLenum type, 
	GLuint id, 
	GLenum severity, 
	GLsizei length, 
	const GLchar *message, 
	void *userParam)
{
	// ignore non-significant error/warning codes
	if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " <<  message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}


/*
* UTILITY
*/
GLuint load_shaders(const char * vertex_file_path,const char * fragment_file_path)
{

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}	

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

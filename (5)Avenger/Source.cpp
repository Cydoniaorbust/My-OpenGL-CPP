#include "Source.h"

#include "Res_Manager.h"
#include "Camera.h"

#include <iomanip>
#include <time.h>

#define SHADERS_VERT_SIMP	"D:/Google/Resources/Shaders/Vert/Simp.vert"
#define SHADERS_FRAG_SIMP	"D:/Google/Resources/Shaders/Frag/Simp.frag"
#define SHADERS_VERT_OBJ	"D:/Google/Resources/Shaders/Vert/Obj.vert"
#define SHADERS_FRAG_OBJ	"D:/Google/Resources/Shaders/Frag/Dir.frag"
#define SHADERS_VERT_BACK	"D:/Google/Resources/Shaders/Vert/Back.vert"
#define SHADERS_FRAG_BACK	"D:/Google/Resources/Shaders/Frag/TexOnly.frag"
							 
#define OBJECT_MODEL_PLAYER "D:/Google/Resources/Model/Ship/Ship.obj"
#define OBJECT_COROVAN		"D:/Google/Resources/Model/corovan/Wagons.obj"
#define BACKGROUND_IMAGE	"D:/Google/Resources/Tex/Back2.jpg"

GLEnvironment Avenger;
Res_Manager Manager;
Camera Cam(vec3(0, 10, 0));
uint BackGroundImage;
vec3 LightAmb(0), LightDir(-0.2f, -1, -0.3f), LightDifDir(1), LightDifSpot(0);

void framebuffer_size_callback(GLFWwindow* Win, int width, int height) { glViewport(0, 0, width, height); }
void processInput(GLFWwindow *Win) {
	if (glfwGetKey(Win, GLFW_KEY_ESCAPE) == GLFW_PRESS) exit(0);
	if (glfwGetKey(Win, GLFW_KEY_W) == GLFW_PRESS) Manager.AllObjects[0].Move(vec3(0, 0, Avenger.GetDelta() * 10));
	if (glfwGetKey(Win, GLFW_KEY_S) == GLFW_PRESS) Manager.AllObjects[0].Move(vec3(0, 0, -Avenger.GetDelta() * 10));
	if (glfwGetKey(Win, GLFW_KEY_A) == GLFW_PRESS) Manager.AllObjects[0].Move(vec3(Avenger.GetDelta() * 10, 0, 0));
	if (glfwGetKey(Win, GLFW_KEY_D) == GLFW_PRESS) Manager.AllObjects[0].Move(vec3(-Avenger.GetDelta() * 10, 0, 0));
}

void DrawScene(GLuint shader) {
	Shader::Use(shader);

	Shader::SetMat4(shader, "view", Cam.View);
	Shader::SetMat4(shader, "projection", glm::perspective(radians(90.0f), (GLfloat)Avenger.GetWidth() / (GLfloat)Avenger.GetHeight(), 0.1f, 1000.0f));
	Shader::SetMat4(shader, "model", Manager.AllObjects[0].position);
	//Basic
	Shader::SetVec3(shader, "ViewPos", Cam.Position);
	Shader::SetVec3(shader, "Ambient", LightAmb);
	//Directional
	Shader::SetVec3(shader, "DirLight.direction", LightDir);
	Shader::SetVec3(shader, "DirLight.diffuse", LightDifDir);
	Manager.AllObjects[0].Draw(shader);

	Shader::SetMat4(shader, "model", Manager.AllObjects[1].position);
	Manager.AllObjects[1].Draw(shader);
}
void DrawBack(GLuint shader) {
	float t = glfwGetTime() * 0.1f;
	GLfloat Background[] = {
		 10, 0, 10, t + 1, 1,
		 10, 0,-10, t + 1, 0,
		-10, 0,-10, t + 0, 0,
		-10, 0, 10, t + 0, 1
	};
	GLuint Indices[] = { 0, 1, 3, 1, 2, 3 };
//Assigning
	GLuint VAO_n, VBO_n, EBO_n;
	glGenVertexArrays(1, &VAO_n);
	glGenBuffers(1, &VBO_n);
	glGenBuffers(1, &EBO_n);
	glBindVertexArray(VAO_n);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_n);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_n);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Background), Background, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
	//Attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
//Shader
	Shader::Use(shader);
	Shader::SetMat4(shader, "view", Cam.View);
	Shader::SetMat4(shader, "projection", glm::perspective(radians(90.0f), (GLfloat)Avenger.GetWidth() / (GLfloat)Avenger.GetHeight(), 0.1f, 1000.0f));
	Shader::SetMat4(shader, "model", mat4());
//Drawing
	glBindTexture(GL_TEXTURE_2D, BackGroundImage);
	glBindVertexArray(VAO_n);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
//Cleaning
	glDeleteVertexArrays(1, &VAO_n);
	glDeleteBuffers(1, &VBO_n);
	glDeleteBuffers(1, &EBO_n);
}

int main(int argc, char * argv[]) {
	srand((GLuint)time(NULL));

	try {
		//Basic
		Avenger.InitWin(800, 600, "OpenGL");
		Avenger.CallbackSet(framebuffer_size_callback, nullptr, nullptr, nullptr);
		Avenger.InitGLAD();
		Avenger.ApplyTests();
		//Preparation
		Manager.AllShaders.resize(3);
		Manager.AllShaders[0] = Shader::CreateProgram(SHADERS_VERT_SIMP, SHADERS_FRAG_SIMP);	//AxisShader
		Manager.AllShaders[1] = Shader::CreateProgram(SHADERS_VERT_OBJ, SHADERS_FRAG_OBJ);		//Shader
		Manager.AllShaders[2] = Shader::CreateProgram(SHADERS_VERT_BACK, SHADERS_FRAG_BACK);	//Back
	//Texture
		glGenTextures(1, &BackGroundImage);
		glBindTexture(GL_TEXTURE_2D, BackGroundImage);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int width, height, nrChannels;
		unsigned char *data = stbi_load(BACKGROUND_IMAGE, &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else std::cout << "Failed to load texture" << std::endl;
		stbi_image_free(data);
		//Model
		Manager.AllObjects.resize(2);

		Manager.AllObjects[0] = Model(OBJECT_MODEL_PLAYER);
		Manager.AllObjects[0].Rotate(90.0f, vec3(0, 1, 0));
		Manager.AllObjects[0].Scale(vec3(0.5));

		Manager.AllObjects[1] = Model(OBJECT_COROVAN);
		Manager.AllObjects[1].Move(vec3(6, 0, 0));
		Manager.AllObjects[1].Rotate(180.0f, vec3(0, 1, 0));
		Manager.AllObjects[1].Scale(vec3(0.7));

		GLuint nbFrames = 0;
		GLfloat LastTime = (GLfloat)glfwGetTime();
		//Main loop
		while (!glfwWindowShouldClose(Avenger.GetWin())) {
			processInput(Avenger.GetWin());
			//Scene and Camera
			Avenger.UpdateFrames();
			glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//Drawing
			DrawBack(Manager.AllShaders[2]);
			DrawScene(Manager.AllShaders[1]);
			//FPS counter
			nbFrames++;
			if (GLfloat(glfwGetTime()) - LastTime >= 1.0) {
				std::cout << nbFrames << " fps\n";// printf("%f ms/frame\n", 1000.0/double(nbFrames));
				//std::cout << fixed << setprecision(0) << "["<< camera.Position.x << "]:["<< camera.Position.y << "]:["<< camera.Position.z << "]" << std::endl;
				nbFrames = 0;
				LastTime += 1.0;
			}
			//Finish
			glfwSwapBuffers(Avenger.GetWin());
			glfwPollEvents();
		}
		//Cleaning
		glfwTerminate();
	}
	catch (MYERROR* e) {
		std::cerr << e->what() << std::endl;
		delete e;
		glfwTerminate();
	}

	return 0;
}
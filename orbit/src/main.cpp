#include "file_system.h"
#include "glm_extension.h"
#include "geometry.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/index_buffer.h"
#include "graphics/vertex_buffer.h"
#include "graphics/vertex_array.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

const char* shader_vs = R"(
#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out VS_OUT {
    vec2 texCoord;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0f);
    vs_out.texCoord = texCoord;
}
)";
const char* shader_fs = R"(
#version 460 core

out vec4 fragColor;

in VS_OUT {
    vec2 texCoord;
} vs_in;

uniform sampler2D texture_diffuse1;

void main() {
    fragColor = texture(texture_diffuse1, vs_in.texCoord);
}
)";

const char* line_shader_vs = R"(
#version 460 core

layout (location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0f);
}
)";
const char* line_shader_fs = R"(
#version 460 core

out vec4 fragColor;

uniform vec3 color;

void main() {
    fragColor = vec4(color, 1.0);
}
)";

float delta_time = 0.0f;
float last_update_time = 0.0f;

int main(int argc, char** argv) {
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Scene", NULL, NULL);
    if (window == NULL) {
		printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
	
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD");
        return -1;
    }
	
	glEnable(GL_DEPTH_TEST);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsLight();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	Shader shader;
	shader.createFromSource(shader_vs, shader_fs);

	// Create earth
	Texture2D earth_texture;
	std::string path = "images/earth2048.bmp";
	earth_texture.createFromFile(join_path(path).c_str());

	glm::vec3 earth_position = glm::vec3(0.0, 0.0, -60.0);
	
	std::vector<float> earth_vertices = generate_sphere_vertices(10, 36, 18);
	std::vector<unsigned int> earth_indices = generate_sphere_indices(36, 18);
	
	int vertices_size = earth_vertices.size() * sizeof(float);
	int indices_size = earth_indices.size() * sizeof(unsigned int);

	VertexArray earth_va;
	VertexBuffer earth_vb;
	IndexBuffer earth_ib;

	earth_va.create();
	earth_va.bind();
	
	std::vector<VertexAttribute> attribs = {
	    VertexAttribute(AttributeType::VEC3, GL_FALSE),
	    VertexAttribute(AttributeType::VEC3, GL_FALSE),
		VertexAttribute(AttributeType::VEC2, GL_FALSE)
	};

	earth_vb.create(earth_vertices.data(), vertices_size, attribs);
	earth_ib.create(earth_indices.data(), indices_size);

	earth_va.addVertexBuffer(earth_vb);
	earth_va.setIndexBuffer(earth_ib);
	
	// Create moon
	Texture2D moon_texture;
	path = "images/moon1024.bmp";
	moon_texture.createFromFile(join_path(path).c_str());
	
	std::vector<float> moon_vertices = generate_sphere_vertices(5, 36, 18);
	std::vector<unsigned int> moon_indices = generate_sphere_indices(36, 18);
	
	vertices_size = moon_vertices.size() * sizeof(float);
	indices_size = moon_indices.size() * sizeof(unsigned int);

	VertexArray moon_va;
	VertexBuffer moon_vb;
	IndexBuffer moon_ib;

	moon_va.create();
	moon_va.bind();
	
	moon_vb.create(moon_vertices.data(), vertices_size, attribs);
	moon_ib.create(moon_indices.data(), indices_size);

	moon_va.addVertexBuffer(moon_vb);
	moon_va.setIndexBuffer(moon_ib);

	Shader line_shader;
	line_shader.createFromSource(line_shader_vs, line_shader_fs);
	glm::vec3 line_color = glm::vec3(1, 1, 1);
	float line_thickness = 5;
	
	glClearColor(0, 0, 0, 1.0f);
	glLineWidth(line_thickness);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
	
	float moon_rotation_speed = 100.0f;
	float orbit_radius = 10.0;
	float inclication_angle = 0.0f; // greendwidge meridian angle
	float rotation_angle = 0.0f; // angle relative to the equator 

    while (!glfwWindowShouldClose(window)) {

		// Update the variables
		float current_time = (float)glfwGetTime();
        delta_time = current_time - last_update_time;
        last_update_time = current_time;
		
		glm::vec3 cart = polar_to_cartesian(orbit_radius*2, glm::radians(inclication_angle), glm::radians(rotation_angle));
		glm::vec3 moon_pos = earth_position + cart;
		rotation_angle += moon_rotation_speed * delta_time;

		while(inclication_angle > 360) {
			inclication_angle /= 360;
		}
		while(rotation_angle > 360) {
			rotation_angle /= 360;
		}

		// Render scene
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render GUI
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		ImGui::Begin("Settings");

		float rs[1] = { moon_rotation_speed };
 		if(ImGui::DragFloat("Rotation Speed", rs, 0.1f, 0.0f, 0.0f, "%.2f"))
			moon_rotation_speed = rs[0];
		float orbitr[1] = { orbit_radius };
		if(ImGui::DragFloat("Orbit Radius", orbitr, 0.1f, 0.0f, 0.0f, "%.2f"))
			orbit_radius = orbitr[0];
		float ia[1] = { inclication_angle };
		if(ImGui::DragFloat("Equator Angle", ia, 0.1f, 0.0f, 0.0f, "%.2f"))
			inclication_angle = ia[0];
		float ra[1] = { rotation_angle };
		if(ImGui::DragFloat("0 Meridian Angle", ra, 0.1f, 0.0f, 0.0f, "%.2f"))
			rotation_angle = ra[0];

		ImGui::End();
		
		// Render Earth
		earth_texture.bind();
		shader.bind();
		earth_va.bind();
		
		glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 model = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_WIDTH, 0.1f, 100.0f);
		model = glm::translate(model, earth_position);
		// Rotate so that north pose is at the top
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
		// Scale, since texcoords are inverted on the x axis
		model = glm::scale(model, glm::vec3(-1, 1, 1));
		
		shader.setMatrix4("projection", projection);
        shader.setMatrix4("view", view);
		shader.setMatrix4("model", model);

		glDrawElements(GL_TRIANGLES, earth_indices.size(), GL_UNSIGNED_INT, 0);

		// Render Moon
		moon_texture.bind();
		shader.bind();
		moon_va.bind();

		model = glm::mat4(1.0f);
		model = glm::translate(model, moon_pos);
		
		shader.setMatrix4("projection", projection);
        shader.setMatrix4("view", view);
		shader.setMatrix4("model", model);

		glDrawElements(GL_TRIANGLES, moon_indices.size(), GL_UNSIGNED_INT, 0);

		// Create the trajectory line
		std::vector<float> line_vertices;
		for(float i = rotation_angle; i < rotation_angle + 360; ++i) {
			glm::vec3 cart = polar_to_cartesian(orbit_radius*2, glm::radians(inclication_angle), glm::radians(i));
			glm::vec3 moon_pos = earth_position + cart;
			
			line_vertices.push_back(moon_pos.x);
			line_vertices.push_back(moon_pos.y);
			line_vertices.push_back(moon_pos.z);

			cart = polar_to_cartesian(orbit_radius*2, glm::radians(inclication_angle), glm::radians(i+1));
			moon_pos = earth_position + cart;
			
			line_vertices.push_back(moon_pos.x);
			line_vertices.push_back(moon_pos.y);
			line_vertices.push_back(moon_pos.z);
		}

		attribs = {
			VertexAttribute(AttributeType::VEC3, GL_FALSE)
		};
	
		vertices_size = line_vertices.size() * sizeof(float);
	
		VertexArray line_va;
		VertexBuffer line_vb;
		line_va.create();
		line_va.bind();

		line_vb.create(line_vertices.data(), vertices_size, attribs);

		line_va.addVertexBuffer(line_vb);
		
		line_shader.bind();
		line_va.bind();

		model = glm::mat4(1.0f);
		
		line_shader.setMatrix4("projection", projection);
        line_shader.setMatrix4("view", view);
		line_shader.setMatrix4("model", model);
		line_shader.setVec3("color", line_color);

		glDrawArrays(GL_LINES, 0, line_vertices.size());

		line_va.destroy();
		line_vb.destroy();
		
		ImGuiIO& io = ImGui::GetIO();
		// io.DisplaySize = ImVec2(window_width,
		// 						window_height);
		ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	earth_texture.destroy();
	earth_va.destroy();
	earth_vb.destroy();
	earth_ib.destroy();
	
	moon_texture.destroy();
	moon_va.destroy();
	moon_vb.destroy();
	moon_ib.destroy();
	
	line_shader.destroy();
	shader.destroy();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

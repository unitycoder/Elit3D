#include "m1Scene.h"

#include <GL/glew.h>
#include <SDL.h>

#include "Application.h"
#include "m1Input.h"
#include "m1Camera3D.h"

#include "m1Render3D.h"
#include "r1Shader.h"

#include "m1GUI.h"
#include "p1Scene.h"
#include "Viewport.h"

#include "m1Resources.h"
#include "r1Mesh.h"

// TEMP ====================================
#include "Object.h"
#include "c1Transform.h"
#include "c1Mesh.h"
#include "c1Material.h"

#include "r1Model.h"
//==========================================

#include "ExternalTools/MathGeoLib/include/Geometry/Ray.h"
#include "ExternalTools/MathGeoLib/include/Geometry/LineSegment.h"

#include "Logger.h"

#include "Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Scene::m1Scene(bool start_enabled) : Module("Scene", start_enabled)
{
}

m1Scene::~m1Scene()
{
}

bool m1Scene::Init(const nlohmann::json& node)
{
	return true;
}

bool m1Scene::Start()
{
	PROFILE_FUNCTION();
	GenerateGrid(); // TODO: Set grid in a shader

	panel_scene = App->gui->scene;

	vertices.size = 4;
	vertices.data = new float[vertices.size * 3];

	float width, height;
	width = height = 1;

	vertices.data[0] = -width;		vertices.data[1] =  -width;	vertices.data[2] = 0.f;
	vertices.data[3] =  -width;		vertices.data[4] =	 width;	vertices.data[5] = 0.f;
	vertices.data[6] =	width;		vertices.data[7] =	 width;	vertices.data[8] = 0.f;
	vertices.data[9] =	width;		vertices.data[10] = -width;	vertices.data[11] = 0.f;

	indices.size = 6;
	indices.data = new unsigned int[indices.size];

	indices.data[0] = 0u; indices.data[1] = 1u; indices.data[2] = 2u;
	indices.data[3] = 0u; indices.data[4] = 2u; indices.data[5] = 3u;

	texture.size = 4;
	texture.data = new float[texture.size * 2];
	memset(texture.data, 0.f, texture.size * 2 * sizeof(float));

	texture.data[0] = 0.f;		texture.data[1] = 0.f;
	texture.data[2] = 1.f;		texture.data[3] = 0.f;
	texture.data[4] = 1.f;		texture.data[5] = 1.f;
	texture.data[6] = 0.f;		texture.data[7] = 1.f;

	// VERTEX ARRAY OBJECT
	glGenVertexArrays(1, &VAOG);
	glBindVertexArray(VAOG);

	// VERTICES BUFFER
	glGenBuffers(1, &vertices.id);
	glBindBuffer(GL_ARRAY_BUFFER, vertices.id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size * 3, vertices.data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// TEXTURE COORDS BUFFER
	glGenBuffers(1, &texture.id);
	glBindBuffer(GL_ARRAY_BUFFER, texture.id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * texture.size, texture.data, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	// INDICES BUFFER
	glGenBuffers(1, &indices.id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size, indices.data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	((r1Model*)App->resources->Get(App->resources->Find("cubecat")))->CreateObject();

	return true;
}

void m1Scene::GenerateGrid()
{
	int width = 100;
	grid_vertex_size = (width + 1) * 4 * 3; // width + 1(for the middle line) * 4(4 points by line) * 3(3 numbers per point)
	float* g = new float[grid_vertex_size];

	float y = 0.f;
	for (int i = 0; i <= width * 3 * 4; i += 12)
	{
		g[i] = i / 12;
		g[i + 1] = y;
		g[i + 2] = 0.f;

		g[i + 3] = i / 12;
		g[i + 4] = y;
		g[i + 5] = width;

		g[i + 6] = 0.f;
		g[i + 7] = y;
		g[i + 8] = i / 12;

		g[i + 9] = width;
		g[i + 10] = y;
		g[i + 11] = i / 12;
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &grid);
	glBindBuffer(GL_ARRAY_BUFFER, grid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * grid_vertex_size, g, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	delete[] g;
}

UpdateStatus m1Scene::Update()
{
	PROFILE_FUNCTION();
	if (App->input->IsKeyDown(SDL_SCANCODE_ESCAPE))
		return UpdateStatus::UPDATE_STOP;

	App->gui->scene->viewport->Begin();

	glBindVertexArray(VAOG);
	glBindBuffer(GL_ARRAY_BUFFER, vertices.id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.id);

	static auto shader1 = App->render->GetShader("grid");
	shader1->Use();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	static auto shader = App->render->GetShader("default");
	shader->Use();
	shader->SetMat4("model", float4x4::identity);

	if (draw_grid)
		DrawGrid();

	if(panel_scene->IsOnHover())
		if (App->input->IsMouseButtonDown(1)) {
			LOG("CLICK");
			/*auto l = App->camera->frustum.UnProjectLineSegment(0, 0);
			Ray r(l);*/

			
		}

	App->gui->scene->viewport->End();
	
	return UpdateStatus::UPDATE_CONTINUE;
}

void m1Scene::DrawGrid()
{
	glLineWidth(1.5f);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, grid);
	glDrawArrays(GL_LINES, 0, grid_vertex_size);
	glLineWidth(1.f);
}

bool m1Scene::CleanUp()
{
	PROFILE_FUNCTION();

	delete[] this->vertices.data;
	delete[] this->texture.data;
	delete[] this->indices.data;

	return true;
}

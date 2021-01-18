#include "Panels/p1Objects.h"
#include "Core/Application.h"
#include "Modules/m1Objects.h"
#include "Objects/Object.h"

#include "Tools/System/Logger.h"

p1Objects::p1Objects(bool start_active, bool appear_mainmenubar, bool can_close)
	: Panel("Objects", start_active, appear_mainmenubar, can_close, ICON_FA_SITEMAP)
{
}

p1Objects::~p1Objects()
{
}

void p1Objects::Update()
{
	if (App->objects->layer_root_selected != nullptr) {
		for (auto i = App->objects->layer_root_selected->children.begin(); i != App->objects->layer_root_selected->children.end(); ++i)
			TreeNode(*i);
	}
}

void p1Objects::TreeNode(Object* obj)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (App->objects->selected == obj)
		flags |= ImGuiTreeNodeFlags_Selected;
	if (obj->children.empty())
		flags |= ImGuiTreeNodeFlags_Leaf;

	bool tree = ImGui::TreeNodeEx(obj->GetName(), flags);
	ImGui::PushID(obj);
	if (ImGui::IsItemClicked() && App->objects->selected != obj)
		App->objects->selected = obj;

	if (tree) {
		for (auto i = obj->children.begin(); i != obj->children.end(); ++i) {
			TreeNode(*i);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}

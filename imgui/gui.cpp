//
//	gui.cpp | Finn Le Var
//
#include "gui.h"

#include "imgui/imgui.h"
#include "hotrod/dllman.h"

//
// hotrod gui
//
namespace gui
{
	void hotrod_window()
	{
		ImGui::Begin("Hot Loader");

		// show loaded module count
		ImGui::Text("Loaded Modules: %zu", g_dll.count());
		ImGui::Separator();

		// list all loaded DLLs
		for (const auto& name : g_dll.get_all_names())
		{
			auto* dll = g_dll.get(name);

			if (!dll)
				continue;

			// use tree node for each module
			if (ImGui::TreeNode(name.c_str()))
			{
				// show path
				ImGui::Text("Path: %s", dll->m_path.c_str());

				// show status
				if (dll->loaded())
				{
					ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Status: Loaded");

					// show module info
					if (dll->m_ctx.name)
						ImGui::Text("Module: %s", dll->m_ctx.name);
					if (dll->m_ctx.author)
						ImGui::Text("Author: %s", dll->m_ctx.author);
					if (dll->m_ctx.desc)
						ImGui::Text("Description: %s", dll->m_ctx.desc);


					// force reload button
					if (ImGui::Button("Update"))
						dll->tick();

					ImGui::SameLine();

					// force reload button
					if (ImGui::Button("Dump"))
						dll->dump();

					ImGui::SameLine();

					// unload button (prevents auto-reload until file changes)
					if (ImGui::Button("Unload"))
						dll->unload();

					// so that the force reload is on the same line
					ImGui::SameLine();
				}
				else
				{
					ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Status: Not Loaded");
				}

				// force reload button
				if (ImGui::Button("Force Reload"))
					dll->reload(true);

				ImGui::TreePop();
			}
		}

		ImGui::Separator();

		// scan for new DLLs button
		if (ImGui::Button("Scan for New DLLs"))
			g_dll.find_and_load();

		ImGui::End();
	}
}

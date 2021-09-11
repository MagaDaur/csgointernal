#include "Menu.h"
#include "Settings.h"
#include "GUI/ImGui/imgui.h"
#include "GUI/ImGui/imgui_internal.h"
#include "GUI/ImGui/dx9/imgui_dx9.h"
Menu g_Menu;

void Menu::Menu_Manipulation() {
	static bool _pressed = true;

	if (!_pressed && GetAsyncKeyState(VK_INSERT))
		_pressed = true;
	else if (_pressed && !GetAsyncKeyState(VK_INSERT))
	{
		_pressed = false;
		g_Settings.bMenuOpened = !g_Settings.bMenuOpened;
	}
}

void Menu::Render() 
{
	static const char* tabs[] = {
				"rage",
				"legit",
				"visuals",
				"misc",
	};

	const char* config[] =
	{
		"slot1",
		"slot2",
		"slot3",
		"slot4",
		"slot5",
		"slot6"
	};

	const char* Material[] =
	{
		"Normal",
		"Flat",
		"Glow"
	};

		ImGui::SetNextWindowSize(ImVec2(600, 450));
		ImGui::Begin("mysterious", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar); {
		{
			ImGui::SetColorEditOptions(ImGuiColorEditFlags_NoInputs | ImGuiWindowFlags_ShowBorders | ImGuiColorEditFlags_PickerHueWheel);

			static int tab_count = 0;

			for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
			{

				if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs) - 9, 0)))
					tab_count = i;

				if (i < IM_ARRAYSIZE(tabs) - 1)
					ImGui::SameLine();
			}
			ImGui::Separator();

			switch (tab_count) {
			case 0:
			{
				ImGui::Columns(2, NULL, false);
				{
					ImGui::Text("Aimbot"); ImGui::NextColumn(); ImGui::Text("Anti-Aim"); ImGui::NextColumn();
					ImGui::Checkbox(("Enabled"), &g_Settings.RageEnable);
					ImGui::SliderInt("Hit Chance", &g_Settings.HitChance, 0, 100, "%.0f");
					ImGui::SliderInt("Min Damage", &g_Settings.MinDamage, 0, 100, "%.0f");
					ImGui::Checkbox(("Double Tap"), &g_Settings.DoubleTap);

				}
				ImGui::NextColumn();
				{

					ImGui::Checkbox(("Enable"), &g_Settings.AntiAimEnable);
					ImGui::Checkbox(("Desync"), &g_Settings.DesyncEnable);
					ImGui::SliderInt("Fake Amount", &g_Settings.FakeAmount, 0, 100, "%.0f");
					ImGui::SliderInt("Fake Lag", &g_Settings.FakeLag, 1, 14, "%.0f");
				}
			}
			break;
			case 1:
			{
				ImGui::Checkbox(("Enabled"), &g_Settings.EnableLegit);
				ImGui::SliderInt("FOV", &g_Settings.LegitFOV, 0, 20, "%.0f");
				ImGui::Checkbox(("AutoFire"), &g_Settings.LegitAutoFire);
				ImGui::Checkbox(("Backtrack"), &g_Settings.LegitBacktrack);
			}

			break;
			case 2:
				ImGui::Columns(3, NULL, false);
				{
					ImGui::Text("Visuals 1"); ImGui::NextColumn(); ImGui::Text("Visuals 2"); ImGui::NextColumn(); ImGui::Text("Chams"); ImGui::NextColumn();
					ImGui::Checkbox(("Enabled"), &g_Settings.EspEnable);
					ImGui::Checkbox(("Box"), &g_Settings.BoxEsp);
					ImGui::SameLine();
					ImGui::ColorEdit3("###boxclr ", g_Settings.boxcolor);
					ImGui::Checkbox(("Health"), &g_Settings.HealthEsp);	
					ImGui::SameLine();
					ImGui::ColorEdit3("###healtclr ", g_Settings.healthcolor);
					ImGui::Checkbox(("Name"), &g_Settings.NameEsp);
					ImGui::SameLine();
					ImGui::ColorEdit3("###nameclr ", g_Settings.namecolor);
					ImGui::Checkbox(("Weapon"), &g_Settings.WeaponEsp);
					ImGui::SameLine();
					ImGui::ColorEdit3("###weaponclr ", g_Settings.weaponcolor);

				}
				ImGui::NextColumn();
				{
					ImGui::Checkbox(("Indicators"), &g_Settings.Indicators);
					ImGui::Checkbox(("NoScope"), &g_Settings.NoScope);
				}
				ImGui::NextColumn();
				{
					ImGui::Checkbox(("Enable"), &g_Settings.NoScope);
					ImGui::Combo("Material", &g_Settings.ChamsMaterial, Material, ARRAYSIZE(Material));
					ImGui::Checkbox(("Visible"), &g_Settings.VisibleChams);
					ImGui::Checkbox(("In Visible"), &g_Settings.InVisibleChams);
				}
			break;
			case 3:
			{
				ImGui::Columns(2, NULL, false);
				{
					if (ImGui::Button("Unload")) g_Settings.bCheatActive = false;
				}
			break;
			}
		}
		ImGui::End();
	}
	ImGui::Render();
	}
}

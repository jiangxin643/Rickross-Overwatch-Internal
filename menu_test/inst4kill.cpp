#define WIN32_LEAN_AND_MEAN
#define VC_EXTRA_LEAN

#include "draw.h"
#include "menu.h"
#include "../dxapi/c_menu.h"
#include "base.h"

char* onOff[]	 = { "Off", "On" };
char* box_type[] = { "normal", "cornered", "3d"};
char* aimBone[]	= { "Torso", "Upper Torso", "Head", "Neck", "Nuts" };
char* aimAngle[]	= { "360", "180", "90", "45", "30", "15" };
char* multiplier[]	= { "Off", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10" };
char* aimTime[]	= { "Instant", "100ms", "200ms", "300ms", "400ms", "500ms", "600ms", "700ms", "800ms", "900ms", "1000ms" };
char* keys[]		= { "LClick", "MClick", "RClick", "Alt", "Ctrl", "Shift" };

string moduleList[] = { "d3d9.dll" };

void InitializeMenu()
{
	g_tMenu->Header("BP Alpha");
	
	g_tMenu->AddTab("Visual");
	//OSD enabled
	g_tMenu->AddItem("esp enabled", onOff, &(g_cvars_misc->esp_active));
	g_tMenu->AddItem("osd enabled", onOff, &(g_cvars_misc->esp_osd));
	g_tMenu->AddItem("box", box_type , &(g_cvars_misc->esp_box), 3);
	g_tMenu->AddItem("class", onOff);
	g_tMenu->AddItem("health bar", onOff);

	g_tMenu->AddTab("Aimbot");
	g_tMenu->AddItem("Aimbot", onOff);
	g_tMenu->AddItem("Aim Key", keys, 6);
	g_tMenu->AddItem("Aim Bone", aimBone, 5);
	g_tMenu->AddItem("Auto Switch", onOff);
	g_tMenu->AddItem("Aim Angle", aimAngle, 6);
	g_tMenu->AddItem("Smooth Aim", aimTime, 11);
	g_tMenu->AddItem("Aim Thru Walls", onOff);
	g_tMenu->AddItem("Friendly Fire", onOff);
	g_tMenu->AddItem("Off After Kill", onOff);
	
	g_tMenu->AddTab("Smooth");
	g_tMenu->AddItem("Off After Kill", onOff);

	g_tMenu->AddTab("Stats");
	g_tMenu->AddItem("Recoil Control", multiplier, 11);
	g_tMenu->AddItem("Spread Control", multiplier, 11);
	g_tMenu->AddItem("Rapid Fire", onOff);
	g_tMenu->AddItem("Far Pickup", onOff);
	g_tMenu->AddItem("Fly Mode", onOff);
	g_tMenu->AddItem("Super Speed", multiplier, 11);
	g_tMenu->AddItem("Super Jump", multiplier, 11);

	g_tMenu->AddTab("Misc");
	g_tMenu->AddItem("Super Pickup", onOff);
	g_tMenu->AddItem("Free Run", onOff);
	g_tMenu->AddItem("Head Glitch", onOff);
	g_tMenu->AddItem("Chat Spam", onOff);
	g_tMenu->AddItem("Voice Spam", onOff);
	g_tMenu->AddItem("Ghost Mode", onOff);
	g_tMenu->AddItem("Glitcher Mode", onOff);
	g_tMenu->AddItem("Tele Kill", onOff);
}
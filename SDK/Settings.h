#pragma once
#include <filesystem>
#include <fstream>
#include <map>
#include "Utils/GlobalVars.h"

class Settings
{
public:
    bool  bCheatActive = true;
    bool  bMenuOpened  = true;
    int ChosenConfig = 0;

    //ESP

    bool EspEnable = 0;
    bool BoxEsp = false;
    bool HealthEsp = false;
    bool NameEsp = false;
    bool WeaponEsp = false;
	float boxcolor[3] = { 1.f, 1.f, 1.f };
	float healthcolor[3] = { 1.f, 1.f, 1.f };
	float namecolor[3] = { 1.f, 1.f, 1.f };
	float weaponcolor[3] = { 1.f, 1.f, 1.f };
    bool Indicators = false;
    bool NoScope = false;

    //RAGE

    bool RageEnable = false;
    bool DoubleTap = false;
    int MinDamage = 0.0f;
    int HitChance = 0.0f;
    bool AntiAimEnable = false;
    bool DesyncEnable = false;
    int FakeLag = 1;
    int FakeAmount = 0;

    //MISC



    //CHAMS

    bool EnableChams = false;
    int ChamsMaterial = 0;
    bool VisibleChams = false;
    bool InVisibleChams = false;
    Color VisibleChamsColor;
    Color InVisibleChamsColor;

    //LEGIT

    bool EnableLegit = false;
    int LegitFOV = 0;
    bool LegitBacktrack = false;
    bool LegitAutoFire = false;



};

extern Settings g_Settings;


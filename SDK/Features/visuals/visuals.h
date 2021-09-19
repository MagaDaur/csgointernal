class CViewSetup;

struct EspInfo_t
{
    float alpha = 1.f;
    
    float health_scale = 1.f;
};

class CVisuals
{
public:
    void OnPaintTraverse();
    void OnOverrideView(CViewSetup*);
private:

    EspInfo_t EspInfo[64];
};

extern CVisuals g_Visuals;
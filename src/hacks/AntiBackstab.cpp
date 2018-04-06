/*
 * AntiBackstab.cpp
 *
 *  Created on: Apr 14, 2017
 *      Author: nullifiedcat
 */

#include "common.hpp"
#include "hack.hpp"

namespace hacks
{
namespace tf2
{
namespace antibackstab
{

static CatVar enabled(CV_SWITCH, "antibackstab", "0", "Enable",
                      "Main anti-backstab switch");
static CatVar distance(CV_FLOAT, "antibackstab_distance", "200", "Distance",
                       "Distance Until anti-backstab reacts");
static CatVar angle(CV_FLOAT, "antibackstab_angle", "150", "Detection Angle");
static CatVar sayno(CV_SWITCH, "antibackstab_nope", "0", "Nope!", "Memes");

void SayNope()
{
    static float last_say = 0.0f;
    if (g_GlobalVars->curtime < last_say)
        last_say = 0.0f;
    if (g_GlobalVars->curtime - last_say < 1.5f)
        return;
    hack::ExecuteCommand("voicemenu 0 7");
    last_say = g_GlobalVars->curtime;
}
float GetAngle(CachedEntity *spy)
{
    float yaw, yaw2, anglediff;
    Vector diff;
    yaw             = g_pLocalPlayer->v_OrigViewangles.y;
    const Vector &A = LOCAL_E->m_vecOrigin;
    const Vector &B = spy->m_vecOrigin;
    diff            = (A - B);
    yaw2            = acos(diff.x / diff.Length()) * 180.0f / PI;
    if (diff.y < 0)
        yaw2  = -yaw2;
    anglediff = yaw - yaw2;
    if (anglediff > 180)
        anglediff -= 360;
    if (anglediff < -180)
        anglediff += 360;
    // logging::Info("Angle: %.2f | %.2f | %.2f | %.2f", yaw, yaw2, anglediff,
    // yaw - yaw2);
    return anglediff;
}
CachedEntity *ClosestSpy()
{
    CachedEntity *closest, *ent;
    float closest_dist, dist;

    closest      = nullptr;
    closest_dist = 0.0f;

    for (int i = 1; i < 32 && i < g_IEntityList->GetHighestEntityIndex(); i++)
    {
        ent = ENTITY(i);
        if (CE_BAD(ent))
            continue;
        if (CE_BYTE(ent, netvar.iLifeState))
            continue;
        if (CE_INT(ent, netvar.iClass) != tf_class::tf_spy)
            continue;
        if (CE_INT(ent, netvar.iTeamNum) == g_pLocalPlayer->team)
            continue;
        if (IsPlayerInvisible(ent))
            continue;
        dist = ent->m_flDistance;
        if (fabs(GetAngle(ent)) > (float) angle)
        {
            break;
            // logging::Info("Backstab???");
        }
        if (dist < (float) distance && (dist < closest_dist || !closest_dist))
        {
            closest_dist = dist;
            closest      = ent;
        }
    }
    return closest;
}
static CatVar test(CV_FLOAT, "a_t", "180", "");
static CatVar test2(CV_SWITCH, "a_t2", "0", "");
void CreateMove()
{
    CachedEntity *spy;
    Vector diff;
    float yaw2, resultangle;

    if (!enabled)
        return;
    spy = ClosestSpy();
    static float backup;
    if (spy && CE_GOOD(spy))
    {
        ConVar *var = g_ICvar->FindVar("cl_pitchdown");
        if (!var)
            return;
        if (!backup)
        	backup = var->GetFloat();
        var->SetValue(140.0f);
        g_pUserCmd->viewangles.x = 140.0f;
        g_pLocalPlayer->bUseSilentAngles = true;
        if (sayno)
            SayNope();
    }
    else if (!spy || CE_BAD(spy)) {
    	ConVar *var = g_ICvar->FindVar("cl_pitchdown");
    	var->SetValue(backup);
    }
}
}
}
}

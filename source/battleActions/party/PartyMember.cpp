#include "PartyMember.h"
#include "../skills/BattleCalcs.h"

float PartyMember::calculateBaseDamage(BattleParticipant& defender, Skill& skill)
{
    u32 atk = BattleCalcs::getAtk(curPersona->battleStats, skill);
    float levelDifference = BattleCalcs::getLevelDifference(lv, defender.lv);
    float affinityMtp = BattleCalcs::getAffinityMtp(*defender.getBattleStats(), skill);
    u32 movePower = (skill.skillType == SkillType::RegularAttack) ? (weapon.weaponPower / 2) : skill.movePower;
    return floor(sqrt((float)(movePower * 15 * atk) / defender.getBattleStats()->en) * 2 * levelDifference *
                 affinityMtp);
}

float PartyMember::getTeamMultiplier()
{
    return 1.0f;
}

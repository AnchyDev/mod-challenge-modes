#ifndef AZEROTHCORE_CHALLENGEMODES_H
#define AZEROTHCORE_CHALLENGEMODES_H

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "GameObjectAI.h"
#include <map>


enum ChallengeModeSettings
{
    SETTING_HARDCORE           = 0,
    SETTING_SEMI_HARDCORE      = 1,
    SETTING_SELF_CRAFTED       = 2,
    SETTING_ITEM_QUALITY_LEVEL = 3,
    SETTING_SLOW_XP_GAIN       = 4,
    SETTING_VERY_SLOW_XP_GAIN  = 5,
    SETTING_QUEST_XP_ONLY      = 6,
    SETTING_IRON_MAN           = 7,
    SETTING_MARK_DIRTY         = 8,
    SETTING_MODE_MAX           = 8
};

enum AllowedProfessions
{
    RUNEFORGING    = 53428,
    POISONS        = 2842,
    BEAST_TRAINING = 5149
};



class ChallengeModes
{
public:
    static ChallengeModes* instance();

    bool challengesEnabled, hardcoreEnable, semiHardcoreEnable, selfCraftedEnable, itemQualityLevelEnable, slowXpGainEnable, verySlowXpGainEnable, questXpOnlyEnable, ironManEnable;
    float hardcoreXpBonus, semiHardcoreXpBonus, selfCraftedXpBonus, itemQualityLevelXpBonus, questXpOnlyXpBonus;
    std::unordered_map<uint8, uint32> hardcoreTitleRewards, semiHardcoreTitleRewards, selfCraftedTitleRewards, itemQualityLevelTitleRewards, slowXpGainTitleRewards, verySlowXpGainTitleRewards, questXpOnlyTitleRewards, ironManTitleRewards;
    std::unordered_map<uint8, uint32> hardcoreItemRewards, semiHardcoreItemRewards, selfCraftedItemRewards, itemQualityLevelItemRewards, slowXpGainItemRewards, verySlowXpGainItemRewards, questXpOnlyItemRewards, ironManItemRewards;
    std::unordered_map<uint8, uint32> hardcoreTalentRewards, semiHardcoreTalentRewards, selfCraftedTalentRewards, itemQualityLevelTalentRewards, slowXpGainTalentRewards, verySlowXpGainTalentRewards, questXpOnlyTalentRewards, ironManTalentRewards;

    std::unordered_map<std::string, std::unordered_map<uint8, uint32>*> rewardConfigMap =
            {
                    { "Hardcore.TitleRewards",                &hardcoreTitleRewards          },
                    { "SemiHardcore.TitleRewards",            &semiHardcoreTitleRewards      },
                    { "SelfCrafted.TitleRewards",             &selfCraftedTitleRewards       },
                    { "ItemQualityLevel.TitleRewards",        &itemQualityLevelTitleRewards  },
                    { "SlowXpGain.TitleRewards",              &slowXpGainTitleRewards        },
                    { "VerySlowXpGain.TitleRewards",          &verySlowXpGainTitleRewards    },
                    { "QuestXpOnly.TitleRewards",             &questXpOnlyTitleRewards       },
                    { "IronMan.TitleRewards",                 &ironManTitleRewards           },

                    { "Hardcore.TalentRewards",               &hardcoreTalentRewards         },
                    { "SemiHardcore.TalentRewards",           &semiHardcoreTalentRewards     },
                    { "SelfCrafted.TalentRewards",            &selfCraftedTalentRewards      },
                    { "ItemQualityLevel.TalentRewards",       &itemQualityLevelTalentRewards },
                    { "SlowXpGain.TalentRewards",             &slowXpGainTalentRewards       },
                    { "VerySlowXpGain.TalentRewards",         &verySlowXpGainTalentRewards   },
                    { "QuestXpOnly.TalentRewards",            &questXpOnlyTalentRewards      },
                    { "IronMan.TalentRewards",                &ironManTalentRewards          },

                    { "Hardcore.ItemRewards",                 &hardcoreItemRewards           },
                    { "SemiHardcore.ItemRewards",             &semiHardcoreItemRewards       },
                    { "SelfCrafted.ItemRewards",              &selfCraftedItemRewards        },
                    { "ItemQualityLevel.ItemRewards",         &itemQualityLevelItemRewards   },
                    { "SlowXpGain.ItemRewards",               &slowXpGainItemRewards         },
                    { "VerySlowXpGain.ItemRewards",           &verySlowXpGainItemRewards     },
                    { "QuestXpOnly.ItemRewards",              &questXpOnlyItemRewards        },
                    { "IronMan.ItemRewards",                  &ironManItemRewards            },
            };

    [[nodiscard]] bool enabled() const { return challengesEnabled; }
    [[nodiscard]] bool challengeEnabled(ChallengeModeSettings setting) const;
    [[nodiscard]] float getXpBonusForChallenge(ChallengeModeSettings setting) const;
    bool challengeEnabledForPlayer(ChallengeModeSettings setting, Player* player) const;
    std::string GetChallengeNameFromEnum(uint8 value);
    void TryMarkDirty(Player* player);
    [[nodiscard]] const std::unordered_map<uint8, uint32> *getTitleMapForChallenge(ChallengeModeSettings setting) const;
    [[nodiscard]] const std::unordered_map<uint8, uint32> *getTalentMapForChallenge(ChallengeModeSettings setting) const;
    [[nodiscard]] const std::unordered_map<uint8, uint32> *getItemMapForChallenge(ChallengeModeSettings setting) const;
};

#define sChallengeModes ChallengeModes::instance()

#endif //AZEROTHCORE_CHALLENGEMODES_H

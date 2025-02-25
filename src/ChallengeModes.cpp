/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ChallengeModes.h"
#include "Tokenize.h"
#include "Player.h"

ChallengeModes* ChallengeModes::instance()
{
    static ChallengeModes instance;
    return &instance;
}

void ChallengeModes::TryMarkDirty(Player* player)
{
    if (!player)
    {
        return;
    }

    if (player->IsInWorld())
    {
        player->UpdatePlayerSetting("mod-challenge-modes", SETTING_MARK_DIRTY, 1);
    }
}

bool ChallengeModes::challengeEnabledForPlayer(ChallengeModeSettings setting, Player* player) const
{
    if (!enabled() || !challengeEnabled(setting))
    {
        return false;
    }
    return player->GetPlayerSetting("mod-challenge-modes", setting).value;
}

std::string ChallengeModes::GetChallengeNameFromEnum(uint8 value)
{
    switch (value)
    {
    case SETTING_HARDCORE:
        return "Hardcore";
    case SETTING_SEMI_HARDCORE:
        return "Semi-Hardcore";
    case SETTING_SELF_CRAFTED:
        return "Self-Crafted";
    case SETTING_ITEM_QUALITY_LEVEL:
        return "Low Quality Items";
    case SETTING_SLOW_XP_GAIN:
        return "Slow XP";
    case SETTING_VERY_SLOW_XP_GAIN:
        return "Very Slow XP";
    case SETTING_QUEST_XP_ONLY:
        return "Quest XP Only";
    case SETTING_IRON_MAN:
        return "Iron Man";
    }

    return "ERROR";
}

bool ChallengeModes::challengeEnabled(ChallengeModeSettings setting) const
{
    switch (setting)
    {
        case SETTING_HARDCORE:
            return hardcoreEnable;
        case SETTING_SEMI_HARDCORE:
            return semiHardcoreEnable;
        case SETTING_SELF_CRAFTED:
            return selfCraftedEnable;
        case SETTING_ITEM_QUALITY_LEVEL:
            return itemQualityLevelEnable;
        case SETTING_SLOW_XP_GAIN:
            return slowXpGainEnable;
        case SETTING_VERY_SLOW_XP_GAIN:
            return verySlowXpGainEnable;
        case SETTING_QUEST_XP_ONLY:
            return questXpOnlyEnable;
        case SETTING_IRON_MAN:
            return ironManEnable;
    }
    return false;
}

float ChallengeModes::getXpBonusForChallenge(ChallengeModeSettings setting) const
{
    switch (setting)
    {
        case SETTING_HARDCORE:
            return hardcoreXpBonus;
        case SETTING_SEMI_HARDCORE:
            return semiHardcoreXpBonus;
        case SETTING_SELF_CRAFTED:
            return selfCraftedXpBonus;
        case SETTING_ITEM_QUALITY_LEVEL:
            return itemQualityLevelXpBonus;
        case SETTING_SLOW_XP_GAIN:
            return 0.5f;
        case SETTING_VERY_SLOW_XP_GAIN:
            return 0.25f;
        case SETTING_QUEST_XP_ONLY:
            return questXpOnlyXpBonus;
        case SETTING_IRON_MAN:
            return 1;
    }
    return 1;
}

const std::unordered_map<uint8, uint32> *ChallengeModes::getTitleMapForChallenge(ChallengeModeSettings setting) const
{
    switch (setting)
    {
        case SETTING_HARDCORE:
            return &hardcoreTitleRewards;
        case SETTING_SEMI_HARDCORE:
            return &semiHardcoreTitleRewards;
        case SETTING_SELF_CRAFTED:
            return &selfCraftedTitleRewards;
        case SETTING_ITEM_QUALITY_LEVEL:
            return &itemQualityLevelTitleRewards;
        case SETTING_SLOW_XP_GAIN:
            return &slowXpGainTitleRewards;
        case SETTING_VERY_SLOW_XP_GAIN:
            return &verySlowXpGainTitleRewards;
        case SETTING_QUEST_XP_ONLY:
            return &questXpOnlyTitleRewards;
        case SETTING_IRON_MAN:
            return &ironManTitleRewards;
    }
    return {};
}

const std::unordered_map<uint8, uint32> *ChallengeModes::getTalentMapForChallenge(ChallengeModeSettings setting) const
{
    switch (setting)
    {
        case SETTING_HARDCORE:
            return &hardcoreTalentRewards;
        case SETTING_SEMI_HARDCORE:
            return &semiHardcoreTalentRewards;
        case SETTING_SELF_CRAFTED:
            return &selfCraftedTalentRewards;
        case SETTING_ITEM_QUALITY_LEVEL:
            return &itemQualityLevelTalentRewards;
        case SETTING_SLOW_XP_GAIN:
            return &slowXpGainTalentRewards;
        case SETTING_VERY_SLOW_XP_GAIN:
            return &verySlowXpGainTalentRewards;
        case SETTING_QUEST_XP_ONLY:
            return &questXpOnlyTalentRewards;
        case SETTING_IRON_MAN:
            return &ironManTalentRewards;
    }
    return {};
}

const std::unordered_map<uint8, uint32> *ChallengeModes::getItemMapForChallenge(ChallengeModeSettings setting) const
{
    switch (setting)
    {
        case SETTING_HARDCORE:
            return &hardcoreItemRewards;
        case SETTING_SEMI_HARDCORE:
            return &semiHardcoreItemRewards;
        case SETTING_SELF_CRAFTED:
            return &selfCraftedItemRewards;
        case SETTING_ITEM_QUALITY_LEVEL:
            return &itemQualityLevelItemRewards;
        case SETTING_SLOW_XP_GAIN:
            return &slowXpGainItemRewards;
        case SETTING_VERY_SLOW_XP_GAIN:
            return &verySlowXpGainItemRewards;
        case SETTING_QUEST_XP_ONLY:
            return &questXpOnlyItemRewards;
        case SETTING_IRON_MAN:
            return &ironManItemRewards;
    }
    return {};
}

class ChallengeModes_WorldScript : public WorldScript
{
public:
    ChallengeModes_WorldScript()
        : WorldScript("ChallengeModes_WorldScript")
    {}

    void OnBeforeConfigLoad(bool /*reload*/) override
    {
        LoadConfig();
    }

private:
    static void LoadStringToMap(std::unordered_map<uint8, uint32> &mapToLoad, const std::string &configString)
    {
        std::string delimitedValue;
        std::stringstream configIdStream;

        configIdStream.str(configString);
        // Process each config ID in the string, delimited by the comma - "," and then space " "
        while (std::getline(configIdStream, delimitedValue, ','))
        {
            std::string pairOne, pairTwo;
            std::stringstream configPairStream(delimitedValue);
            configPairStream>>pairOne>>pairTwo;
            auto configLevel = atoi(pairOne.c_str());
            auto rewardValue = atoi(pairTwo.c_str());
            mapToLoad[configLevel] = rewardValue;
        }
    }

    static void LoadConfig()
    {
        sChallengeModes->challengesEnabled = sConfigMgr->GetOption<bool>("ChallengeModes.Enable", false);
        if (sChallengeModes->enabled())
        {
            for (auto& [confName, rewardMap] : sChallengeModes->rewardConfigMap)
            {
                rewardMap->clear();
                LoadStringToMap(*rewardMap, sConfigMgr->GetOption<std::string>(confName, ""));
            }

            sChallengeModes->hardcoreEnable          = sConfigMgr->GetOption<bool>("Hardcore.Enable", true);
            sChallengeModes->semiHardcoreEnable      = sConfigMgr->GetOption<bool>("SemiHardcore.Enable", true);
            sChallengeModes->selfCraftedEnable       = sConfigMgr->GetOption<bool>("SelfCrafted.Enable", true);
            sChallengeModes->itemQualityLevelEnable  = sConfigMgr->GetOption<bool>("ItemQualityLevel.Enable", true);
            sChallengeModes->slowXpGainEnable        = sConfigMgr->GetOption<bool>("SlowXpGain.Enable", true);
            sChallengeModes->verySlowXpGainEnable    = sConfigMgr->GetOption<bool>("VerySlowXpGain.Enable", true);
            sChallengeModes->questXpOnlyEnable       = sConfigMgr->GetOption<bool>("QuestXpOnly.Enable", true);
            sChallengeModes->ironManEnable           = sConfigMgr->GetOption<bool>("IronMan.Enable", true);

            sChallengeModes->hardcoreXpBonus         = sConfigMgr->GetOption<float>("Hardcore.XPMultiplier", 1.0f);
            sChallengeModes->semiHardcoreXpBonus     = sConfigMgr->GetOption<float>("SemiHardcore.XPMultiplier", 1.0f);
            sChallengeModes->selfCraftedXpBonus      = sConfigMgr->GetOption<float>("SelfCrafted.XPMultiplier", 1.0f);
            sChallengeModes->itemQualityLevelXpBonus = sConfigMgr->GetOption<float>("ItemQualityLevel.XPMultiplier", 1.0f);
            sChallengeModes->questXpOnlyXpBonus      = sConfigMgr->GetOption<float>("QuestXpOnly.XPMultiplier", 1.0f);
        }
    }
};

class ChallengeMode : public PlayerScript
{
public:
    explicit ChallengeMode(const char *scriptName,
                           ChallengeModeSettings settingName)
            : PlayerScript(scriptName), settingName(settingName)
    { }

    static bool mapContainsKey(const std::unordered_map<uint8, uint32>* mapToCheck, uint8 key)
    {
        return (mapToCheck->find(key) != mapToCheck->end());
    }

    void OnGiveXP(Player* player, uint32& amount, Unit* /*victim*/) override
    {
        sChallengeModes->TryMarkDirty(player);
        if (!sChallengeModes->challengeEnabledForPlayer(settingName, player))
        {
            return;
        }
        amount *= sChallengeModes->getXpBonusForChallenge(settingName);
    }

    void OnLevelChanged(Player* player, uint8 /*oldlevel*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(settingName, player))
        {
            return;
        }
        const std::unordered_map<uint8, uint32> *titleRewardMap = sChallengeModes->getTitleMapForChallenge(settingName);
        const std::unordered_map<uint8, uint32> *talentRewardMap = sChallengeModes->getTalentMapForChallenge(settingName);
        const std::unordered_map<uint8, uint32> *itemRewardMap = sChallengeModes->getItemMapForChallenge(settingName);
        uint8 level = player->GetLevel();

        // Disable modes at 80
        if (level == 80)
        {
            player->UpdatePlayerSetting("mod-challenge-modes", settingName, 0);
        }

        if (mapContainsKey(titleRewardMap, level))
        {
            CharTitlesEntry const* titleInfo = sCharTitlesStore.LookupEntry(titleRewardMap->at(level));
            if (!titleInfo)
            {
                LOG_ERROR("mod-challenge-modes", "Invalid title ID {}!", titleRewardMap->at(level));
                return;
            }
            ChatHandler handler(player->GetSession());
            std::string tNameLink = handler.GetNameLink(player);
            std::string titleNameStr = Acore::StringFormat(player->getGender() == GENDER_MALE ? titleInfo->nameMale[handler.GetSessionDbcLocale()] : titleInfo->nameFemale[handler.GetSessionDbcLocale()], player->GetName());
            player->SetTitle(titleInfo);
        }
        if (mapContainsKey(talentRewardMap, level))
        {
            player->RewardExtraBonusTalentPoints(talentRewardMap->at(level));
        }
        if (mapContainsKey(itemRewardMap, level))
        {
            // Mail item to player
            uint32 itemEntry = itemRewardMap->at(level);
            player->SendItemRetrievalMail({ { itemEntry, 1 } });
        }
    }

    PlayerSettingMap* GetPlayerSettingsFromDB(ObjectGuid guid)
    {
        PlayerSettingMap* settingMap = new PlayerSettingMap();

        auto result = CharacterDatabase.Query("SELECT source, data FROM character_settings WHERE guid = {}", guid.GetRawValue());

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();

                std::string source = fields[0].Get<std::string>();
                std::string data = fields[1].Get<std::string>();

                std::vector<std::string_view> tokens = Acore::Tokenize(data, ' ', false);

                PlayerSettingVector setting;
                setting.resize(tokens.size());

                uint32 count = 0;

                for (auto token : tokens)
                {
                    if (token.empty())
                    {
                        continue;
                    }

                    PlayerSetting set;
                    set.value = Acore::StringTo<uint32>(token).value();
                    setting[count] = set;
                    ++count;
                }

                (*settingMap)[source] = setting;

            } while (result->NextRow());
        }

        return settingMap;
    }

    bool HasPlayerSetting(PlayerSettingMap* m_charSettingsMap, std::string source, uint8 index)
    {
        auto itr = m_charSettingsMap->find(source);

        if (itr == m_charSettingsMap->end())
        {
            return false;
        }

        PlayerSettingVector settingVector = itr->second;
        if (settingVector.size() < (uint8)(index + 1))
        {
            return false;
        }

        return settingVector.at(index).value == 1;
    }

    bool CanSendMail(Player* player, ObjectGuid receiverGUID, ObjectGuid /*mailbox*/, std::string& /*subject*/, std::string& /*body*/, uint32 /*money*/, uint32 /*COD*/, Item* /*item*/) override
    {
        if (!sChallengeModes->enabled())
        {
            return true;
        }

        auto targetPlayer = ObjectAccessor::FindPlayer(receiverGUID);

        // Load offline player settings.
        PlayerSettingMap* playerSettings = nullptr;
        if (!targetPlayer)
        {
            playerSettings = GetPlayerSettingsFromDB(receiverGUID);
        }

        if (sChallengeModes->challengeEnabled(SETTING_HARDCORE))
        {
            if ((targetPlayer && targetPlayer->GetPlayerSetting("mod-challenge-modes", SETTING_HARDCORE).value == 1) ||
                (playerSettings && HasPlayerSetting(playerSettings, "mod-challenge-modes", SETTING_HARDCORE)) /* Fallback to DB */)
            {
                ChatHandler(player->GetSession()).SendSysMessage("You can't send mail to hardcore players.");
                return false;
            }
        }

        if (sChallengeModes->challengeEnabled(SETTING_SELF_CRAFTED))
        {
            if ((targetPlayer && targetPlayer->GetPlayerSetting("mod-challenge-modes", SETTING_SELF_CRAFTED).value == 1) ||
                (playerSettings && HasPlayerSetting(playerSettings, "mod-challenge-modes", SETTING_SELF_CRAFTED)) /* Fallback to DB */)
            {
                ChatHandler(player->GetSession()).SendSysMessage("You can't send mail to self-crafted players.");
                return false;
            }
        }

        return true;
    }

private:
    ChallengeModeSettings settingName;
};

class ChallengeMiscScripts : public MiscScript
{
public:
    ChallengeMiscScripts() : MiscScript("ChallengeMiscScripts") { }
    bool CanSendAuctionHello(WorldSession const* session, ObjectGuid /*guid*/, Creature* /*creature*/) override
    {
        if (!session->GetPlayer())
        {
            return true;
        }

        auto player = session->GetPlayer();

        sChallengeModes->TryMarkDirty(player);

        auto isHardcore = player->GetPlayerSetting("mod-challenge-modes", SETTING_HARDCORE).value == 1;
        auto isSelfCrafted = player->GetPlayerSetting("mod-challenge-modes", SETTING_SELF_CRAFTED).value == 1;

        if (isHardcore)
        {
            ChatHandler(player->GetSession()).SendSysMessage("You cannot use the auction house in hardcore mode.");
            return false;
        }

        if (isSelfCrafted)
        {
            ChatHandler(player->GetSession()).SendSysMessage("You cannot use the auction house in self-crafted mode.");
            return false;
        }

        return true;
    }
};

class ChallengeMiscPlayerScripts : public PlayerScript
{
public:
    ChallengeMiscPlayerScripts() : PlayerScript("ChallengeMiscPlayerScripts") { }

    void OnLogin(Player* player) override
    {
        if (!player)
        {
            return;
        }

        std::stringstream ss;
        ss << "Challenge Modes Enabled: ";

        uint8 enabledCount = 0;
        for (uint8 i = 0; i < SETTING_MODE_MAX; ++i)
        {
            auto setting = player->GetPlayerSetting("mod-challenge-modes", i);
            if (setting.value == 1)
            {
                ss << sChallengeModes->GetChallengeNameFromEnum(i);
                ss << ", ";
                enabledCount++;
            }
        }

        if (enabledCount == 0)
        {
            return;
        }

        ChatHandler(player->GetSession()).SendSysMessage(ss.str());
    }

    void OnLootItem(Player* player, Item* /*item*/, uint32 /*count*/, ObjectGuid /*lootguid*/) override { sChallengeModes->TryMarkDirty(player); }
    void OnStoreNewItem(Player* player, Item* /*item*/, uint32 /*count*/) override { sChallengeModes->TryMarkDirty(player); }
    void OnCreateItem(Player* player, Item* /*item*/, uint32 /*count*/) override { sChallengeModes->TryMarkDirty(player); }
    void OnQuestRewardItem(Player* player, Item* /*item*/, uint32 /*count*/) override { sChallengeModes->TryMarkDirty(player); }
    void OnGroupRollRewardItem(Player* player, Item* /*item*/, uint32 /*count*/, RollVote /*voteType*/, Roll* /*roll*/) override { sChallengeModes->TryMarkDirty(player); }
    void OnMoneyChanged(Player* player, int32& /*amount*/) override { sChallengeModes->TryMarkDirty(player); }
    void OnAfterStoreOrEquipNewItem(Player* player, uint32 /*vendorslot*/, Item* /*item*/, uint8 /*count*/, uint8 /*bag*/, uint8 /*slot*/, ItemTemplate const* /*pProto*/, Creature* /*pVendor*/, VendorItem const* /*crItem*/, bool /*bStore*/) override { sChallengeModes->TryMarkDirty(player); }
    bool CanInitTrade(Player* player, Player* target) override { sChallengeModes->TryMarkDirty(player); sChallengeModes->TryMarkDirty(target); return true; }
};

class ChallengeGuildScripts : public GuildScript
{
public:
    ChallengeGuildScripts() : GuildScript("ChallengeGuildScripts") { }

    bool CanGuildSendBankList(Guild const* /*guild*/, WorldSession* session, uint8 /*tabId*/, bool /*sendAllSlots*/) override
    {
        if (!session)
        {
            return true;
        }

        if (!session->GetPlayer())
        {
            return true;
        }

        auto player = session->GetPlayer();

        sChallengeModes->TryMarkDirty(player);

        auto isHardcore = player->GetPlayerSetting("mod-challenge-modes", SETTING_HARDCORE).value == 1;
        auto isSelfCrafted = player->GetPlayerSetting("mod-challenge-modes", SETTING_SELF_CRAFTED).value == 1;

        if (isHardcore)
        {
            ChatHandler(player->GetSession()).SendSysMessage("You cannot use the guild bank in hardcore mode.");
            return false;
        }

        if (isSelfCrafted)
        {
            ChatHandler(player->GetSession()).SendSysMessage("You cannot use the guild bank in self-crafted mode.");
            return false;
        }

        return true;
    }
};

class ChallengeMode_Hardcore : public ChallengeMode
{
public:
    ChallengeMode_Hardcore() : ChallengeMode("ChallengeMode_Hardcore", SETTING_HARDCORE) {}

    void OnPlayerResurrect(Player* player, float /*restore_percent*/, bool /*applySickness*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_HARDCORE, player))
        {
            return;
        }
        // A better implementation is to not allow the resurrect but this will need a new hook added first
        player->KillPlayer();
    }

    void OnGiveXP(Player* player, uint32& amount, Unit* victim) override
    {
        ChallengeMode::OnGiveXP(player, amount, victim);
    }

    void OnLevelChanged(Player* player, uint8 oldlevel) override
    {
        ChallengeMode::OnLevelChanged(player, oldlevel);
    }

    bool CanInitTrade(Player* player, Player* target) override
    {
        if (sChallengeModes->challengeEnabledForPlayer(SETTING_HARDCORE, player))
        {
            ChatHandler(player->GetSession()).SendSysMessage("You cannot trade with other players while in hardcore mode.");

            return false;
        }

        if (sChallengeModes->challengeEnabledForPlayer(SETTING_HARDCORE, target))
        {
            ChatHandler(player->GetSession()).SendSysMessage("You cannot trade with players in hardcore mode.");

            return false;
        }

        return true;
    }
};

class ChallengeMode_SemiHardcore : public ChallengeMode
{
public:
    ChallengeMode_SemiHardcore() : ChallengeMode("ChallengeMode_SemiHardcore", SETTING_SEMI_HARDCORE) {}

    void OnPlayerKilledByCreature(Creature* /*killer*/, Player* player) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_SEMI_HARDCORE, player))
        {
            return;
        }
        for (uint8 i = 0; i < EQUIPMENT_SLOT_END; ++i)
        {
            if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            {
                if (pItem->GetTemplate() && !pItem->IsEquipped())
                    continue;
                uint8 slot = pItem->GetSlot();
                ChatHandler(player->GetSession()).PSendSysMessage("|cffDA70D6You have lost your |cffffffff|Hitem:%d:0:0:0:0:0:0:0:0|h[%s]|h|r", pItem->GetEntry(), pItem->GetTemplate()->Name1.c_str());
                player->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);
            }
        }
        player->SetMoney(0);
    }

    void OnGiveXP(Player* player, uint32& amount, Unit* victim) override
    {
        ChallengeMode::OnGiveXP(player, amount, victim);
    }

    void OnLevelChanged(Player* player, uint8 oldlevel) override
    {
        ChallengeMode::OnLevelChanged(player, oldlevel);
    }
};

class ChallengeMode_SelfCrafted : public ChallengeMode
{
public:
    ChallengeMode_SelfCrafted() : ChallengeMode("ChallengeMode_SelfCrafted", SETTING_SELF_CRAFTED) {}

    bool CanEquipItem(Player* player, uint8 /*slot*/, uint16& /*dest*/, Item* pItem, bool /*swap*/, bool /*not_loading*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_SELF_CRAFTED, player))
        {
            return true;
        }

        // Allow fishing poles to be equipped since you cannot craft them.
        if (pItem->GetTemplate()->Class == ITEM_CLASS_WEAPON &&
            pItem->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_FISHING_POLE)
        {
            return true;
        }

        if (!pItem->GetTemplate()->HasSignature())
        {
            return false;
        }
        return pItem->GetGuidValue(ITEM_FIELD_CREATOR) == player->GetGUID();
    }

    void OnGiveXP(Player* player, uint32& amount, Unit* victim) override
    {
        ChallengeMode::OnGiveXP(player, amount, victim);
    }

    void OnLevelChanged(Player* player, uint8 oldlevel) override
    {
        ChallengeMode::OnLevelChanged(player, oldlevel);
    }

    bool CanInitTrade(Player* player, Player* target) override
    {
        if (sChallengeModes->challengeEnabledForPlayer(SETTING_SELF_CRAFTED, player))
        {
            ChatHandler(player->GetSession()).SendSysMessage("You cannot trade with other players while in self-crafted mode.");

            return false;
        }

        if (sChallengeModes->challengeEnabledForPlayer(SETTING_SELF_CRAFTED, target))
        {
            ChatHandler(player->GetSession()).SendSysMessage("You cannot trade with players in self-crafted mode.");

            return false;
        }

        return true;
    }
};

class ChallengeMode_ItemQualityLevel : public ChallengeMode
{
public:
    ChallengeMode_ItemQualityLevel() : ChallengeMode("ChallengeMode_ItemQualityLevel", SETTING_ITEM_QUALITY_LEVEL) {}

    bool CanEquipItem(Player* player, uint8 /*slot*/, uint16& /*dest*/, Item* pItem, bool /*swap*/, bool /*not_loading*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_ITEM_QUALITY_LEVEL, player))
        {
            return true;
        }
        return pItem->GetTemplate()->Quality <= ITEM_QUALITY_NORMAL;
    }

    void OnGiveXP(Player* player, uint32& amount, Unit* victim) override
    {
        ChallengeMode::OnGiveXP(player, amount, victim);
    }

    void OnLevelChanged(Player* player, uint8 oldlevel) override
    {
        ChallengeMode::OnLevelChanged(player, oldlevel);
    }
};

class ChallengeMode_SlowXpGain : public ChallengeMode
{
public:
    ChallengeMode_SlowXpGain() : ChallengeMode("ChallengeMode_SlowXpGain", SETTING_SLOW_XP_GAIN) {}

    void OnGiveXP(Player* player, uint32& amount, Unit* victim) override
    {
        ChallengeMode::OnGiveXP(player, amount, victim);
    }

    void OnLevelChanged(Player* player, uint8 oldlevel) override
    {
        ChallengeMode::OnLevelChanged(player, oldlevel);
    }
};

class ChallengeMode_VerySlowXpGain : public ChallengeMode
{
public:
    ChallengeMode_VerySlowXpGain() : ChallengeMode("ChallengeMode_VerySlowXpGain", SETTING_VERY_SLOW_XP_GAIN) {}

    void OnGiveXP(Player* player, uint32& amount, Unit* victim) override
    {
        ChallengeMode::OnGiveXP(player, amount, victim);
    }

    void OnLevelChanged(Player* player, uint8 oldlevel) override
    {
        ChallengeMode::OnLevelChanged(player, oldlevel);
    }
};

class ChallengeMode_QuestXpOnly : public ChallengeMode
{
public:
    ChallengeMode_QuestXpOnly() : ChallengeMode("ChallengeMode_QuestXpOnly", SETTING_QUEST_XP_ONLY) {}

    void OnGiveXP(Player* player, uint32& amount, Unit* victim) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_QUEST_XP_ONLY, player))
        {
            return;
        }
        if (victim)
        {
            amount = 0;
        }
        else
        {
            ChallengeMode::OnGiveXP(player, amount, victim);
        }
    }

    void OnLevelChanged(Player* player, uint8 oldlevel) override
    {
        ChallengeMode::OnLevelChanged(player, oldlevel);
    }
};

class ChallengeMode_IronMan : public ChallengeMode
{
public:
    ChallengeMode_IronMan() : ChallengeMode("ChallengeMode_IronMan", SETTING_IRON_MAN) {}

    void OnPlayerResurrect(Player* player, float /*restore_percent*/, bool /*applySickness*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return;
        }
        // A better implementation is to not allow the resurrect but this will need a new hook added first
        player->KillPlayer();
    }

    void OnGiveXP(Player* player, uint32& amount, Unit* victim) override
    {
        ChallengeMode::OnGiveXP(player, amount, victim);
    }

    void OnLevelChanged(Player* player, uint8 oldlevel) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return;
        }
        player->SetFreeTalentPoints(0); // Remove all talent points
        ChallengeMode::OnLevelChanged(player, oldlevel);
    }

    void OnTalentsReset(Player* player, bool /*noCost*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return;
        }
        player->SetFreeTalentPoints(0); // Remove all talent points
    }

    bool CanEquipItem(Player* player, uint8 /*slot*/, uint16& /*dest*/, Item* pItem, bool /*swap*/, bool /*not_loading*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return true;
        }
        return pItem->GetTemplate()->Quality <= ITEM_QUALITY_NORMAL;
    }

    bool CanApplyEnchantment(Player* player, Item* /*item*/, EnchantmentSlot /*slot*/, bool /*apply*/, bool /*apply_dur*/, bool /*ignore_condition*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return true;
        }
        // Are there any exceptions in WotLK? If so need to be added here
        return false;
    }

    void OnLearnSpell(Player* player, uint32 spellID) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return;
        }
        // These professions are class skills so they are always acceptable
        switch (spellID)
        {
            case RUNEFORGING:
            case POISONS:
            case BEAST_TRAINING:
                return;
            default:
                break;
        }
        // Do not allow learning any trade skills
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellID);
        if (!spellInfo)
            return;
        bool shouldForget = false;
        for (uint8 i = 0; i < 3; i++)
        {
            if (spellInfo->Effects[i].Effect == SPELL_EFFECT_TRADE_SKILL)
            {
                shouldForget = true;
            }
        }
        if (shouldForget)
        {
            player->removeSpell(spellID, SPEC_MASK_ALL, false);
        }
    }

    bool CanUseItem(Player* player, ItemTemplate const* proto, InventoryResult& /*result*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return true;
        }
        // Do not allow using elixir, potion, or flask
        if (proto->Class == ITEM_CLASS_CONSUMABLE &&
                (proto->SubClass == ITEM_SUBCLASS_POTION ||
                proto->SubClass == ITEM_SUBCLASS_ELIXIR ||
                proto->SubClass == ITEM_SUBCLASS_FLASK))
        {
            return false;
        }
        // Do not allow food that gives food buffs
        if (proto->Class == ITEM_CLASS_CONSUMABLE && proto->SubClass == ITEM_SUBCLASS_FOOD)
        {
            for (const auto & Spell : proto->Spells)
            {
                SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(Spell.SpellId);
                if (!spellInfo)
                    continue;

                for (uint8 i = 0; i < 3; i++)
                {
                    if (spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_PERIODIC_TRIGGER_SPELL)
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    bool CanGroupInvite(Player* player, std::string& /*membername*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return true;
        }
        return false;
    }

    bool CanGroupAccept(Player* player, Group* /*group*/) override
    {
        if (!sChallengeModes->challengeEnabledForPlayer(SETTING_IRON_MAN, player))
        {
            return true;
        }
        return false;
    }

};

class gobject_challenge_modes : public GameObjectScript
{
private:
    static bool playerSettingEnabled(Player* player, uint8 settingIndex)
    {
        return player->GetPlayerSetting("mod-challenge-modes", settingIndex).value;
    }

public:
    gobject_challenge_modes() : GameObjectScript("gobject_challenge_modes") { }

    struct gobject_challenge_modesAI: GameObjectAI
    {
        explicit gobject_challenge_modesAI(GameObject* object) : GameObjectAI(object) { };

        bool CanBeSeen(Player const* player) override
        {
            if ((player->getLevel() > 1) || (player->getClass() == CLASS_DEATH_KNIGHT && player->getLevel() > 55))
            {
                return false;
            }
            return sChallengeModes->enabled();
        }
    };

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        bool isDirty = playerSettingEnabled(player, SETTING_MARK_DIRTY);
        if (isDirty)
        {
            ChatHandler(player->GetSession()).SendSysMessage("Your character is not fresh, do not loot items or money before activiting this setting.");
            return false;
        }

        if (sChallengeModes->challengeEnabled(SETTING_HARDCORE) && !playerSettingEnabled(player, SETTING_HARDCORE) && !playerSettingEnabled(player, SETTING_SEMI_HARDCORE))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Enable Hardcore Mode", 0, SETTING_HARDCORE);
        }
        if (sChallengeModes->challengeEnabled(SETTING_SEMI_HARDCORE) && !playerSettingEnabled(player, SETTING_HARDCORE) && !playerSettingEnabled(player, SETTING_SEMI_HARDCORE))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Enable Semi-Hardcore Mode", 0, SETTING_SEMI_HARDCORE);
        }
        if (sChallengeModes->challengeEnabled(SETTING_SELF_CRAFTED) && !playerSettingEnabled(player, SETTING_SELF_CRAFTED) && !playerSettingEnabled(player, SETTING_IRON_MAN))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Enable Self-Crafted Mode", 0, SETTING_SELF_CRAFTED);
        }
        if (sChallengeModes->challengeEnabled(SETTING_ITEM_QUALITY_LEVEL) && !playerSettingEnabled(player, SETTING_ITEM_QUALITY_LEVEL))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Enable Low Quality Item Mode", 0, SETTING_ITEM_QUALITY_LEVEL);
        }
        if (sChallengeModes->challengeEnabled(SETTING_SLOW_XP_GAIN) && !playerSettingEnabled(player, SETTING_SLOW_XP_GAIN) && !playerSettingEnabled(player, SETTING_VERY_SLOW_XP_GAIN))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Enable Slow XP Mode", 0, SETTING_SLOW_XP_GAIN);
        }
        if (sChallengeModes->challengeEnabled(SETTING_VERY_SLOW_XP_GAIN) && !playerSettingEnabled(player, SETTING_SLOW_XP_GAIN) && !playerSettingEnabled(player, SETTING_VERY_SLOW_XP_GAIN))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Enable Very Slow XP Mode", 0, SETTING_VERY_SLOW_XP_GAIN);
        }
        if (sChallengeModes->challengeEnabled(SETTING_QUEST_XP_ONLY) && !playerSettingEnabled(player, SETTING_QUEST_XP_ONLY))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Enable Quest XP Only Mode", 0, SETTING_QUEST_XP_ONLY);
        }
        if (sChallengeModes->challengeEnabled(SETTING_IRON_MAN) && !playerSettingEnabled(player, SETTING_IRON_MAN) && !playerSettingEnabled(player, SETTING_SELF_CRAFTED))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Enable Iron Man Mode", 0, SETTING_IRON_MAN);
        }
        SendGossipMenuFor(player, 12669, go->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, GameObject* /*go*/, uint32 /*sender*/, uint32 action) override
    {
        player->UpdatePlayerSetting("mod-challenge-modes", action, 1);
        ChatHandler(player->GetSession()).PSendSysMessage("Challenge enabled.");
        CloseGossipMenuFor(player);
        return true;
    }

    GameObjectAI* GetAI(GameObject* object) const override
    {
        return new gobject_challenge_modesAI(object);
    }
};

// Add all scripts in one
void AddSC_mod_challenge_modes()
{
    new ChallengeModes_WorldScript();
    new gobject_challenge_modes();
    new ChallengeMode_Hardcore();
    new ChallengeMode_SemiHardcore();
    new ChallengeMode_SelfCrafted();
    new ChallengeMode_ItemQualityLevel();
    new ChallengeMode_SlowXpGain();
    new ChallengeMode_VerySlowXpGain();
    new ChallengeMode_QuestXpOnly();
    new ChallengeMode_IronMan();
    new ChallengeMiscPlayerScripts();
    new ChallengeMiscScripts();
    new ChallengeGuildScripts();
}

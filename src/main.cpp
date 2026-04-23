#include "scotland2/shared/modloader.h"
#include "beatsaber-hook/shared/utils/logging.hpp"

static ModInfo modInfo{"AnimalCompanyMods", "1.0.0", 0};
static Paper::ConstLoggerContext<13> logger = Paper::Logger::WithContext<"ACMods">();

extern "C" void setup(CModInfo* info) {
    info->id = "AnimalCompanyMods";
    info->version = "1.0.0";
    info->version_long = 0;
    logger.info("Animal Company Mod Menu setup called!");
}

extern "C" void late_load() {
    logger.info("Animal Company Mod Menu loaded!");
    logger.info("No hooks installed yet - need cordl bindings first.");
}

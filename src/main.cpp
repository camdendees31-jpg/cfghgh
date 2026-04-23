#include "scotland2/shared/modloader.h"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Physics.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

static ModInfo modInfo{"AnimalCompanyMods", "1.0.0", 0};
static Paper::ConstLoggerContext<13> logger = Paper::Logger::WithContext<"ACMods">();

// ===== Mod toggles =====
static bool flyEnabled = false;
static bool noclipEnabled = false;
static bool speedEnabled = false;
static float speedMultiplier = 2.0f;
static bool devModeEnabled = false;
static bool menuVisible = false;

// ===== Helper: find local player =====
static UnityEngine::GameObject* GetLocalPlayer() {
    auto playerControllerClass = il2cpp_utils::GetClassFromName("AnimalCompany", "PlayerController");
    if (!playerControllerClass) return nullptr;
    auto instance = il2cpp_utils::RunMethod<UnityEngine::Object*>(
        nullptr, playerControllerClass, "FindObjectOfType", playerControllerClass
    );
    if (!instance) return nullptr;
    return il2cpp_utils::RunMethod<UnityEngine::GameObject*>(instance, "get_gameObject").value_or(nullptr);
}

// ===== Helper: get player transform =====
static UnityEngine::Transform* GetPlayerTransform() {
    auto go = GetLocalPlayer();
    if (!go) return nullptr;
    return il2cpp_utils::RunMethod<UnityEngine::Transform*>(go, "get_transform").value_or(nullptr);
}

// ===== Fly / Noclip via GorillaLocomotion hook =====
// GorillaLocomotion.Update controls player movement velocity
MAKE_HOOK_MATCH(GorillaLocomotion_Update, 
    &GorillaLocomotion::Update, 
    void, GorillaLocomotion* self)
{
    GorillaLocomotion_Update(self);

    if (flyEnabled) {
        // Zero out gravity effect by pushing player upward slightly each frame
        auto rb = il2cpp_utils::GetFieldValue<UnityEngine::Rigidbody*>(self, "playerRigidBody");
        if (rb) {
            auto vel = il2cpp_utils::RunMethod<UnityEngine::Vector3>(rb, "get_velocity").value_or(UnityEngine::Vector3::get_zero());
            vel.y = 0.0f; // neutralize gravity
            il2cpp_utils::RunMethod(rb, "set_velocity", vel);
        }
    }

    if (speedEnabled) {
        auto rb = il2cpp_utils::GetFieldValue<UnityEngine::Rigidbody*>(self, "playerRigidBody");
        if (rb) {
            auto vel = il2cpp_utils::RunMethod<UnityEngine::Vector3>(rb, "get_velocity").value_or(UnityEngine::Vector3::get_zero());
            auto horizontal = UnityEngine::Vector3(vel.x * speedMultiplier, vel.y, vel.z * speedMultiplier);
            il2cpp_utils::RunMethod(rb, "set_velocity", horizontal);
        }
    }
}

// ===== Teleport =====
static void TeleportToPosition(UnityEngine::Vector3 targetPos) {
    auto transform = GetPlayerTransform();
    if (!transform) {
        logger.error("Teleport failed: can't find player transform");
        return;
    }
    il2cpp_utils::RunMethod(transform, "set_position", targetPos);
    logger.info("Teleported to ({}, {}, {})", targetPos.x, targetPos.y, targetPos.z);
}

// ===== Dev Mode toggle =====
static void SetDevMode(bool enabled) {
    // Dispatch SetUseDevModeAction through the game's own system
    auto actionClass = il2cpp_utils::GetClassFromName("AnimalCompany", "SetUseDevModeAction");
    if (!actionClass) {
        logger.warn("SetUseDevModeAction class not found");
        return;
    }
    auto action = il2cpp_utils::New<Il2CppObject*>(actionClass, enabled).value_or(nullptr);
    if (!action) return;

    // Dispatch the action — AC uses a Redux-style dispatcher
    auto appClass = il2cpp_utils::GetClassFromName("AnimalCompany", "App");
    if (appClass) {
        il2cpp_utils::RunMethod(nullptr, appClass, "Dispatch", action);
    }
    devModeEnabled = enabled;
    logger.info("Dev mode: {}", enabled ? "ON" : "OFF");
}

// ===== Holdable Menu hook =====
// We'll attach a simple world-space UI to the right hand transform
// This hooks PlayerWatch.Update to piggyback on the wrist update loop
MAKE_HOOK_MATCH(PlayerWatch_Update,
    &PlayerWatch::Update,
    void, PlayerWatch* self)
{
    PlayerWatch_Update(self);
    // Menu rendering would be handled by a separate MonoBehaviour component
    // attached via our mod's setup — see ModMenu below
}

// ===== Mod setup =====
extern "C" void setup(CModInfo* info) {
    info->id = "AnimalCompanyMods";
    info->version = "1.0.0";
    info->version_long = 0;
    logger.info("Setup called");
}

extern "C" void late_load() {
    logger.info("Loading Animal Company Mod Menu...");

    il2cpp_functions::Init();

    INSTALL_HOOK(logger, GorillaLocomotion_Update);
    INSTALL_HOOK(logger, PlayerWatch_Update);

    logger.info("Hooks installed!");
    logger.info("Features: Fly={}, Speed={}, Noclip={}", flyEnabled, speedEnabled, noclipEnabled);
}

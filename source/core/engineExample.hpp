#pragma once

/**
 * @file GameExample.hpp
 * @brief EXAMPLE ONLY — reference game module built on top of Engine.hpp.
 *
 * None of this is part of the engine. It exists to demonstrate the pattern
 * a game should follow: define your own component-type enum, event ids,
 * event structs, and payload structs, then hand them to `ComponentRouter` /
 * `SystemRouter` as template arguments. Delete/replace this file with your
 * actual game code.
 */

#include "engine.hpp"

// =============================================================================
// Game-defined component types (NOT part of the engine)
// =============================================================================
enum class ComponentType : ComponentTypeID
{
    None = 0,
    Mesh,
    Hitbox,
    Sfx,
    Health,
};

// =============================================================================
// Game-defined event ids + event structs (NOT part of the engine)
// =============================================================================
namespace EventID
{
enum : etl::message_id_t
{
    TookDamage = 0,
    PlaySound,
    ChangeState,
};
} // namespace EventID

namespace Event
{
/// Component -> System: this entity just took damage.
struct Damage : public etl::message<EventID::TookDamage>
{
    std::int32_t amount;
    std::int32_t element;
};

/// Component -> System: request a sound be played.
struct Audio : public etl::message<EventID::PlaySound>
{
    std::int32_t soundID;
    std::int32_t volume;
};

/// System -> Component: change internal state.
struct State : public etl::message<EventID::ChangeState>
{
    std::int32_t newStateID;
};
} // namespace Event

// =============================================================================
// Game-defined payload types (Component -> Manager, NOT part of the engine)
// =============================================================================
namespace Payload
{
struct RenderMesh
{
    std::int32_t modelID;
    fixed_t matrix[16];
};

struct CollisionHitbox
{
    fixed_t radius;
    fixed_t x;
    fixed_t y;
};
} // namespace Payload

// =============================================================================
// Example concrete Manager
// =============================================================================
class RenderManager : public Manager, public Singleton<RenderManager>
{
  public:
    void Init() override
    {
    }
    void Process() override
    {
    }
    void Shutdown() override
    {
    }

    /// Concrete, Manager-specific submission API (see Manager's class docs).
    void SubmitData(const Payload::RenderMesh& data)
    { /* push to VRAM, etc. */
    }

  private:
    friend class Singleton<RenderManager>;
    RenderManager() = default;
};

// =============================================================================
// Example concrete Component — only listens for Event::Damage.
// =============================================================================
class HealthComponent : public ComponentRouter<HealthComponent, Event::Damage>
{
  public:
    void Init() override
    {
        currentHP = 100;
    }
    void Update(fixed_t /*dt*/) override
    {
    }
    void Destroy() override
    {
    }
    ComponentTypeID GetType() const override
    {
        return static_cast<ComponentTypeID>(ComponentType::Health);
    }

    void on_receive(const Event::Damage& msg)
    {
        currentHP -= msg.amount;
    }
    void on_receive_unknown(const etl::imessage&)
    {
    }

  protected:
    void SubmitToManager() override
    {
    }

  private:
    std::int32_t currentHP = 100;
};

// =============================================================================
// Example concrete Component — submits render data to RenderManager.
// =============================================================================
class MeshComponent : public Component // no Pub/Sub needed -> plain Component
{
  public:
    void Init() override
    {
    }
    void Update(fixed_t /*dt*/) override
    {
        SubmitToManager();
    }
    void Destroy() override
    {
    }
    ComponentTypeID GetType() const override
    {
        return static_cast<ComponentTypeID>(ComponentType::Mesh);
    }

  protected:
    void SubmitToManager() override
    {
        RenderManager::GetInstance().SubmitData(Payload::RenderMesh{modelID, {}});
    }

  private:
    std::int32_t modelID = 0;
};

// =============================================================================
// Example concrete System (singleton, listens for Event::Damage).
// =============================================================================
constexpr etl::message_router_id_t kBattleSystemRouterID = 0;

class BattleSystem : public SystemRouter<BattleSystem, Event::Damage>, public Singleton<BattleSystem>
{
  public:
    void Init() override
    {
    }
    void Update(fixed_t /*dt*/) override
    {
    }
    void Shutdown() override
    {
    }

    void on_receive(const Event::Damage& msg)
    { /* apply battle rules */
    }
    void on_receive_unknown(const etl::imessage&)
    {
    }

  private:
    friend class Singleton<BattleSystem>;
    BattleSystem() : SystemRouter(kBattleSystemRouterID)
    {
    }
};

// =============================================================================
// Example: sizing and using the Engine
// =============================================================================
namespace GameEngineConfig
{
// The pool must be big enough for the largest concrete Component; here
// that's HealthComponent or MeshComponent, whichever is bigger.
constexpr std::size_t kLargestComponentSize = sizeof(HealthComponent) > sizeof(MeshComponent) ? sizeof(HealthComponent)
                                                                                              : sizeof(MeshComponent);
constexpr std::size_t kLargestComponentAlign = alignof(HealthComponent) > alignof(MeshComponent)
                                                   ? alignof(HealthComponent)
                                                   : alignof(MeshComponent);
} // namespace GameEngineConfig

using GameEngine = Engine<GameEngineConfig::kLargestComponentSize, GameEngineConfig::kLargestComponentAlign>;

// Example hardware hooks
void MyInputPoller()
{ /* read gamepad state */
}
void MyComputePusher()
{ /* swap buffers, push to VRAM */
}

int main()
{
    // 1. Instantiate the Engine
    GameEngine engine;

    // 2. Register your Singletons
    engine.RegisterManager(&RenderManager::GetInstance());
    engine.RegisterSystem(&BattleSystem::GetInstance());

    // 3. Set Hardware Hooks (HAL design)
    engine.SetPollInputCallback(MyInputPoller);
    engine.SetComputeCallback(MyComputePusher);

    // 4. Initialize everything (Systems first, then Managers)
    engine.InitAll();

    // --- (Optional) Setup Initial Game State ---
    Entity* player = engine.CreateEntity();
    if (player != nullptr)
    {
        // Allocate from the engine's memory pool
        HealthComponent* health = engine.CreateComponent<HealthComponent>();
        MeshComponent* mesh = engine.CreateComponent<MeshComponent>();

        // Attach to the entity
        player->AddComponent(health);
        player->AddComponent(mesh);
    }

    // 5. The Main Game Loop
    bool isRunning = true;
    while (isRunning)
    {
        // In a real game, you calculate the time since the last frame.
        // The engine requires this to be converted to the fixed_t type.
        fixed_t dt = fixed_t(16); // Example: 16ms delta time (roughly 60 FPS)

        // Drive the core engine loop forward one frame
        engine.Tick(dt);

        // (Insert your loop exit condition here)
    }

    // 6. Cleanup memory and hardware states
    engine.ShutdownAll();

    return 0;
}

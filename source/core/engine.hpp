#pragma once

/**
 * @file engine.hpp
 * @brief Aegis Engine - core EC/DOD hybrid framework.
 *
 * @note This header defines no game-specific data. `ComponentTypeID` is a
 *       plain integer alias, and Components/Systems accept their message
 *       types as template parameters. Game code supplies its own event
 *       enums, event structs, payload structs, and component-type enum in a
 *       separate header. See the companion `engineExample.hpp` file for a
 *       reference game module built on top of this file.
 *
 * @note Requires C++17 (use of inline variables).
 */

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include <etl/generic_pool.h>
#include <etl/message_bus.h>
#include <etl/message_router.h>
#include <etl/pool.h>
#include <etl/vector.h>
#include <fpm/fixed.hpp>

// =============================================================================
// Core type aliases
// =============================================================================

/// Unique identifier for an Entity instance.
using EntityID = std::uint32_t;

/**
 * @brief Engine-wide fixed-point type (Q16.16 signed).
 *
 * Assume no FPU, so all game-logic math (positions, damage rolls, timers,
 * delta-time, etc.) must go through this type instead of float/double.
 */
using fixed_t = fpm::fixed<std::int32_t, std::int64_t, 16>;

/**
 * @brief Generic identifier for a concrete Component's "type".
 *
 * The engine only ever stores/compares this as an opaque integer; it has no
 * knowledge of what the values mean. Game code defines its own enum (e.g.
 * `enum class ComponentType : ComponentTypeID { Mesh, Hitbox, Health, ... }`)
 * and returns the underlying value from `Component::GetType()`.
 */
using ComponentTypeID = std::uint16_t;

// Forward declarations
class Entity;
class Component;
class System;
class Manager;

// =============================================================================
// Engine-wide default capacity limits
//
// Every container in this file is fixed-capacity (etl::vector / etl::pool)
// rather than std::vector/new/delete. These are only *defaults*: `Engine` is a
// template so a game can override any of them per-project without editing
// this header. See `Engine`'s template parameters below.
// =============================================================================
namespace EngineLimits
{
constexpr std::size_t MAX_COMPONENTS_PER_ENTITY = 8;
constexpr std::size_t MAX_ENTITIES = 256;
constexpr std::size_t MAX_COMPONENTS = 1024;
constexpr std::size_t MAX_SYSTEMS = 32;
constexpr std::size_t MAX_MANAGERS = 16;
constexpr std::size_t MAX_MESSAGE_ROUTERS = 64;
} // namespace EngineLimits

/**
 * @brief The single global Pub/Sub router for all Component <-> System
 *        communication
 *
 * Declared `inline` so this header can be included in multiple translation
 * units without needing a companion .cpp.
 */
inline etl::message_bus<EngineLimits::MAX_MESSAGE_ROUTERS> engineBus;

/**
 * @brief Publishes an event onto the global `engineBus` so every subscribed
 *        Component/System (i.e. every `ComponentRouter`/`SystemRouter`
 *        instance whose template argument list includes `TMessage`)
 *        receives it via its `on_receive(const TMessage&)`.
 *
 * This is the "send" half of the two-way Pub/Sub design - the counterpart
 * to the `on_receive(...)` overloads a component/system writes to
 * "receive". Call it from anywhere a Component or System wants to
 * broadcast, e.g.:
 * @code
 * BroadcastEvent(Event::Damage{15, 0});
 * @endcode
 *
 * @tparam TMessage An `etl::message<ID>`-derived event type (see the
 *                   `Event` namespace convention in the companion example).
 * @param msg The event instance to broadcast.
 */
template <typename TMessage> inline void BroadcastEvent(const TMessage& msg)
{
    engineBus.receive(msg);
}

namespace detail
{
/**
 * @brief Hands out unique router ids (valid range: 0-249, see ETL docs)
 *        for Component instances subscribing to `engineBus`.
 *
 * Systems/Managers are singletons and supply their own fixed id instead
 * (see `SystemRouter`), since a monotonic counter isn't needed for a
 * type that only ever has one instance.
 */
inline etl::message_router_id_t NextComponentRouterID()
{
    static etl::message_router_id_t next = 0;
    return next++;
}

/**
* @brief SFINAE trait to verify a Component has defined TYPE_ID.
*/
template <typename, typename = void> struct has_type_id : std::false_type
{
};

template <typename T> struct has_type_id<T, std::void_t<decltype(T::TYPE_ID)>> : std::true_type
{
};
} // namespace detail

// =============================================================================
// Singleton — CRTP helper for System/Manager singletons
// =============================================================================

/**
 * @brief CRTP mixin that gives a class a lazily-constructed, thread-safe
 *        singleton accessor.
 *
 * Each concrete System/Manager subclass (e.g. `BattleSystem`, `RenderManager`)
 * gets its own independent singleton instance by inheriting `Singleton<TDerived>`
 * with itself as `TDerived`. There is deliberately no single shared
 * "System instance" at the abstract-base level, since a game has many distinct
 * System/Manager subclasses.
 *
 * @tparam TDerived The concrete class inheriting this mixin.
 */
template <typename TDerived> class Singleton
{
  public:
    /**
     * @brief Returns the single instance of TDerived, constructing it on
     *        first use.
     */
    static TDerived& GetInstance()
    {
        static TDerived instance;
        return instance;
    }

  protected:
    Singleton() = default;
    ~Singleton() = default;

  public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};

// =============================================================================
// Entity
// =============================================================================

/**
 * @brief An identity plus a fixed-capacity, non-owning list of Components.
 *
 * @note Ownership: Components are allocated from a shared pool owned by `Engine`.
 *       Entity only stores pointers into that pool; it does not own the memory.
 */
class Entity
{
  public:
    Entity() = default;

    /// @param id This entity's unique identifier.
    explicit Entity(EntityID id) : entityID(id)
    {
    }

    /// @return This entity's unique identifier.
    EntityID GetID() const
    {
        return entityID;
    }

    /**
     * @brief Registers an already-constructed Component with this entity
     *        and calls its `Init()`.
     * @param c Pointer to a Component allocated by `Engine`.
     */
    void AddComponent(Component* c);

    /**
     * @param type The `ComponentTypeID` to search for.
     * @return The first attached Component of the given type, or `nullptr`.
     */
    Component* GetComponentByID(ComponentTypeID type) const;

    /**
     * @brief Retrieves and safely casts a Component of the specified type.
     * @tparam T The concrete Component type (must define `TYPE_ID`).
     * @return Pointer to the component, or nullptr if not found.
     */
    template <typename T> T* GetComponent() const;

    /**
     * @brief Detaches the Component of the given type, calling its
     *        `Destroy()` first.
     * @note Does NOT free pool memory (see `Engine::DestroyComponent`),
     *       which is the recommended way to fully retire a component (it
     *       detaches from the owner, destroys, and frees the pool slot in
     *       one call without double-invoking `Destroy()`).
     * @param type The `ComponentTypeID` to remove.
     */
    void RemoveComponentByID(ComponentTypeID type);

    /**
     * @brief Detaches and destroys a Component of the specified type.
     * @tparam T The concrete Component type (must define `TYPE_ID`).
     */
    template <typename T> void RemoveComponent();

    /**
     * @brief Removes a component pointer from this entity's internal list
     *        WITHOUT calling `Destroy()` on it.
     *
     * Used by `Engine::DestroyComponent` to keep an entity's component list
     * in sync when a component's lifecycle (Destroy + pool free) is being
     * managed externally. Prefer `RemoveComponent`/`Engine::DestroyComponent`
     * for normal use; this is a low-level building block for those.
     * @param c The component pointer to detach. No-op if not found.
     */
    void DetachComponent(Component* c);

    /**
     * @brief Calls `Update(dt)` on every attached Component, per the Core
     *        Engine Loop's "Update Components" step.
     * @param dt Fixed-point delta time for this frame.
     */
    void Update(fixed_t dt);

    /// Destroys (but does not free) all attached components.
    void Destroy();

  private:
    EntityID entityID = 0;
    etl::vector<Component*, EngineLimits::MAX_COMPONENTS_PER_ENTITY> components;
};

// =============================================================================
// Component
// =============================================================================

/**
 * @brief Abstract base for all gameplay-facing "plugins".
 *
 * Bridges an Entity to Systems (via Pub/Sub) and to Managers (via a direct
 * one-way call). Structure: inherits from this abstract base only
 * (1-level max).
 *
 * @note This class intentionally knows nothing about specific message
 *       types. A Component that needs Pub/Sub should instead derive from
 *       `ComponentRouter<TDerived, TMessageTypes...>` (below), which mixes
 *       this interface together with an `etl::message_router` templated on
 *       whatever event types that concrete component cares about. A
 *       Component with no messaging needs can derive from `Component`
 *       directly.
 */
class Component
{
  public:
    virtual ~Component() = default;

    /// Called once when the component is attached to an Entity.
    virtual void Init() = 0;

    /**
     * @brief Per-frame update, called from `Entity::Update`.
     * @param dt Fixed-point delta time for this frame.
     */
    virtual void Update(fixed_t dt) = 0;

    /// Called when the component is detached/destroyed.
    virtual void Destroy() = 0;

    /// @return This component's game-defined type identifier.
    virtual ComponentTypeID GetType() const = 0;

    /// @return The Entity this component is attached to, or `nullptr`.
    Entity* GetOwner() const
    {
        return owner;
    }

    /// @param entity The Entity that now owns this component.
    void SetOwner(Entity* entity)
    {
        owner = entity;
    }

    /**
     * @brief Returns this component as an `etl::imessage_router` if it
     *        participates in Pub/Sub, or `nullptr` if it doesn't.
     *
     * `Engine` uses this to decide whether to subscribe/unsubscribe a
     * component to/from `engineBus`. Overridden by `ComponentRouter`.
     */
    virtual etl::imessage_router* AsMessageRouter()
    {
        return nullptr;
    }

  protected:
    /**
     * @brief One-way push of this component's data to its associated
     *        Manager(s).
     *
     * Deliberately takes no generic parameter: each concrete component
     * knows exactly which Manager singleton and payload type it submits to
     * (e.g. a MeshComponent calls
     * `RenderManager::GetInstance().SubmitData(Payload::RenderMesh{...})`).
     * A single virtual signature can't express "a different payload type
     * per component" without type erasure, which we want to avoid.
     */
    virtual void SubmitToManager() = 0;

    /// The Entity that owns this component (set via `SetOwner`).
    Entity* owner = nullptr;
};

/**
 * @brief CRTP mixin that gives a concrete Component Pub/Sub capability.
 *
 * Game code defines its own event types (see the companion example header)
 * and lists exactly the ones a given component cares about, e.g.:
 * @code
 * class HealthComponent : public ComponentRouter<HealthComponent, Event::Damage>
 * {
 * public:
 *     void on_receive(const Event::Damage& msg) { ... }
 *     void on_receive_unknown(const etl::imessage&) {}
 *     // ... Init/Update/Destroy/GetType/SubmitToManager ...
 * };
 * @endcode
 *
 * @note Per `etl::message_router`'s CRTP contract, `TDerived` (the final
 *       concrete class, NOT this mixin) must define `on_receive(...)` for
 *       every type in `TMessageTypes` plus `on_receive_unknown(...)`.
 *
 * @tparam TDerived      The concrete, final Component subclass.
 * @tparam TMessageTypes The Event types this component subscribes to/emits.
 */
template <typename TDerived, typename... TMessageTypes>
class ComponentRouter : public Component, public etl::message_router<TDerived, TMessageTypes...>
{
  public:
    /// @param routerID Defaults to an auto-assigned unique id per instance.
    explicit ComponentRouter(etl::message_router_id_t routerID = detail::NextComponentRouterID())
        : etl::message_router<TDerived, TMessageTypes...>(routerID)
    {
    }

    etl::imessage_router* AsMessageRouter() override
    {
        return static_cast<etl::message_router<TDerived, TMessageTypes...>*>(this);
    }
};

// =============================================================================
// System
// =============================================================================

/**
 * @brief Abstract base for a singleton handling one slice of game rules /
 *        state (e.g. Battle, Dialogue, UILogic).
 *
 * Talks to Components via two-way Pub/Sub only. Like `Component`, this base
 * knows nothing about specific message types (see `SystemRouter`).
 */
class System
{
  public:
    virtual ~System() = default;

    /// Called once at startup.
    virtual void Init() = 0;

    /**
     * @brief Per-frame update, called from `Engine::Tick`.
     * @param dt Fixed-point delta time for this frame.
     */
    virtual void Update(fixed_t dt) = 0;

    /// Called once at shutdown.
    virtual void Shutdown() = 0;

    /**
     * @brief Returns this system as an `etl::imessage_router` if it
     *        participates in Pub/Sub, or `nullptr` if it doesn't.
     */
    virtual etl::imessage_router* AsMessageRouter()
    {
        return nullptr;
    }

  protected:
    System() = default;

    /**
     * @brief Whether this system's `Update` should run its logic this
     *        frame. Toggled by concrete subclasses in response to events
     *        received from `engineBus` (e.g. a
     *        "Start[SystemName]System" event).
     */
    bool isActive = false;
};

/**
 * @brief CRTP mixin that gives a concrete System Pub/Sub capability.
 *
 * Mirrors `ComponentRouter`; see its documentation for the CRTP contract.
 * A concrete System is typically also combined with `Singleton<TDerived>`:
 * @code
 * class BattleSystem : public SystemRouter<BattleSystem, Event::Damage>,
 *                       public Singleton<BattleSystem>
 * {
 * public:
 *     BattleSystem() : SystemRouter(kBattleSystemRouterID) {}
 *     void on_receive(const Event::Damage& msg) { ... }
 *     void on_receive_unknown(const etl::imessage&) {}
 *     // ... Init/Update/Shutdown ...
 * };
 * @endcode
 *
 * @tparam TDerived      The concrete, final System subclass.
 * @tparam TMessageTypes The Event types this system subscribes to/emits.
 */
template <typename TDerived, typename... TMessageTypes>
class SystemRouter : public System, public etl::message_router<TDerived, TMessageTypes...>
{
  public:
    /// @param routerID A unique id for this System (systems are singletons).
    explicit SystemRouter(etl::message_router_id_t routerID) : etl::message_router<TDerived, TMessageTypes...>(routerID)
    {
    }

    etl::imessage_router* AsMessageRouter() override
    {
        return static_cast<etl::message_router<TDerived, TMessageTypes...>*>(this);
    }
};

// =============================================================================
// Manager
// =============================================================================

/**
 * @brief Abstract base for a singleton handling hardware abstraction, heavy
 *        computation, or memory management (the DOD side of the engine).
 *
 * Managers are the ONLY code allowed to call hardware-specific (platform)
 * functions.
 *
 * @note Manager intentionally does NOT derive from `etl::message_router`.
 *       Components submit to Managers via direct one-way calls, not
 *       Pub/Sub (see `Component::SubmitToManager`). Concrete Managers
 *       (e.g. `RenderManager`) each declare their own
 *       `SubmitData(const Payload::X&)` overload(s); those are not forced
 *       into this base class so adding a new payload type never requires
 *       touching `Manager` itself. Combine with `Singleton<TDerived>` for
 *       singleton behavior.
 */
class Manager
{
  public:
    virtual ~Manager() = default;

    /// Called once at startup.
    virtual void Init() = 0;

    /// Called once per frame, per the Core Engine Loop's "Process Managers" step.
    virtual void Process() = 0;

    /// Called once at shutdown.
    virtual void Shutdown() = 0;
};

// =============================================================================
// Engine
// =============================================================================

/**
 * @brief Owns global engine memory (Entity/Component pools), the registry
 *        of Systems/Managers, and drives the Core Engine Loop.
 *
 * `Engine` deliberately knows nothing about specific Systems, Managers,
 * Components, or message types. Those are all registered/created by game code at
 * runtime. This keeps the engine itself fully hardware and game-agnostic.
 *
 * All capacities are template parameters (not hardcoded constants) so each
 * project/platform can size its pools appropriately, e.g.:
 * @code
 * using GameEngine = Engine<sizeof(LargestComponent), alignof(LargestComponent)>;
 * GameEngine engine;
 * @endcode
 *
 * @tparam ComponentBlockSize  Byte size of the largest concrete Component
 *                             type the game will allocate (`sizeof(T)`).
 * @tparam ComponentBlockAlign Alignment of the largest concrete Component
 *                             type the game will allocate (`alignof(T)`).
 * @tparam MaxEntities         Max simultaneous Entities.
 * @tparam MaxComponents       Max simultaneous Components (all entities combined).
 * @tparam MaxSystems          Max registered Systems.
 * @tparam MaxManagers         Max registered Managers.
 */
template <std::size_t ComponentBlockSize,
          std::size_t ComponentBlockAlign,
          std::size_t MaxEntities = EngineLimits::MAX_ENTITIES,
          std::size_t MaxComponents = EngineLimits::MAX_COMPONENTS,
          std::size_t MaxSystems = EngineLimits::MAX_SYSTEMS,
          std::size_t MaxManagers = EngineLimits::MAX_MANAGERS>
class Engine
{
  public:
    Engine() = default;

    // --- Entity lifecycle -----------------------------------------------

    /// @return A freshly allocated, empty Entity, or `nullptr` if the entity pool/registry is full.
    Entity* CreateEntity()
    {
        if (activeEntities.full())
        {
            return nullptr;
        }

        Entity* entity = entityPool.create(nextEntityID++);
        if (entity != nullptr)
        {
            activeEntities.push_back(entity);
        }
        return entity;
    }

    /**
     * @brief Destroys all of the entity's components and returns the
     *        entity itself to the pool.
     * @param entity Entity previously returned by `CreateEntity`.
     */
    void DestroyEntity(Entity* entity)
    {
        if (entity == nullptr)
        {
            return;
        }

        entity->Destroy();

        for (auto it = activeEntities.begin(); it != activeEntities.end(); ++it)
        {
            if (*it == entity)
            {
                activeEntities.erase(it);
                break;
            }
        }

        entityPool.destroy(entity);
    }

    // --- Component lifecycle ---------------------------------------------

    /**
     * @brief Allocates a Component of concrete type `T` from the shared
     *        component pool, forwarding constructor arguments.
     * @tparam T    Concrete Component subclass to construct.
     * @tparam Args Constructor argument types, forwarded to `T`'s constructor.
     * @return Pointer to the new component, or `nullptr` if the pool is exhausted.
     */
    template <typename T, typename... Args> T* CreateComponent(Args&&... args)
    {
        static_assert(std::is_base_of<Component, T>::value, "\n\n[AE ERROR]: T must derive from Component");
        static_assert(detail::has_type_id<T>::value,
                      "\n\n[AE ERROR]: Component is missing its TYPE_ID\n"
                      "You must define: static constexpr ComponentTypeID TYPE_ID = ...\n");

        T* component = componentPool.template create<T>(std::forward<Args>(args)...);
        if (component != nullptr)
        {
            RegisterComponentForMessaging(component);
        }
        return component;
    }

    /**
     * @brief Fully retires a component: calls `Destroy()`, detaches it from
     *        its owning Entity (if any), unsubscribes it from `engineBus`
     *        (if applicable), and returns its memory to the pool.
     *
     * This is the recommended way to destroy a component. It keeps the
     * owning Entity's internal list in sync, unlike freeing the pool slot
     * directly.
     * @tparam T Concrete Component subclass (must match the allocated type).
     * @param component Pointer previously returned by `CreateComponent`.
     */
    template <typename T> void DestroyComponent(T* component)
    {
        if (component == nullptr)
        {
            return;
        }

        component->Destroy();

        if (Entity* owner = component->GetOwner())
        {
            owner->DetachComponent(component);
        }

        if (etl::imessage_router* router = component->AsMessageRouter())
        {
            engineBus.unsubscribe(*router);
        }

        componentPool.destroy(component);
    }

    /**
     * @brief Subscribes a component to `engineBus` if it participates in
     *        Pub/Sub. Called automatically by `CreateComponent`; exposed
     *        publicly for components constructed outside the pool (e.g. in
     *        unit tests).
     * @param component The component to register.
     */
    void RegisterComponentForMessaging(Component* component)
    {
        if (component == nullptr)
        {
            return;
        }

        if (etl::imessage_router* router = component->AsMessageRouter())
        {
            engineBus.subscribe(*router);
        }
    }

    // --- System / Manager registration -----------------------------------

    /**
     * @brief Registers a System so `Tick` calls its `Update` each frame,
     *        subscribing it to `engineBus` if it participates in Pub/Sub.
     * @param system Typically `&SomeSystem::GetInstance()`.
     */
    void RegisterSystem(System* system)
    {
        if (system == nullptr || systems.full())
        {
            return;
        }

        systems.push_back(system);

        if (etl::imessage_router* router = system->AsMessageRouter())
        {
            engineBus.subscribe(*router);
        }
    }

    /**
     * @brief Registers a Manager so `Tick` calls its `Process` each frame.
     * @param manager Typically `&SomeManager::GetInstance()`.
     */
    void RegisterManager(Manager* manager)
    {
        if (manager == nullptr || managers.full())
        {
            return;
        }

        managers.push_back(manager);
    }

    // --- Loop control -------------------------------------------------------

    /// Calls `Init()` on every registered System, then every registered Manager.
    void InitAll()
    {
        for (System* s : systems)
        {
            s->Init();
        }
        for (Manager* m : managers)
        {
            m->Init();
        }
    }

    /// Calls `Shutdown()` on every registered Manager, then every registered System.
    void ShutdownAll()
    {
        for (Manager* m : managers)
        {
            m->Shutdown();
        }
        for (System* s : systems)
        {
            s->Shutdown();
        }
    }

    /**
     * @brief Installs the platform-specific input-polling hook, called at
     *        the start of every `Tick`. Left injectable (rather than
     *        hardcoded) to keep `Engine` hardware-agnostic. Only Managers
     *        may touch hardware directly, and the function supplied here is
     *        expected to itself delegate to a Manager.
     */
    void SetPollInputCallback(void (*fn)())
    {
        pollInputFn = fn;
    }

    /**
     * @brief Installs the platform-specific "push final data to hardware"
     *        hook, called at the end of every `Tick`. Same rationale as
     *        `SetPollInputCallback`.
     */
    void SetComputeCallback(void (*fn)())
    {
        computeFn = fn;
    }

    /**
     * @brief Runs one iteration of the Core Engine Loop:
     *        Poll Input -> Update Systems -> Update Components -> Process Managers -> Compute.
     * @param dt Fixed-point delta time for this frame.
     */
    void Tick(fixed_t dt)
    {
        if (pollInputFn != nullptr)
        {
            pollInputFn();
        }

        for (System* s : systems)
        {
            s->Update(dt);
        }

        for (Entity* e : activeEntities)
        {
            e->Update(dt);
        }

        for (Manager* m : managers)
        {
            m->Process();
        }

        if (computeFn != nullptr)
        {
            computeFn();
        }
    }

  private:
    etl::pool<Entity, MaxEntities> entityPool;
    etl::generic_pool<ComponentBlockSize, ComponentBlockAlign, MaxComponents> componentPool;

    etl::vector<Entity*, MaxEntities> activeEntities;
    etl::vector<System*, MaxSystems> systems;
    etl::vector<Manager*, MaxManagers> managers;

    EntityID nextEntityID = 1;

    void (*pollInputFn)() = nullptr;
    void (*computeFn)() = nullptr;
};

// =============================================================================
// Entity method implementations
// =============================================================================

inline void Entity::AddComponent(Component* c)
{
    if (c == nullptr || components.full())
    {
        return;
    }

    c->SetOwner(this);
    c->Init();
    components.push_back(c);
}

inline Component* Entity::GetComponentByID(ComponentTypeID type) const
{
    for (Component* c : components)
    {
        if (c->GetType() == type)
        {
            return c;
        }
    }
    return nullptr;
}

template <typename T> inline T* Entity::GetComponent() const
{
    static_assert(detail::has_type_id<T>::value,
                  "\n\n[AE ERROR]: Component is missing its TYPE_ID\n"
                  "You must define: static constexpr ComponentTypeID TYPE_ID = ...\n");
    return static_cast<T*>(GetComponentByID(T::TYPE_ID));
}

inline void Entity::RemoveComponentByID(ComponentTypeID type)
{
    for (auto it = components.begin(); it != components.end(); ++it)
    {
        if ((*it)->GetType() == type)
        {
            (*it)->Destroy();
            components.erase(it);
            return;
        }
    }
}

template <typename T> inline void Entity::RemoveComponent()
{
    static_assert(detail::has_type_id<T>::value,
                  "\n\n[AE ERROR]: Component is missing its TYPE_ID\n"
                  "You must define: static constexpr ComponentTypeID TYPE_ID = ...\n");
    RemoveComponentById(T::TYPE_ID);
}

inline void Entity::DetachComponent(Component* c)
{
    for (auto it = components.begin(); it != components.end(); ++it)
    {
        if (*it == c)
        {
            components.erase(it);
            return;
        }
    }
}

inline void Entity::Update(fixed_t dt)
{
    for (Component* c : components)
    {
        c->Update(dt);
    }
}

inline void Entity::Destroy()
{
    for (Component* c : components)
    {
        c->Destroy();
    }
    components.clear();
}

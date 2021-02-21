#include <stdafx.h>
#include <Components.h>
#include <Events/UpdateEvent.h>
#include <Messages/RequestActorValueChanges.h>
#include <Messages/RequestActorMaxValueChanges.h>
#include <Messages/RequestHealthChangeBroadcast.h>
#include <Services/ActorService.h>
#include <World.h>
#include <GameServer.h>
#include <Messages/NotifyActorValueChanges.h>
#include <Messages/NotifyActorMaxValueChanges.h>
#include <Messages/NotifyHealthChangeBroadcast.h>

ActorService::ActorService(entt::dispatcher& aDispatcher, World& aWorld) noexcept
    : m_world(aWorld)
{
    m_updateHealthConnection = aDispatcher.sink<PacketEvent<RequestActorValueChanges>>().connect<&ActorService::OnActorValueChanges>(this);
    m_updateMaxValueConnection = aDispatcher.sink<PacketEvent<RequestActorMaxValueChanges>>().connect<&ActorService::OnActorMaxValueChanges>(this);
    m_updateDeltaHealthConnection = aDispatcher.sink<PacketEvent<RequestHealthChangeBroadcast>>().connect<&ActorService::OnHealthChangeBroadcast>(this);
}

ActorService::~ActorService() noexcept
{
}

void ActorService::OnActorValueChanges(const PacketEvent<RequestActorValueChanges>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    auto actorValuesView = m_world.view<ActorValuesComponent, OwnerComponent>();

    auto itor = actorValuesView.find(static_cast<entt::entity>(message.Id));

    if (itor != std::end(actorValuesView) ||
        actorValuesView.get<OwnerComponent>(*itor).ConnectionId == acMessage.ConnectionId)
    {
        auto& actorValuesComponent = actorValuesView.get<ActorValuesComponent>(*itor);
        for (auto& [id, value] : message.Values)
        {
            actorValuesComponent.CurrentActorValues.ActorValuesList[id] = value;
            auto val = actorValuesComponent.CurrentActorValues.ActorValuesList[id];
            spdlog::debug("Updating value {:x}:{:f} of {:x}", id, val, message.Id);
        }
    }

    NotifyActorValueChanges notifyChanges;
    notifyChanges.Id = acMessage.Packet.Id;
    notifyChanges.Values = acMessage.Packet.Values;

    auto view = m_world.view<PlayerComponent>();
    for (auto entity : view)
    {
        auto& player = view.get<PlayerComponent>(entity);

        if (player.ConnectionId != acMessage.ConnectionId)
        {
            GameServer::Get()->Send(player.ConnectionId, notifyChanges);
        }
    }
}

void ActorService::OnActorMaxValueChanges(const PacketEvent<RequestActorMaxValueChanges>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    auto actorValuesView = m_world.view<ActorValuesComponent, OwnerComponent>();

    auto itor = actorValuesView.find(static_cast<entt::entity>(message.Id));

    if (itor != std::end(actorValuesView) ||
        actorValuesView.get<OwnerComponent>(*itor).ConnectionId == acMessage.ConnectionId)
    {
        auto& actorValuesComponent = actorValuesView.get<ActorValuesComponent>(*itor);
        for (auto& [id, value] : message.Values)
        {
            actorValuesComponent.CurrentActorValues.ActorMaxValuesList[id] = value;
            auto val = actorValuesComponent.CurrentActorValues.ActorMaxValuesList[id];
            spdlog::debug("Updating max value {:x}:{:f} of {:x}", id, val, message.Id);
        }
    }

    NotifyActorMaxValueChanges notifyChanges;
    notifyChanges.Id = message.Id;
    notifyChanges.Values = message.Values;

    auto view = m_world.view<PlayerComponent>();
    for (auto entity : view)
    {
        auto& player = view.get<PlayerComponent>(entity);

        if (player.ConnectionId != acMessage.ConnectionId)
        {
            GameServer::Get()->Send(player.ConnectionId, notifyChanges);
        }
    }
}

void ActorService::OnHealthChangeBroadcast(const PacketEvent<RequestHealthChangeBroadcast>& acMessage) const noexcept
{
    NotifyHealthChangeBroadcast notifyDamageEvent;
    notifyDamageEvent.Id = acMessage.Packet.Id;
    notifyDamageEvent.DeltaHealth = acMessage.Packet.DeltaHealth;

    auto view = m_world.view<PlayerComponent>();
    for (auto entity : view)
    {
        auto& player = view.get<PlayerComponent>(entity);

        if (player.ConnectionId != acMessage.ConnectionId)
        {
            GameServer::Get()->Send(player.ConnectionId, notifyDamageEvent);
        }
    }
}


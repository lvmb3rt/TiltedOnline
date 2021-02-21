#pragma once

#include "Message.h"

using TiltedPhoques::Map;

struct NotifyActorMaxValueChanges final : ServerMessage
{
    NotifyActorMaxValueChanges() : ServerMessage(kNotifyActorMaxValueChanges)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyActorMaxValueChanges& acRhs) const noexcept
    {
        return Id == acRhs.Id && 
               Values == acRhs.Values &&
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Id;
    Map<uint32_t, float> Values;
};

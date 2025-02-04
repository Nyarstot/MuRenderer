#include "MuRenderer/Device/DevicePool.hpp"


namespace murenderer
{
    DevicePool::DevicePool()
    {
        m_devices.reserve(2);
    }

    DevicePool::~DevicePool()
    {
    }

    void DevicePool::AppendDevice(Device* aDevice)
    {
        if (aDevice->GetPriorityType() == DevicePriority_Primary) {
            m_primaryDeviceIndex = m_devices.size() + 1;
        }
        m_devices.push_back(aDevice);
    }

    void DevicePool::RemoveDevice(Device* aDevice)
    {

    }

    std::size_t DevicePool::DeviceCount() const
    {
        return m_devices.size();
    }

    Device* DevicePool::GetPrimaryDevice() const
    {
        return m_devices.at(m_primaryDeviceIndex);
    }
}
#include "include/midway/midway.h"

#include <utility>
#include <iostream>


void midway::MidiClient::UpdateAvailableDevices() {
    auto numSources = CountMidiDevices();

    std::vector<bool> stillExistingDevices(m_activeDevices.size(), false);
    std::vector<std::shared_ptr<MidiDevice>> newDevices;

    // Create connections to all new devices
    for (int i = 0; i < numSources; i++) {
        // Check if this source is an existing device
        auto found = std::find_if(m_activeDevices.begin(), m_activeDevices.end(), [&](std::shared_ptr<MidiDevice> const& a) {
            return a->IsSameAsDeviceAtIndex(i);
        });

        if (found != m_activeDevices.end()) {
            // Make sure we know this device still exists
            stillExistingDevices[std::distance(m_activeDevices.begin(), found)] = true;
        } else {
            // Create a handle for new devices
            auto device = std::make_shared<MidiDevice>(this);
            device->Setup(GetMidiHandle(i, device.get()));
            newDevices.push_back(device);
            m_handleDeviceConnect(device);
        }
    }

    for (int i = 0; i < m_activeDevices.size();) {
        if (stillExistingDevices[i]) {
            i++;
            continue;
        }

        m_activeDevices[i]->SetConnectionStatus(false);
        m_handleDeviceDisconnect(m_activeDevices[i]);
        stillExistingDevices.erase(stillExistingDevices.begin() + i);
        m_activeDevices.erase(m_activeDevices.begin() + i);
    }

    for (auto& device : newDevices) {
        m_activeDevices.emplace_back(device);
    }
}


void midway::MidiClient::OnNoteStart(std::function<void(std::shared_ptr<MidiDevice>&, int, int, int)> handler) {
    m_handleNoteStart = std::move(handler);
}

void midway::MidiClient::OnNoteEnd(std::function<void(std::shared_ptr<MidiDevice>&, int, int)> handler) {
    m_handleNoteEnd = std::move(handler);
}

void midway::MidiClient::OnControlChange(std::function<void(std::shared_ptr<MidiDevice>&, int, int, int)> handler) {
    m_handleControlChange = std::move(handler);
}

void midway::MidiClient::OnDeviceDisconnect(std::function<void(std::shared_ptr<MidiDevice>&)> handler) {
    m_handleDeviceDisconnect = std::move(handler);
}


void midway::MidiClient::OnDeviceConnect(std::function<void(std::shared_ptr<MidiDevice> &)> handler) {
    m_handleDeviceConnect = std::move(handler);
}

void midway::MidiClient::SetUpdateCausesOSRunLoop(bool updateCausesOSRunLoop) {
    m_updateCausesOSRunLoop = updateCausesOSRunLoop;
}

midway::MidiDevice::MidiDevice(MidiClient *client) : m_pMidiClient(client) {
}
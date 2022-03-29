#include "include/midway/midway.h"

#include <utility>


void midway::MidiClient::OnNoteStart(std::function<void(int, int, int)> handler) {
    m_handleNoteStart = std::move(handler);
}

void midway::MidiClient::OnNoteEnd(std::function<void(int, int)> handler) {
    m_handleNoteEnd = std::move(handler);
}

void midway::MidiClient::OnDeviceDisconnect(std::function<void()> handler) {
    m_handleDeviceDisconnect = std::move(handler);
}

void midway::MidiClient::OnControlChange(std::function<void(int, int, int)> handler) {
    m_handleControlChange = std::move(handler);
}

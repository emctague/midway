#include "include/midway/midway.h"
#include <iostream>

void midway::MidiCallback(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    auto client = (MidiClient*)dwInstance;
    BYTE status = dwParam1;
    BYTE note = dwParam1 >> 8;
    BYTE velocity = dwParam1 >> 16;

    if (wMsg == MIM_DATA) {
        if (status == 144) {
            client->m_handleNoteStart(1, note, velocity);
        } else if (status == 128) {
            client->m_handleNoteEnd(1, note);
        } else if (status == 176) {
            client->m_handleControlChange(1, note, velocity);
        } else {
            std::cout << "UNKNOWN STATUS: " << (UINT)status << std::endl;
        }
    } else if (wMsg == MIM_OPEN) {
        return;
    } else if (wMsg == MIM_CLOSE) {
        client->m_handleDeviceDisconnect();
    } else {
        std::cout << "UNKNOWN MESSAGE: " << wMsg << std::endl;
    }
}


midway::MidiClient::MidiClient(const char *clientName) {
}

midway::MidiClient::~MidiClient() = default;

int midway::MidiClient::CountDevices() {
    return (int) midiInGetNumDevs();
}

bool midway::MidiClient::ConnectToDevice(const MidiDeviceInfo &deviceInfo) {
    HMIDIIN dev;

    MMRESULT res = midiInOpen(&dev, deviceInfo.m_id, (DWORD_PTR) MidiCallback, (DWORD_PTR)this, CALLBACK_FUNCTION);
    if (res != MMSYSERR_NOERROR) {
        std::cerr << "MIDI Error" << std::endl;
        return false;
    }

    midiInStart(dev);
    return true;
}

midway::MidiDeviceInfo midway::MidiClient::GetDeviceInfo(int id) {
    MIDIINCAPS caps;
    midiInGetDevCaps(id, &caps, sizeof(MIDIINCAPS));

    return MidiDeviceInfo {
        (uint32_t)id,
        std::string(caps.szPname)
    };
}

#include "include/midway/midway.h"
#include <iostream>

void midway::MidiCallback(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    auto device = (MidiDevice*)dwInstance;
    auto devShared = device->GetShared();
    auto client = device->GetClient();
    BYTE status = dwParam1;
    BYTE note = dwParam1 >> 8;
    BYTE velocity = dwParam1 >> 16;

    if (wMsg == MIM_DATA) {
        if (status >= 128 && status <= 143) {
            client->m_handleNoteEnd(devShared, status - 128, note);
        } else if (status >= 144 && status <= 159) {
            client->m_handleNoteStart(devShared, status - 144, note, velocity);
        } else if (status >= 176 && status <= 191) {
            client->m_handleControlChange(devShared, status - 176, note, velocity);
        }
    }
}


midway::MidiClient::MidiClient(const char *clientName) {
}

midway::MidiClient::~MidiClient() = default;

int midway::MidiClient::CountMidiDevices() const {
    return (int) midiInGetNumDevs();
}

midway::MidiDeviceOSHandle midway::MidiClient::GetMidiHandle(int index, MidiDevice *devicePtr) {
    HMIDIIN midiIn;
    MMRESULT res = midiInOpen(&midiIn, index, (DWORD_PTR)MidiCallback, (DWORD_PTR)devicePtr, CALLBACK_FUNCTION);
    if (res != MMSYSERR_NOERROR) {
        char text[512]{};
        midiInGetErrorText(res, text, 511);
        throw std::runtime_error("midiInOpen Error: " + std::string(text));
    }
    return midiIn;
}


void midway::MidiClient::StartDeviceInput(std::shared_ptr<MidiDevice> &device) {
    midiInStart(device->m_handle);
}

void midway::MidiClient::StopDeviceInput(std::shared_ptr<MidiDevice> &device) {
    midiInStop(device->m_handle);
}

void midway::MidiDevice::Setup(midway::MidiDeviceOSHandle handle)
{
    m_handle = handle;
    UINT id;
    midiInGetID(m_handle, &id);

    MIDIINCAPS caps;
    MMRESULT res = midiInGetDevCaps(id, &caps, sizeof(MIDIINCAPS));

    if (res != MMSYSERR_NOERROR) {
        char text[512]{};
        midiInGetErrorText(res, text, 511);
        throw std::runtime_error("midiInGetDevCaps Error: " + std::string(text));
    }

    m_deviceName = std::string(caps.szPname);
    m_deviceManufacturer = "Unknown Manufacturer";
}

bool midway::MidiDevice::IsSameAsDeviceAtIndex(int index) const {
    UINT id;
    midiInGetID(m_handle, &id);
    return id == index;
}

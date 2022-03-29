#include "include/midway/midway.h"
#include <CoreMIDI/CoreMIDI.h>
#include <iostream>

void midwayMidiNotifyProcOSX(const MIDINotification *message, void *refCon) {
    // why is this never called?
    std::cout << "MIDI Notif" << std::endl;
}


void midway::midwayDecodeEventOSX(void *context, MIDITimeStamp ts, MIDIUniversalMessage msg) {
    auto client = ((MidiClient *) context);

#define CMB(A, B) (((uint8_t)(A) << 8) | (uint8_t)(B))
#define VOICE1 kMIDIMessageTypeChannelVoice1
    switch (CMB(msg.type, msg.system.status)) {
        case CMB(kMIDIMessageTypeSystem, kMIDIStatusStop):
            client->m_handleDeviceDisconnect();
            break;
        case CMB(VOICE1, kMIDICVStatusNoteOn):
            client->m_handleNoteStart(msg.group, msg.channelVoice1.note.number, msg.channelVoice1.note.velocity);
            break;
        case CMB(VOICE1, kMIDICVStatusNoteOff):
            client->m_handleNoteEnd(msg.group, msg.channelVoice1.note.number);
            break;
        case CMB(VOICE1, kMIDICVStatusControlChange):
            client->m_handleControlChange(msg.group, msg.channelVoice1.controlChange.index,
                                          msg.channelVoice1.controlChange.data);
            break;
    }
#undef CMB
#undef VOICE1
}

midway::MidiClient::MidiClient(const char *clientName) {
    auto clientNameRef = CFStringCreateWithCStringNoCopy(nullptr, clientName, kCFStringEncodingUTF8, nullptr);
    MIDIClientCreate(clientNameRef, (MIDINotifyProc) midwayMidiNotifyProcOSX, (void *) this, &m_client);

    // MIDI message pump. We have no way of figuring out which device sent the message right now :(
    MIDIInputPortCreateWithProtocol(m_client, clientNameRef, kMIDIProtocol_1_0, &m_inPort,
                                    ^(const MIDIEventList *events, void *ref) {
                                        MIDIEventListForEachEvent(events, midway::midwayDecodeEventOSX, ref);
                                    });
}

midway::MidiClient::~MidiClient() {
    MIDIPortDispose(m_inPort);
    MIDIClientDispose(m_client);
}

int midway::MidiClient::CountDevices() {
    return (int) MIDIGetNumberOfSources();
}

bool midway::MidiClient::ConnectToDevice(const MidiDeviceInfo &deviceInfo) {
    MIDIEndpointRef ref = 0;
    MIDIObjectType type;
    auto status = MIDIObjectFindByUniqueID((MIDIUniqueID) deviceInfo.GetId(), &ref, &type);

    if (!ref || status == kMIDIObjectNotFound || type != kMIDIObjectType_Source) return false;
    MIDIPortConnectSource(m_inPort, ref, this);

    return true;
}

midway::MidiDeviceInfo midway::MidiClient::GetDeviceInfo(int id) {
    MIDIEndpointRef er = MIDIGetSource(id);

    SInt32 deviceId = 0;
    MIDIObjectGetIntegerProperty(er, kMIDIPropertyUniqueID, &deviceId);

    CFStringRef displayName = CFSTR("");
    MIDIObjectGetStringProperty(er, kMIDIPropertyDisplayName, &displayName);

    return MidiDeviceInfo{
            (uint32_t) deviceId,
            std::string(CFStringGetCStringPtr(displayName, kCFStringEncodingUTF8))
    };
}

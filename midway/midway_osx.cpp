#include "include/midway/midway.h"
#include <CoreMIDI/CoreMIDI.h>
#include <iostream>
#import <Foundation/Foundation.h>

void midway::midwayDecodeEventOSX(void *context, MIDITimeStamp ts, MIDIUniversalMessage msg) {
    auto device = (MidiDevice*)context;
    auto devShared = device->GetShared();
    auto client = device->GetClient();

#define CMB(A, B) (((uint64_t)(A) << 32) | (uint64_t)(B))
#define VOICE1 kMIDIMessageTypeChannelVoice1
    switch (CMB(msg.type, msg.system.status)) {
        case CMB(VOICE1, kMIDICVStatusNoteOn):
            client->m_handleNoteStart(devShared, msg.channelVoice1.channel, msg.channelVoice1.note.number, msg.channelVoice1.note.velocity);
            break;
        case CMB(VOICE1, kMIDICVStatusNoteOff):
            client->m_handleNoteEnd(devShared, msg.channelVoice1.channel, msg.channelVoice1.note.number);
            break;
        case CMB(VOICE1, kMIDICVStatusControlChange):
            client->m_handleControlChange(devShared, msg.channelVoice1.channel, msg.channelVoice1.controlChange.index,
                                          msg.channelVoice1.controlChange.data);
            break;
    }
#undef CMB
#undef VOICE1
}

midway::MidiClient::MidiClient(const char *clientName) {
    auto clientNameRef = CFStringCreateWithCStringNoCopy(nullptr, clientName, kCFStringEncodingUTF8, kCFAllocatorNull);
    MIDIClientCreate(clientNameRef, nullptr, (void *) this, &m_client);

    // MIDI message pump. We have no way of figuring out which device sent the message right now :(
    MIDIInputPortCreateWithProtocol(m_client, clientNameRef, kMIDIProtocol_1_0, &m_inPort,
                                    ^(const MIDIEventList *events, void *ref) {
                                        MIDIEventListForEachEvent(events, midway::midwayDecodeEventOSX, ref);
                                    });

    CFRelease(clientNameRef);
}

midway::MidiClient::~MidiClient() {
    MIDIPortDispose(m_inPort);
    MIDIClientDispose(m_client);
}

int midway::MidiClient::CountMidiDevices() const {
    if (m_updateCausesOSRunLoop) CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true);
    return (int)MIDIGetNumberOfSources();
}

midway::MidiDeviceOSHandle midway::MidiClient::GetMidiHandle(int index, MidiDevice *device) {
    return MIDIGetSource(index);
}

void midway::MidiClient::StartDeviceInput(std::shared_ptr<MidiDevice> &device) {
    MIDIPortConnectSource(m_inPort, device->m_handle, device.get());
}

void midway::MidiClient::StopDeviceInput(std::shared_ptr<MidiDevice> &device) {
    MIDIPortDisconnectSource(m_inPort, device->m_handle);
}


bool midway::MidiDevice::IsSameAsDeviceAtIndex(int index) const {
    return MIDIGetSource(index) == m_handle;
}

void midway::MidiDevice::Setup(midway::MidiDeviceOSHandle handle)
{
    m_handle = handle;
    CFStringRef ref = CFSTR("");
    MIDIObjectGetStringProperty(m_handle, kMIDIPropertyDisplayName, &ref);
    m_deviceName = [((NSString*)ref) UTF8String];

    MIDIObjectGetStringProperty(m_handle, kMIDIPropertyManufacturer, &ref);
    m_deviceManufacturer = [((NSString*)ref) UTF8String];

    CFRelease(ref);
}

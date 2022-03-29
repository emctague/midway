#include <iostream>
#include <midway/midway.h>

#if defined(WIN32)
#include <windows.h>
#define sleep(s) Sleep(s * 1000)
#else
#include <unistd.h>
#endif

int main() {
    // Initialize MIDI client
    midway::MidiClient client("TestMIDIClient");
    client.OnNoteStart([](int channel, int note, int velocity) {
        std::cout << "Key " << note << " pressed at vel. " << velocity << std::endl;
    });
    client.OnNoteEnd([](int channel, int note) {
        std::cout << "Key " << note << " released" << std::endl;
    });

    // Make sure there is a MIDI keyboard plugged in
    auto nDevices = client.CountDevices();
    if (nDevices < 1) return 0;

    // Connect to first MIDI keyboard
    auto firstDeviceInfo = client.GetDeviceInfo(0);
    std::cout << "Connecting to: " << firstDeviceInfo.GetName() << std::endl;
    if (!client.ConnectToDevice(firstDeviceInfo)) return 0;

    // Wait a bit!
    sleep(60);
}
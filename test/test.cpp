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
    client.OnDeviceConnect([&](std::shared_ptr<midway::MidiDevice>& device) {
        std::cout << "Device added: " << device->GetDeviceName() << " by " << device->GetDeviceManufacturer() << std::endl;
        client.StartDeviceInput(device);
    });
    client.OnDeviceDisconnect([&](std::shared_ptr<midway::MidiDevice>& device) {
        std::cout << "Device removed: " << device->GetDeviceName() << " by " << device->GetDeviceManufacturer() << std::endl;
        client.StopDeviceInput(device);
    });
    client.OnNoteStart([](int channel, int note, int velocity) {
        std::cout << "Key " << note << " pressed at vel. " << velocity << " on ch. " << channel << std::endl;
    });
    client.OnNoteEnd([](int channel, int note) {
        std::cout << "Key " << note << " released on ch. " << channel << std::endl;
    });

    for (int i = 0; i < 120; i++) {
        client.UpdateAvailableDevices();
        sleep(1);
    }
}
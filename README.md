# midway

A small C++ library for MIDI input on macOS and Win32.

Uses system APIs on both platforms (Windows Multimedia, Core MIDI).

## How to use

```c++
#include <midway/midway.h>
#include <iostream>

int main() {
    // Initialize a Midway MIDI client. The name here is used when on macOS.
    midway::MidiClient client("MyAwesomeApplication");

    // When we connect to a device, log that and start reading input from it!
    client.OnDeviceConnect([&](std::shared_ptr<midway::MidiDevice>& device) {
        std::cout << "Device added: " << device->GetDeviceName() << " by " << device->GetDeviceManufacturer() << std::endl;
        client.StartDeviceInput(device);
    });
    
    // When we disconnect from a device, stop reading input!
    client.OnDeviceDisconnect([&](std::shared_ptr<midway::MidiDevice>& device) {
        client.StopDeviceInput(device);
    });
    
    // Called when a key is pressed down
    client.OnNoteStart([](std::shared_ptr<midway::MidiDevice>& device, int channel, int note, int velocity) {
    });
    
    // Called when a key is released
    client.OnNoteEnd([](std::shared_ptr<midway::MidiDevice>& device, int channel, int note) {
    });
    
    // Called when a control surface's value is changed
    client.OnControlChange([](std::shared_ptr<midway::MidiDevice>& device, int channel, int control, int value) {
    });

    // Now we loop or whatever else your application needs to do, making sure to
    // occasionally call UpdateAvailableDevices.
    for (int i = 0; i < 120; i++) {
        client.UpdateAvailableDevices();
        sleep(1);
    }
    
    return 0;
}
```

## Current Limitations

 - We cannot get the device manufacturer name on Win32.
 - We only support INPUT.
 - We only support basic note and control surface changes (as this is what my own hardware supports.)
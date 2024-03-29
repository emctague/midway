#include <utility>
#include <cstdint>
#include <string>
#include <functional>
#include <vector>
#include <memory>

#ifndef MIDWAY_MIDWAY_H
#define MIDWAY_MIDWAY_H

#if defined(__APPLE__)
#include <CoreMIDI/CoreMIDI.h>
#elif defined(WIN32)
#include <windows.h>
#include <mmeapi.h>
#endif

namespace midway {

#if defined(__APPLE__)
    using MidiDeviceOSHandle = MIDIEndpointRef;
#elif defined(WIN32)
    using MidiDeviceOSHandle = HMIDIIN;
#endif

    class MidiClient;

    class MidiDevice : public std::enable_shared_from_this<MidiDevice> {
    public:
        explicit MidiDevice(MidiClient *client);
        [[nodiscard]] bool IsConnected() const { return m_connected; }

        /** Get device's display name. */
        [[nodiscard]] const std::string& GetDeviceName() const { return m_deviceName; }

        /** Get device manufacturer. Note that Win32 does not provide accurate manufacturer information. */
        [[nodiscard]] const std::string& GetDeviceManufacturer() const { return m_deviceManufacturer; }

        MidiClient *GetClient() { return m_pMidiClient; }

        std::shared_ptr<MidiDevice> GetShared() { return shared_from_this(); }

    protected:
        friend class MidiCLient;
        void Setup(MidiDeviceOSHandle endpoint);

        friend class MidiClient;
        void SetConnectionStatus(bool isConnected) { m_connected = isConnected; }

        friend class MidiClient;
        [[nodiscard]] bool IsSameAsDeviceAtIndex(int index) const;

#if defined(__APPLE__)
        MIDIEndpointRef m_handle {};
#elif defined(WIN32)
        HMIDIIN m_handle;
#endif

        MidiClient *m_pMidiClient;

        std::string m_deviceName;
        std::string m_deviceManufacturer;
        bool m_connected = true;
    };

    /**
     * The Client manages connectivity to MIDI devices.
     * It can poll for new devices and help establish connections to them.
     */
    class MidiClient {
    public:

        /**
         * Initialize a new MIDI client.
         * @param clientName A display name for this client application as a MIDI user (relevant only on macOS.)
         */
        explicit MidiClient(const char *clientName);

        /**
         * Begin input from the given MIDI device.
         * @param device The MIDI device to receive input from.
         */
        void StartDeviceInput(std::shared_ptr<MidiDevice>& device);

        /**
         * Halt input from the given MIDI devie.
         * @param device The MIDI device to stop input from.
         */
        void StopDeviceInput(std::shared_ptr<MidiDevice>& device);

        /**
         * Update the client's knowledge of available MIDI devices, potentially
         * causing device add/remove events to be triggered.
         *
         * Note that by default, on macOS, this triggers a single CFRunLoop pass - this may
         * be undesirable in GUI applications that already have a CF run loop, so this behaviour may
         * be disabled with SetUpdateCausesOSRunLoop(false).
         */
        void UpdateAvailableDevices();

        /**
         * Configure whether UpdateAvailableDevices causes an OS-level 'run loop'
         * update when necessary - on macOS, setting this to true forces
         * UpdateAvailableDevices to trigger one CFRunLoop pass, which is necessary for
         * MIDI device listings to change.
         *
         * This defaults to true, but may be disabled if your application already has the necessary
         * run loop behaviour.
         * @param updateCausesOSRunLoop Whether an update should trigger the OS run loop or not.
         */
        void SetUpdateCausesOSRunLoop(bool updateCausesOSRunLoop);

        /**
         * Set the callback to be called when a note begins playing.
         * @param handler The callback function to be invoked.
         */
        void OnNoteStart(std::function<void(std::shared_ptr<MidiDevice>& device, int channel, int note, int velocity)> handler);

        /**
         * Set the callback to be called when a note stops playing.
         * @param handler The callback function to be invoked.
         */
        void OnNoteEnd(std::function<void(std::shared_ptr<MidiDevice>& device, int channel, int note)> handler);

        /**
         * Set the callback to be called when a device becomes available.
         * @param handler The callback function to be invoked.
         */
        void OnDeviceConnect(std::function<void(std::shared_ptr<MidiDevice>&)> handler);

        /**
         * Set the callback to be called when a device disconnects.
         * @param handler The callback function to be invoked.
         */
        void OnDeviceDisconnect(std::function<void(std::shared_ptr<MidiDevice>&)> handler);

        /**
         * Set the callback to be called when a control surface's value is changed.
         * @param handler The callback function to be invoked.
         */
        void OnControlChange(std::function<void(std::shared_ptr<MidiDevice>& device, int channel, int control, int value)> handler);

        ~MidiClient();

    private:
#if defined(__APPLE__)
        MIDIClientRef m_client{};
        MIDIPortRef m_inPort{};

        friend void midwayDecodeEventOSX(void *context, MIDITimeStamp ts, MIDIUniversalMessage msg);
#elif defined(WIN32)
        friend void MidiCallback(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
#endif

        bool m_updateCausesOSRunLoop = true;

        [[nodiscard]] int CountMidiDevices() const;
        MidiDeviceOSHandle GetMidiHandle(int index, MidiDevice *devicePtr);

        std::vector<std::shared_ptr<MidiDevice>> m_activeDevices;
        std::function<void(std::shared_ptr<MidiDevice>&, int, int, int)> m_handleNoteStart = [](std::shared_ptr<MidiDevice>&,int,int,int){};
        std::function<void(std::shared_ptr<MidiDevice>&, int, int)> m_handleNoteEnd = [](std::shared_ptr<MidiDevice>&,int,int){};
        std::function<void(std::shared_ptr<MidiDevice>&, int, int, int)> m_handleControlChange = [](std::shared_ptr<MidiDevice>&,int,int,int){};
        std::function<void(std::shared_ptr<MidiDevice>&)> m_handleDeviceConnect = [](std::shared_ptr<MidiDevice>&){};
        std::function<void(std::shared_ptr<MidiDevice>&)> m_handleDeviceDisconnect = [](std::shared_ptr<MidiDevice>&){};
    };


}

#endif //MIDWAY_MIDWAY_H

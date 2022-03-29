#include <utility>
#include <cstdint>
#include <string>
#include <functional>

#ifndef MIDWAY_MIDWAY_H
#define MIDWAY_MIDWAY_H

#if defined(__APPLE__)
#include <CoreMIDI/CoreMIDI.h>
#endif

#if defined(WIN32)
#include <windows.h>
#include <mmeapi.h>
#endif

namespace midway {

    /**
     * Identifying information about a MIDI input device.
     */
    class MidiDeviceInfo {
    public:

        /// Gets the unique ID identifying this MIDI device on the system
        [[nodiscard]] uint32_t GetId() const { return m_id; }

        /// Gets a display name for this MIDI device
        [[nodiscard]] std::string GetName() const { return m_name; }

    protected:
        friend class MidiClient;
        MidiDeviceInfo(uint32_t id, std::string  name) : m_id(id), m_name(std::move(name))
        {}

        const uint32_t m_id;
        const std::string m_name;
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
         * Counts the number of MIDI devices available.
         * @return The number of devices found.
         */
        int CountDevices();

        /**
         * Obtain identifying information about an available MIDI device.
         * @param id Index of the device (out of # available devices.)
         * @return Information about said device.
         */
        MidiDeviceInfo GetDeviceInfo(int id);

        /**
         * Connect to a MIDI device.
         * @param deviceInfo A MidiDeviceInfo object representing the device to connect to.
         * @return True if the device connected successfully.
         */
        bool ConnectToDevice(const MidiDeviceInfo& deviceInfo);

        /**
         * Set the callback to be called when a note begins playing.
         * @param handler The callback function to be invoked.
         */
        void OnNoteStart(std::function<void(int channel, int note, int velocity)> handler);

        /**
         * Set the callback to be called when a note stops playing.
         * @param handler The callback function to be invoked.
         */
        void OnNoteEnd(std::function<void(int channel, int note)> handler);

        /**
         * Set the callback to be called when a device disconnects.
         * @param handler The callback function to be invoked.
         */
        void OnDeviceDisconnect(std::function<void()> handler);

        /**
         * Set the callback to be called when a control surface's value is changed.
         * @param handler The callback function to be invoked.
         */
        void OnControlChange(std::function<void(int channel, int control, int value)> handler);

        ~MidiClient();

    private:
#if defined(__APPLE__)
        MIDIClientRef m_client{};
        MIDIPortRef m_inPort{};

        friend void midwayDecodeEventOSX(void *context, MIDITimeStamp ts, MIDIUniversalMessage msg);
#endif
#if defined(WIN32)
        friend void MidiCallback(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
#endif
        std::function<void(int, int, int)> m_handleNoteStart = [](int,int,int){};
        std::function<void(int, int)> m_handleNoteEnd = [](int,int){};
        std::function<void()> m_handleDeviceDisconnect = [](){};
        std::function<void(int, int, int)> m_handleControlChange = [](int,int,int){};
    };

}

#endif //MIDWAY_MIDWAY_H

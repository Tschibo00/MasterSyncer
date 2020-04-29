# MasterSyncer
- Universal master clock for snyc-triggered synths.
- BPM goes from 1-999
- By pressing Select you get to the channel selection. Select the required channel 1-9 by turning the encoder.
- Press Select again to choose the sync mode: 2,4,24,48 PPQ positive pulse, 2,4,24,48 PPQ negative pulse.
- Press Select again to choose pulse length (0.1-9.9ms)
- Press Select again to get back to channel selection.
- Mode will fallback to BPM after 1.5s from channel selection and after 10s from other modes.
- Press Start/Stop to start/stop the sequencer.
- First beat flashes both key leds, consecutive flashes only yellow.
- All configurations including BPM are stored in EEPROM, thus are still available after switching off.

That's basically it.
Have fun building and syncing all your devices.

# Todos
- Implement configurable delay (-200..+200ms) for each channel as some machines seem to be a bit off with the timing (e.g. Behringer TD-3)
- Add table with recommended settings for several machines

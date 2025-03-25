// Bitwig controllerscript for octacon
// cp octacon.control.js "$HOME/Bitwig Studio/Controller Scripts/"
//
// api-docs: /opt/bitwig-studio/resources/ControllerScripts/api/midi.js

loadAPI(20);

// Remove this if you want to be able to use deprecated methods without causing script to stop.
// This is useful during development.
host.setShouldFailOnDeprecatedUse(true);

host.defineController("ensonic", "Octacon", "0.1", "c952ed22-2f21-4b56-b5ba-c015f13da556", "ensonic74");
host.defineMidiPorts(1, 1);
host.addDeviceNameBasedDiscoveryPair(["Octacon MIDI 1"], ["Octacon MIDI 1"]);

const SYSEX_BEGIN = "F0 7D ";
const SYSEX_END = " F7";

let remoteControlCursor;
let outPort;
let midiQueue = [];

function init() {
	host.getMidiInPort(0).setMidiCallback(onMidi0);
	outPort = host.getMidiOutPort(0);

	// follows UI selection
	let cursorDevice = host.createCursorTrack(0, 0).createCursorDevice();
	remoteControlCursor = cursorDevice.createCursorRemoteControlsPage(8);

	for (let j = 0; j < remoteControlCursor.getParameterCount(); j++) {
		let valueFn = onValueChange.bind(this, j);
		let nameFn = onNameChange.bind(this, j);
		let param = remoteControlCursor.getParameter(j);
		param.markInterested();
		param.setIndication(true);
		param.value().addValueObserver(127, valueFn);
		param.name().addValueObserver(nameFn)
	}
	// TODO: see if we can get human readable param value

	println("octacon initialized!");
}

function onNameChange(index, value) {
	println("Id: " + index + " Name: " + value);
	let ix = index.toString(16).padStart(2, '0');
	let len = value.length.toString(16).padStart(2, '0');
	value = value.replace(/[^\x00-\x7F]/g, "").trim();
	let name = value.toHex(value.length)
	midiQueue.push({
		type: 'sysex',
		// 00 + <ix> + <len> + <name>
		data: '00 ' + ix + len + name,
	})
}

function onValueChange(index, value) {
	midiQueue.push({
		type: 'cc',
		ix: index,
		value: value,
	});
}

// Called when a short MIDI message is received on MIDI input port 0.
function onMidi0(status, data1, data2) {
   if (!isChannelController(status)) {
      return true;
   }
   if (data1 >= 9 && data1 <= 16) {
      let ix = data1 - 9;
      remoteControlCursor.getParameter(ix).value().set(data2 / 127);
	  // println("got control change: " + data1 + ", " + data2);
   }
	return true;
}

function flush() {
   for (let m of midiQueue) {
		switch (m.type) {
			case 'cc': {
				outPort.sendMidi(0xb0, 9 + m.ix, m.value);
				break;
			}
			case 'sysex': {
				outPort.sendSysex(SYSEX_BEGIN + m.data + SYSEX_END);
				break;
			}
			default: 
				println("unhandled type " + m.type);
		}
	}
	midiQueue = [];
}

function exit() {

}
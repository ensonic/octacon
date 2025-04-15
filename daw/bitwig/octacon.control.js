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
const SYSEX_HELLO = SYSEX_BEGIN + "02 01" + SYSEX_END
const SYSEX_BYE = SYSEX_BEGIN + "02 00" + SYSEX_END
const SYSEX_RATE = 100

const CC_MSB_ValueBase = 9;
const CC_LSB_ValueBase = 9 + 32;


let remoteControlCursor;
let outPort;
let midiQueue = [];
let displayValues = [];
let values = [0, 0, 0, 0, 0, 0, 0, 0];

function init() {
	host.getMidiInPort(0).setMidiCallback(onMidi0);
	outPort = host.getMidiOutPort(0);
	outPort.sendSysex(SYSEX_HELLO);

	// follows UI selection
	let cursorDevice = host.createCursorTrack(0, 0).createCursorDevice();
	remoteControlCursor = cursorDevice.createCursorRemoteControlsPage(8);

	for (let j = 0; j < remoteControlCursor.getParameterCount(); j++) {
		let valueFn = onValueChange.bind(this, j);
		let nameFn = onNameChange.bind(this, j);
		let displayValueFn = onDisplayValueChange.bind(this, j);
		let param = remoteControlCursor.getParameter(j);
		param.markInterested();
		param.setIndication(true);
		param.value().addValueObserver(16384, valueFn);
		param.name().addValueObserver(nameFn)
		param.displayedValue().addValueObserver(displayValueFn);
		displayValues[j] = { prev: "", next: "" };
	}

	// rate limit sending the pretty values, when using automation this can
	// otherwise overload the controller
	host.scheduleTask(queueDisplayValueChanges, SYSEX_RATE);
	println("octacon initialized!");
}

function onValueChange(index, value) {
	//let param = remoteControlCursor.getParameter(index);
	//println("Id: " + index + " PrettyValue: " + param.displayedValue().get());
	midiQueue.push({
		type: 'cc',
		ix: index,
		value: value,
	});
}

function onNameChange(index, value) {
	//println("Id: " + index + " Name: " + value);
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

function onDisplayValueChange(index, value) {
	displayValues[index].next = value;
	//println("Id: " + index + " PrettyValue: " + value);
}

function queueDisplayValueChanges() {
	for (let index = 0; index < displayValues.length; index++) {
		let dv = displayValues[index];
		if (dv.next === dv.prev) {
			continue;
		}
		dv.prev = dv.next
		let ix = index.toString(16).padStart(2, '0');
		let value = dv.next;
		let len = value.length.toString(16).padStart(2, '0');
		value = value.replace(/[^\x00-\x7F]/g, "").trim();
		let name = value.toHex(value.length)
		midiQueue.push({
			type: 'sysex',
			// 01 + <ix> + <len> + <name>
			data: '01 ' + ix + len + name,
		})
	}
	host.scheduleTask(queueDisplayValueChanges, SYSEX_RATE);
}

// Called when a short MIDI message is received on MIDI input port 0.
function onMidi0(status, data1, data2) {
	if (!isChannelController(status)) {
		return true;
	}
	let ix = -1
	if (data1 >= CC_MSB_ValueBase && data1 < CC_MSB_ValueBase+8) {
		ix = data1 - CC_MSB_ValueBase;
		values[ix] = data2 << 7;
	}
	if (data1 >= CC_LSB_ValueBase && data1 < CC_LSB_ValueBase+8) {
		ix = data1 - CC_LSB_ValueBase;
		values[ix] += data2;
	}
	if (ix >= 0) {
		remoteControlCursor.getParameter(ix).value().set(values[ix]/16384.0);
	}
	return true;
}

function flush() {
   for (let m of midiQueue) {
		switch (m.type) {
			case 'cc': {
				outPort.sendMidi(0xb0, CC_MSB_ValueBase + m.ix, m.value >> 7);
				outPort.sendMidi(0xb0, CC_LSB_ValueBase + m.ix, m.value & 127);
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
	outPort.sendSysex(SYSEX_BYE);
}
// Bitwig controller-script for octacon
// cp daw/bitwig/octacon.control.js "$HOME/Bitwig Studio/Controller Scripts/"
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
// Daw-Mode (on/off)
const SYSEX_HELLO = SYSEX_BEGIN + "02 01" + SYSEX_END;
const SYSEX_BYE = SYSEX_BEGIN + "02 00" + SYSEX_END;
const SYSEX_RATE = 100;

const CC_MSB_ValueBase = 9;
const CC_LSB_ValueBase = 9 + 32;
const CC_ButtonBase = 9 + 8;
function Mode() {
	this.values = [0, 0, 0, 0, 0, 0, 0, 0];
}
ctrlMode = new Mode();
navMode = new Mode();

const MODE_CTRL = 0;
const MODE_NAV = 1;
//let modes = [new CtrlMode(), new NavMode()];
let modes = [ctrlMode, navMode];
let mode = MODE_CTRL;

let cursorTrack;
let cursorDevice;
let remoteControlCursor;
let deviceBank;
let trackBank;
let outPort;
let midiQueue = [];
let displayValues = [];
let values = [0, 0, 0, 0, 0, 0, 0, 0];
let pageNames = [];
let pageIx = 0;

let info = {
	track: "",
	device: "",
	page: "",
};

function init() {
	host.getMidiInPort(0).setMidiCallback(onMidi0);
	outPort = host.getMidiOutPort(0);
	outPort.sendSysex(SYSEX_HELLO);

	// active device that follows UI selection
	cursorTrack = host.createCursorTrack(0, 0);
	cursorDevice = cursorTrack.createCursorDevice();
	remoteControlCursor = cursorDevice.createCursorRemoteControlsPage(8);

	let namesToSub = [
		{ name: "track", value: cursorTrack.name()},
		{ name: "device", value: cursorDevice.name()},
		// TODO: this works for device/preset-pages, but not for module/modulator pages ??
		// e.g. open Polymer and select "Union" or "Vibrato"
		{ name: "page", value: remoteControlCursor.getName()}
	];
	for (const nts of namesToSub) {
		nts.value.markInterested();
		nts.value.addValueObserver(onInfoNameChange.bind(this, nts.name));
	} 

	// complex workaround to get all page-names (see TODO above)
	remoteControlCursor.pageNames().addValueObserver(function(value) {
		pageNames = [];
		for(var j = 0; j < value.length; j++) {
         	pageNames[j] = value[j];
     	}
		if (typeof pageNames[pageIx] !== 'undefined') {
			info.page = pageNames[pageIx];
			println("Name[page]: " + pageNames[pageIx]);
			sendInfoString();
		}
		//println("pages: " + pageNames.join(","))
	})
	remoteControlCursor.selectedPageIndex().addValueObserver(function(value) { 
		pageIx = value;
		if (typeof pageNames[pageIx] !== 'undefined') {
			info.page = pageNames[pageIx];
			println("Name[page]: " + pageNames[pageIx]);
			sendInfoString();
		}
	})

	// navigation along the device strip with one device active
	// TODO: also checkout cursorDevice.createSiblingsDeviceBank(1)/createLayerBank()/createDrumPadBank()
	deviceBank = cursorDevice.deviceChain().createDeviceBank(1);
	deviceBank.itemCount().addValueObserver(function(value) {
		println("device-chain-length: " + value);
	})
	cursorDevice.position().addValueObserver(function(value) {
		if (value < 0) {
			return;
		}
		println("cursor-device-pos: " + value);
		deviceBank.scrollIntoView(value+1);
	})

	// active track that follows UI selection
	trackBank = host.createTrackBank(1,1,0, true)
	trackBank.itemCount().addValueObserver(function(value) {
		println("track-bank-length: " + value);
	})
	cursorTrack.position().addValueObserver(function(value) {
		if (value < 0) {
			return;
		}
		// this counts different that the flattened trackBank
		println("cursor-track-pos: " + value);
		// trackBank.scrollIntoView(value+1);
	})

	for (let j = 0; j < remoteControlCursor.getParameterCount(); j++) {
		let param = remoteControlCursor.getParameter(j);
		param.markInterested();
		param.setIndication(true);
		param.value().addValueObserver(16384, onParamValueChange.bind(this, j));
		param.name().addValueObserver(onParamNameChange.bind(this, j))
		param.displayedValue().addValueObserver(onParamDisplayedValueChange.bind(this, j));
		/* also send how many ticks we have
		param.discreteValueCount().addValueObserver(...)
		*/
		displayValues[j] = { prev: "", next: "" };
	}

	// rate limit sending the pretty values, when using automation this can
	// otherwise overload the controller
	host.scheduleTask(queueDisplayValueChanges, SYSEX_RATE);
	println("octacon initialized!");
}

function onParamValueChange(index, value) {
	//let param = remoteControlCursor.getParameter(index);
	//println("Id: " + index + " PrettyValue: " + param.displayedValue().get());
	midiQueue.push({
		type: 'cc',
		ix: index,
		value: value,
	});
}

function onParamNameChange(index, value) {
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

function onParamDisplayedValueChange(index, value) {
	displayValues[index].next = value;
	//println("Id: " + index + " PrettyValue: " + value);
}

function onInfoNameChange(key, value) {
	if (value == "" ) {
		return;
	}
	println("Name[" + key +"]: " + value);
	info[key] = value;
	sendInfoString();
}

function sendInfoString() {
	// rebuild full name & send
	let value = info.track + "/" + info.device + "/" + info.page
	// println("Info: " + value);

	let len = value.length.toString(16).padStart(2, '0');
	value = value.replace(/[^\x00-\x7F]/g, "").trim();
	let name = value.toHex(value.length)
	midiQueue.push({
		type: 'sysex',
		// 04 + <len> + <name>
		data: '04 ' + len + name,
	})
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
	const [ix, isValue] = modes[mode].onMidi0(data1,data2);
	if (ix >= 0 && !isValue && data2 > 0) {
		modeChange();
	}
	return true;
}

function modeChange() {
	mode = 1 - mode;
	// toggle led-color on device
	let modestr = mode.toString(16).padStart(2, '0');
	outPort.sendSysex(SYSEX_BEGIN + "03 " + modestr + SYSEX_END);
	// update octacon state
	for (i = 0;i < 8; i++) {
		// onParamValueChange(i, modes[mode].values[i])
		// onParamNameChange(i, ...)
		// queueDisplayValueChanges(i, ...)
	}
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

function PrepMidi(data1, data2) {
	let ix = -1;
	let isValue = false;
	if (data1 >= CC_MSB_ValueBase && data1 < CC_MSB_ValueBase+8) {
		ix = data1 - CC_MSB_ValueBase;
		this.values[ix] = data2 << 7;
		isValue = true;
	}
	if (data1 >= CC_LSB_ValueBase && data1 < CC_LSB_ValueBase+8) {
		ix = data1 - CC_LSB_ValueBase;
		this.values[ix] += data2;
		isValue = true;
	}
	if (data1 >= CC_ButtonBase && data1 < CC_ButtonBase+8) {
		ix = data1 - CC_ButtonBase;
	}
	return [ix, isValue];
}

ctrlMode.prepMidi = PrepMidi;
ctrlMode.onMidi0 = function(data1, data2) {
	const [ix, isValue] = this.prepMidi(data1,data2);
	if (ix >= 0 &&  isValue) {
		println("ctrl.knob[" + ix + "]=" + this.values[ix]/16384.0);
		remoteControlCursor.getParameter(ix).value().set(values[ix]/16384.0);
	}
	return [ix, isValue];
};

navMode.prepMidi = PrepMidi;
navMode.onMidi0 = function(data1, data2) {
	const [ix, isValue] = this.prepMidi(data1,data2);
	if (ix >= 0 && isValue) {
		println("nav.knob[" + ix + "]=" + this.values[ix]/16384.0);
		/* TODO: implement nav-mode:
		  - don't queue midi messages from bitwig param changes (only update
			internal state), see onParamValueChange, onParamNameChange,
			queueDisplayValueChanges
		*/

	}
	return [ix, isValue];
};

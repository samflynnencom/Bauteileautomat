/*
 * dspin.c
 *
 *  Created on: 19.10.2013
 *      Author: einball
 */

#include "inc_dspin.h"


#define DEVICE_COUNT	6 /* Up to 8 motors */

/************************************************/
/*
 * Description:
 * This function writes a uint8_t to the SPI chain
 * and therefore receives a uint8_t from the chain.
 * It is not used directly by the user, but helps
 * enhancing the readability of the code. This
 * will be called multiple times and is the only
 * function that depends on another library.
 *
 * Arguments:
 * int device - number of the device in the chain
 * uint8_t data - the uint8_t to be transferred
 *
 * Returns:
 * uint8_t reply - 8 bit of reply
 */
/************************************************/
uint8_t dSPIN_Xfer(int device, uint8_t data) {
	uint8_t reply = 0;
	uint32_t SSIPortBase = 0x00000000;
	int i = 0;

	/* ACHTUNG!
	 * Ich kann nicht garantieren, dass das funktioniert. Das sollte man
	 * vielleicht nochmal überdenken, da das SPI nur gewisse Modi supported!
	 *
	 * Könnte sein, dass wir eine Software SPI implementieren müssen, bzw
	 * bitbangen müssen ... Wäre bescheiden, ist nicht zu vermeiden!
	 *
	 * Auf jeden Fall muss alles bis auf das device ein NOP erhalten und auch
	 * nur das Device, welches den Befehl erhalten hat, sendet eine Antwort.
	 * Das bedeutet, wir müssen die Antwort noch vom Datenmüll (NOPs) separieren!
	 */
	for (i = DEVICE_COUNT; i > 0; --i) {
		if (i = device) {
			SoftSSI_DataPut(SSIPortBase, data);
		} else {
			SoftSSI_DataPut(SSIPortBase, dSPIN_NOP);
		}
	}

	SoftSSI_Enable(SSIPortBase);

	while (SoftSSI_Busy(SSIPortBase))
		;

	for (i = DEVICE_COUNT; i > 0; --i) {
		if (i = device)
			reply = SoftSSI_DataGet(SSIPortBase);
	}

	return reply;
}

/************************************************/
/*
 * Description:
 * This function writes the data and gives back
 * a 32 bit value that holds 0 to 3 return uint8_ts
 * depending on the command that has been sent to
 * the SPI chain
 */
/************************************************/

uint32_t dSPIN_Write(int device, uint32_t data, uint8_t bitLen) {
	unsigned long retVal = 0;   // We'll return this to generalize this function
	//  for both read and write of registers.
	uint8_t uint8_tLen = bitLen / 8;      // How many uint8_tS do we have?
	if (bitLen % 8 > 0)
		uint8_tLen++;  // Make sure not to lose any partial uint8_t values.
	// Let's make sure our value has no spurious bits set, and if the value was too
	//  high, max it out.
	unsigned long mask = 0xffffffff >> (32 - bitLen);
	if (data > mask)
		data = mask;
	// The following three if statements handle the various possible uint8_t length
	//  transfers- it'll be no less than 1 but no more than 3 uint8_ts of data.
	// dSPIN_Xfer() sends a uint8_t out through SPI and returns a uint8_t received
	//  over SPI- when calling it, we typecast a shifted version of the masked
	//  value, then we shift the received value back by the same amount and
	//  store it until return time.
	if (uint8_tLen == 3) {
		retVal |= dSPIN_Xfer(device, (uint8_t) (data >> 16)) << 16;
	}
	if (uint8_tLen >= 2) {
		retVal |= dSPIN_Xfer(device, (uint8_t) (data >> 8)) << 8;
	}
	if (uint8_tLen >= 1) {
		retVal |= dSPIN_Xfer(device, (uint8_t) data);
	}
	// Return the received values. Mask off any unnecessary bits, just for
	//  the sake of thoroughness- we don't EXPECT to see anything outside
	//  the bit length range but better to be safe than sorry.
	return (retVal & mask);
}

//dSPIN_commands.ino - Contains high-level command implementations- movement
//   and configuration commands, for example.

// Realize the "set parameter" function, to write to the various registers in
//  the dSPIN chip.
void dSPIN_SetParam(int device, uint8_t param, uint32_t value) {
	dSPIN_Xfer(device, dSPIN_SET_PARAM | param);
	dSPIN_ParamHandler(device, param, value);
}

// Realize the "get parameter" function, to read from the various registers in
//  the dSPIN chip.
unsigned long dSPIN_GetParam(int device, uint8_t param) {
	dSPIN_Xfer(device, dSPIN_GET_PARAM | param);
	return dSPIN_ParamHandler(device, param, 0);
}

// Much of the functionality between "get parameter" and "set parameter" is
//  very similar, so we deal with that by putting all of it in one function
//  here to save memory space and simplify the program.
unsigned long dSPIN_ParamHandler(int device, uint8_t param, unsigned long value) {
	unsigned long ret_val = 0;   // This is a temp for the value to return.
	// This switch structure handles the appropriate action for each register.
	//  This is necessary since not all registers are of the same length, either
	//  bit-wise or uint8_t-wise, so we want to make sure we mask out any spurious
	//  bits and do the right number of transfers. That is handled by the dSPIN_Param()
	//  function, in most cases, but for 1-uint8_t or smaller transfers, we call
	//  dSPIN_Xfer() directly.
	switch (param) {
	// ABS_POS is the current absolute offset from home. It is a 22 bit number expressed
	//  in two's complement. At power up, this value is 0. It cannot be written when
	//  the motor is running, but at any other time, it can be updated to change the
	//  interpreted position of the motor.
	case dSPIN_ABS_POS:
		ret_val = dSPIN_Param(device, value, 22);
		break;
		// EL_POS is the current electrical position in the step generation cycle. It can
		//  be set when the motor is not in motion. Value is 0 on power up.
	case dSPIN_EL_POS:
		ret_val = dSPIN_Param(device, value, 9);
		break;
		// MARK is a second position other than 0 that the motor can be told to go to. As
		//  with ABS_POS, it is 22-bit two's complement. Value is 0 on power up.
	case dSPIN_MARK:
		ret_val = dSPIN_Param(device, value, 22);
		break;
		// SPEED contains information about the current speed. It is read-only. It does
		//  NOT provide direction information.
	case dSPIN_SPEED:
		ret_val = dSPIN_Param(device, 0, 20);
		break;
		// ACC and DEC set the acceleration and deceleration rates. Set ACC to 0xFFF
		//  to get infinite acceleration/decelaeration- there is no way to get infinite
		//  deceleration w/o infinite acceleration (except the HARD STOP command).
		//  Cannot be written while motor is running. Both default to 0x08A on power up.
		// AccCalc() and DecCalc() functions exist to convert steps/s/s values into
		//  12-bit values for these two registers.
	case dSPIN_ACC:
		ret_val = dSPIN_Param(device, value, 12);
		break;
	case dSPIN_DEC:
		ret_val = dSPIN_Param(device, value, 12);
		break;
		// MAX_SPEED is just what it says- any command which attempts to set the speed
		//  of the motor above this value will simply cause the motor to turn at this
		//  speed. Value is 0x041 on power up.
		// MaxSpdCalc() function exists to convert steps/s value into a 10-bit value
		//  for this register.
	case dSPIN_MAX_SPEED:
		ret_val = dSPIN_Param(device, value, 10);
		break;
		// MIN_SPEED controls two things- the activation of the low-speed optimization
		//  feature and the lowest speed the motor will be allowed to operate at. LSPD_OPT
		//  is the 13th bit, and when it is set, the minimum allowed speed is automatically
		//  set to zero. This value is 0 on startup.
		// MinSpdCalc() function exists to convert steps/s value into a 12-bit value for this
		//  register. SetLSPDOpt() function exists to enable/disable the optimization feature.
	case dSPIN_MIN_SPEED:
		ret_val = dSPIN_Param(device, value, 12);
		break;
		// FS_SPD register contains a threshold value above which microstepping is disabled
		//  and the dSPIN operates in full-step mode. Defaults to 0x027 on power up.
		// FSCalc() function exists to convert steps/s value into 10-bit integer for this
		//  register.
	case dSPIN_FS_SPD:
		ret_val = dSPIN_Param(device, value, 10);
		break;
		// KVAL is the maximum voltage of the PWM outputs. These 8-bit values are ratiometric
		//  representations: 255 for full output voltage, 128 for half, etc. Default is 0x29.
		// The implications of different KVAL settings is too complex to dig into here, but
		//  it will usually work to max the value for RUN, ACC, and DEC. Maxing the value for
		//  HOLD may result in excessive power dissipation when the motor is not running.
	case dSPIN_KVAL_HOLD:
		ret_val = dSPIN_Xfer(device, (uint8_t) value);
		break;
	case dSPIN_KVAL_RUN:
		ret_val = dSPIN_Xfer(device, (uint8_t) value);
		break;
	case dSPIN_KVAL_ACC:
		ret_val = dSPIN_Xfer(device, (uint8_t) value);
		break;
	case dSPIN_KVAL_DEC:
		ret_val = dSPIN_Xfer(device, (uint8_t) value);
		break;
		// INT_SPD, ST_SLP, FN_SLP_ACC and FN_SLP_DEC are all related to the back EMF
		//  compensation functionality. Please see the datasheet for details of this
		//  function- it is too complex to discuss here. Default values seem to work
		//  well enough.
	case dSPIN_INT_SPD:
		ret_val = dSPIN_Param(device, value, 14);
		break;
	case dSPIN_ST_SLP:
		ret_val = dSPIN_Xfer(device, (uint8_t) value);
		break;
	case dSPIN_FN_SLP_ACC:
		ret_val = dSPIN_Xfer(device, (uint8_t) value);
		break;
	case dSPIN_FN_SLP_DEC:
		ret_val = dSPIN_Xfer(device, (uint8_t) value);
		break;
		// K_THERM is motor winding thermal drift compensation. Please see the datasheet
		//  for full details on operation- the default value should be okay for most users.
	case dSPIN_K_THERM:
		ret_val = dSPIN_Xfer(device, (uint8_t) value & 0x0F);
		break;
		// ADC_OUT is a read-only register containing the result of the ADC measurements.
		//  This is less useful than it sounds; see the datasheet for more information.
	case dSPIN_ADC_OUT:
		ret_val = dSPIN_Xfer(device, 0);
		break;
		// Set the overcurrent threshold. Ranges from 375mA to 6A in steps of 375mA.
		//  A set of defined constants is provided for the user's convenience. Default
		//  value is 3.375A- 0x08. This is a 4-bit value.
	case dSPIN_OCD_TH:
		ret_val = dSPIN_Xfer(device, (uint8_t) value & 0x0F);
		break;
		// Stall current threshold. Defaults to 0x40, or 2.03A. Value is from 31.25mA to
		//  4A in 31.25mA steps. This is a 7-bit value.
	case dSPIN_STALL_TH:
		ret_val = dSPIN_Xfer(device, (uint8_t) value & 0x7F);
		break;
		// STEP_MODE controls the microstepping settings, as well as the generation of an
		//  output signal from the dSPIN. Bits 2:0 control the number of microsteps per
		//  step the part will generate. Bit 7 controls whether the BUSY/SYNC pin outputs
		//  a BUSY signal or a step synchronization signal. Bits 6:4 control the frequency
		//  of the output signal relative to the full-step frequency; see datasheet for
		//  that relationship as it is too complex to reproduce here.
		// Most likely, only the microsteps per step value will be needed; there is a set
		//  of constants provided for ease of use of these values.
	case dSPIN_STEP_MODE:
		ret_val = dSPIN_Xfer(device, (uint8_t) value);
		break;
		// ALARM_EN controls which alarms will cause the FLAG pin to fall. A set of constants
		//  is provided to make this easy to interpret. By default, ALL alarms will trigger the
		//  FLAG pin.
	case dSPIN_ALARM_EN:
		ret_val = dSPIN_Xfer(device, (uint8_t) value);
		break;
		// CONFIG contains some assorted configuration bits and fields. A fairly comprehensive
		//  set of reasonably self-explanatory constants is provided, but users should refer
		//  to the datasheet before modifying the contents of this register to be certain they
		//  understand the implications of their modifications. Value on boot is 0x2E88; this
		//  can be a useful way to verify proper start up and operation of the dSPIN chip.
	case dSPIN_CONFIG:
		ret_val = dSPIN_Param(device, value, 16);
		break;
		// STATUS contains read-only information about the current condition of the chip. A
		//  comprehensive set of constants for masking and testing this register is provided, but
		//  users should refer to the datasheet to ensure that they fully understand each one of
		//  the bits in the register.
	case dSPIN_STATUS:  // STATUS is a read-only register
		ret_val = dSPIN_Param(device, 0, 16);
		break;
	default:
		ret_val = dSPIN_Xfer(device, (uint8_t) (value));
		break;
	}
	return ret_val;
}

// Enable or disable the low-speed optimization option. If enabling,
//  the other 12 bits of the register will be automatically zero.
//  When disabling, the value will have to be explicitly written by
//  the user with a SetParam() call. See the datasheet for further
//  information about low-speed optimization.
void SetLSPDOpt(int device, uint8_t enable, unsigned long speed) {
	dSPIN_Xfer(device, dSPIN_SET_PARAM | dSPIN_MIN_SPEED);
	if (enable)
		dSPIN_SetParam(device, dSPIN_MIN_SPEED, 0x1000 && MinSpd_Steps_to_Par(speed));
	else
		dSPIN_SetParam(device, dSPIN_MIN_SPEED, MinSpd_Steps_to_Par(speed));
}

// RUN sets the motor spinning in a direction (defined by the constants
//  FWD and REV). Maximum speed and minimum speed are defined
//  by the MAX_SPEED and MIN_SPEED registers; exceeding the FS_SPD value
//  will switch the device into full-step mode.
// The SpdCalc() function is provided to convert steps/s values into
//  appropriate integer values for this function.
void dSPIN_Run(int device, uint8_t dir, unsigned long spd) {
	dSPIN_Xfer(device, dSPIN_RUN | dir);
	if (spd > 0xFFFFF)
		spd = 0xFFFFF;
	dSPIN_Xfer(device, (uint8_t) (spd >> 16));
	dSPIN_Xfer(device, (uint8_t) (spd >> 8));
	dSPIN_Xfer(device, (uint8_t) (spd));
}

// STEP_CLOCK puts the device in external step clocking mode. When active,
//  pin 25, STCK, becomes the step clock for the device, and steps it in
//  the direction (set by the FWD and REV constants) imposed by the call
//  of this function. Motion commands (RUN, MOVE, etc) will cause the device
//  to exit step clocking mode.
void dSPIN_Step_Clock(int device, uint8_t dir) {
	dSPIN_Xfer(device, dSPIN_STEP_CLOCK | dir);
}

// MOVE will send the motor n_step steps (size based on step mode) in the
//  direction imposed by dir (FWD or REV constants may be used). The motor
//  will accelerate according the acceleration and deceleration curves, and
//  will run at MAX_SPEED. Stepping mode will adhere to FS_SPD value, as well.
void dSPIN_Move(int device, uint8_t dir, unsigned long n_step) {
	dSPIN_Xfer(device, dSPIN_MOVE | dir);
	if (n_step > 0x3FFFFF)
		n_step = 0x3FFFFF;
	dSPIN_Xfer(device, (uint8_t) (n_step >> 16));
	dSPIN_Xfer(device, (uint8_t) (n_step >> 8));
	dSPIN_Xfer(device, (uint8_t) (n_step));
}

// GOTO operates much like MOVE, except it produces absolute motion instead
//  of relative motion. The motor will be moved to the indicated position
//  in the shortest possible fashion.
void dSPIN_GoTo(int device, unsigned long pos) {

	dSPIN_Xfer(device, dSPIN_GO_TO);
	if (pos > 0x3FFFFF)
		pos = 0x3FFFFF;
	dSPIN_Xfer(device, (uint8_t) (pos >> 16));
	dSPIN_Xfer(device, (uint8_t) (pos >> 8));
	dSPIN_Xfer(device, (uint8_t) (pos));
}

// Same as GOTO, but with user constrained rotational direction.
void dSPIN_GoTo_DIR(int device, uint8_t dir, unsigned long pos) {

	dSPIN_Xfer(device, dSPIN_GO_TO_DIR);
	if (pos > 0x3FFFFF)
		pos = 0x3FFFFF;
	dSPIN_Xfer(device, (uint8_t) (pos >> 16));
	dSPIN_Xfer(device, (uint8_t) (pos >> 8));
	dSPIN_Xfer(device, (uint8_t) (pos));
}

// GoUntil will set the motor running with direction dir (REV or
//  FWD) until a falling edge is detected on the SW pin. Depending
//  on bit SW_MODE in CONFIG, either a hard stop or a soft stop is
//  performed at the falling edge, and depending on the value of
//  act (either RESET or COPY) the value in the ABS_POS register is
//  either RESET to 0 or COPY-ed into the MARK register.
void dSPIN_GoUntil(int device, uint8_t act, uint8_t dir, unsigned long spd) {
	dSPIN_Xfer(device, dSPIN_GO_UNTIL | act | dir);
	if (spd > 0x3FFFFF)
		spd = 0x3FFFFF;
	dSPIN_Xfer(device, (uint8_t) (spd >> 16));
	dSPIN_Xfer(device, (uint8_t) (spd >> 8));
	dSPIN_Xfer(device, (uint8_t) (spd));
}

// Similar in nature to GoUntil, ReleaseSW produces motion at the
//  higher of two speeds: the value in MIN_SPEED or 5 steps/s.
//  The motor continues to run at this speed until a rising edge
//  is detected on the switch input, then a hard stop is performed
//  and the ABS_POS register is either COPY-ed into MARK or RESET to
//  0, depending on whether RESET or COPY was passed to the function
//  for act.
void dSPIN_ReleaseSW(int device, uint8_t act, uint8_t dir) {
	dSPIN_Xfer(device, dSPIN_RELEASE_SW | act | dir);
}

// GoHome is equivalent to GoTo(0), but requires less time to send.
//  Note that no direction is provided; motion occurs through shortest
//  path. If a direction is required, use GoTo_DIR().
void dSPIN_GoHome(int device) {
	dSPIN_Xfer(device, dSPIN_GO_HOME);
}

// GoMark is equivalent to GoTo(MARK), but requires less time to send.
//  Note that no direction is provided; motion occurs through shortest
//  path. If a direction is required, use GoTo_DIR().
void dSPIN_GoMark(int device) {
	dSPIN_Xfer(device, dSPIN_GO_MARK);
}

// Sets the ABS_POS register to 0, effectively declaring the current
//  position to be "HOME".
void dSPIN_ResetPos(int device) {
	dSPIN_Xfer(device, dSPIN_RESET_POS);
}

// Reset device to power up conditions. Equivalent to toggling the STBY
//  pin or cycling power.
void dSPIN_ResetDev(int device) {
	dSPIN_Xfer(device, dSPIN_RESET_DEVICE);
}

// Bring the motor to a halt using the deceleration curve.
void dSPIN_SoftStop(int device) {
	dSPIN_Xfer(device, dSPIN_SOFT_STOP);
}

// Stop the motor with infinite deceleration.
void dSPIN_HardStop(int device) {
	dSPIN_Xfer(device, dSPIN_HARD_STOP);
}

// Decelerate the motor and put the bridges in Hi-Z state.
void dSPIN_SoftHiZ(int device) {
	dSPIN_Xfer(device, dSPIN_SOFT_HIZ);
}

// Put the bridges in Hi-Z state immediately with no deceleration.
void dSPIN_HardHiZ(int device) {
	dSPIN_Xfer(device, dSPIN_HARD_HIZ);
}

// Fetch and return the 16-bit value in the STATUS register. Resets
//  any warning flags and exits any error states. Using GetParam()
//  to read STATUS does not clear these values.
int dSPIN_GetStatus(int device) {
	int temp = 0;
	dSPIN_Xfer(device, dSPIN_GET_STATUS);
	temp = dSPIN_Xfer(device, 0) << 8;
	temp |= dSPIN_Xfer(device, 0);
	return temp;
}

// Generalization of the subsections of the register read/write functionality.
//  We want the end user to just write the value without worrying about length,
//  so we pass a bit length parameter from the calling function.
unsigned long dSPIN_Param(int device, unsigned long value, uint8_t bit_len) {
	unsigned long ret_val = 0;      // We'll return this to generalize this function
	//  for both read and write of registers.
	uint8_t uint8_t_len = bit_len / 8;      // How many uint8_tS do we have?
	if (bit_len % 8 > 0)
		uint8_t_len++;  // Make sure not to lose any partial uint8_t values.
	// Let's make sure our value has no spurious bits set, and if the value was too
	//  high, max it out.
	unsigned long mask = 0xffffffff >> (32 - bit_len);
	if (value > mask)
		value = mask;
	// The following three if statements handle the various possible uint8_t length
	//  transfers- it'll be no less than 1 but no more than 3 uint8_ts of data.
	// dSPIN_Xfer() sends a uint8_t out through SPI and returns a uint8_t received
	//  over SPI- when calling it, we typecast a shifted version of the masked
	//  value, then we shift the received value back by the same amount and
	//  store it until return time.
	if (uint8_t_len == 3) {
		ret_val |= dSPIN_Xfer(device, (uint8_t) (value >> 16)) << 16;
		//Serial.println(ret_val, HEX);
	}
	if (uint8_t_len >= 2) {
		ret_val |= dSPIN_Xfer(device, (uint8_t) (value >> 8)) << 8;
		//Serial.println(ret_val, HEX);
	}
	if (uint8_t_len >= 1) {
		ret_val |= dSPIN_Xfer(device, (uint8_t) value);
		//Serial.println(ret_val, HEX);
	}
	// Return the received values. Mask off any unnecessary bits, just for
	//  the sake of thoroughness- we don't EXPECT to see anything outside
	//  the bit length range but better to be safe than sorry.
	return (ret_val & mask);
}

uint8_t SoftSSI_DataPut(uint32_t SSIPortBase, uint8_t data){

	return 0;
}
uint8_t SoftSSI_DataGet(uint32_t SSIPortBase){return 0;}
uint8_t	SoftSSI_Busy(uint32_t SSIPortBase){return 0;}

uint8_t SoftSSI_Enable(uint32_t SSIPortBase){return 0;}


#include <AUXYKR121EHeatpumpIR.h>

// Timings inherited from ToniA's AUXHeatpumpIR (not yet measured against YKR-T/121E).
#define AUX121_HDR_MARK 8800
#define AUX121_HDR_SPACE 4580
#define AUX121_BIT_MARK 490
#define AUX121_ZERO_SPACE 620
#define AUX121_ONE_SPACE 1740

AUXYKR121EHeatpumpIR::AUXYKR121EHeatpumpIR() : HeatpumpIR() {
  static const char model[] PROGMEM = "AUX YKR-T/121E";
  static const char info[] PROGMEM = "{\"mdl\":\"aux121\",\"dn\":\"AUX YKR-T/121E\",\"mT\":16,\"xT\":32,\"fs\":4}";
  _model = model;
  _info = info;
}

void AUXYKR121EHeatpumpIR::send(IRSender& IR, uint8_t powerModeCmd,
    uint8_t operatingModeCmd, uint8_t fanSpeedCmd, uint8_t temperatureCmd,
    uint8_t swingVCmd, uint8_t swingHCmd) {
  // Frame indices are zero-based. The final byte is the additive checksum.
  // Byte 11 is an unresolved status byte; 0x45 matches controlled HEAT/AUTO
  // captures but may need adaptation for other remote states.
  uint8_t frame[13] = {0xC3, 0x87, 0xE0, 0x00, 0xA0, 0x00,
                       0x80, 0x00, 0x00, 0x30, 0x00, 0x45, 0x00};
  uint8_t mode = 0x80;
  switch (operatingModeCmd) {
    case MODE_AUTO: mode = 0x00; break;
    case MODE_COOL: mode = 0x20; break;
    case MODE_DRY:  mode = 0x40; break;
    case MODE_FAN:  mode = 0xC0; break;
    case MODE_HEAT: default: mode = 0x80; break;
  }
  frame[6] = mode;
  frame[9] = (powerModeCmd == POWER_OFF) ? 0x00 : (mode == 0x80 ? 0x30 : 0x20);
  // In AUTO/FAN the original remote does not expose a temperature setting.
  if (mode == 0x00 || mode == 0xC0) {
    frame[1] = 0x07;
  } else {
    if (temperatureCmd < 16) temperatureCmd = 16;
    if (temperatureCmd > 32) temperatureCmd = 32;
    frame[1] = (uint8_t)(0x47 + (temperatureCmd - 16) * 8);
  }
  // Remote has no fan selection in AUTO; DRY is fixed LOW.
  if (mode == 0x00) {
    frame[4] = 0xA0;
  } else if (mode == 0x40) {
    frame[4] = 0x60;
    frame[11] = 0x40; // Seen in DRY captures; other status bits TBD.
  } else {
    switch (fanSpeedCmd) {
      case FAN_1: frame[4] = 0x60; break;
      case FAN_2: frame[4] = 0x40; break;
      case FAN_3: frame[4] = 0x20; break;
      case FAN_AUTO: default: frame[4] = (mode == 0xC0) ? 0x60 : 0xA0; break;
    }
  }
  // In controlled captures, these bits are set when the swing is OFF.
  frame[1] = (uint8_t)((frame[1] & 0xF8) | ((swingVCmd == VDIR_SWING) ? 0x00 : 0x07));
  frame[2] = (swingHCmd == HDIR_SWING) ? 0x00 : 0xE0;
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < 12; ++i) checksum += frame[i];
  frame[12] = checksum;
  IR.setFrequency(38);
  IR.mark(AUX121_HDR_MARK);
  IR.space(AUX121_HDR_SPACE);
  for (uint8_t i = 0; i < 13; ++i)
    IR.sendIRbyte(frame[i], AUX121_BIT_MARK, AUX121_ZERO_SPACE, AUX121_ONE_SPACE);
  IR.mark(AUX121_BIT_MARK);
  IR.space(0);
}

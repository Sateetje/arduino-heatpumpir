/* AUX YKR-T/121E remote - experimental HeatpumpIR implementation.
 * Based on AUXHeatpumpIR; validate transmitted waveforms against captures.
 */
#ifndef AUXYKR121EHeatpumpIR_h
#define AUXYKR121EHeatpumpIR_h
#include <HeatpumpIR.h>

class AUXYKR121EHeatpumpIR : public HeatpumpIR {
 public:
  using HeatpumpIR::send;
  AUXYKR121EHeatpumpIR();
  void send(IRSender& IR, uint8_t powerModeCmd, uint8_t operatingModeCmd,
            uint8_t fanSpeedCmd, uint8_t temperatureCmd,
            uint8_t swingVCmd, uint8_t swingHCmd);
};
#endif

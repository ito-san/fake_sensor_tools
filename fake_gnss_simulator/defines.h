#ifndef FAKE_GNSS_SIMULATOR_DEFINES_H_
#define FAKE_GNSS_SIMULATOR_DEFINES_H_

/**
 * @file defines.h
 * @brief Defines
 */

typedef enum {
  A_STATUS_INIT = 0,
  A_STATUS_DONTKNOW,
  A_STATUS_OK,
  A_STATUS_SHORT,
  A_STATUS_OPEN,
} AStatus;

typedef enum {
  JAMMING_STATE_UNKNOWN = 0,
  JAMMING_STATE_OK,
  JAMMING_STATE_WARNING,
  JAMMING_STATE_CRITICAL,
} JammingState;

typedef enum {
  SPOOF_DET_STATE_UNKNOWN = 0,
  SPOOF_DET_STATE_NO_SPOOFING,
  SPOOF_DET_STATE_SPOOFING,
  POOF_DET_STATE_MULTIPLE_SPOOFING,
} SpoofDetState;

typedef enum {
  PORT_ID_I2C = 0,
  PORT_ID_UART1,
  PORT_ID_UART2,
  PORT_ID_USB,
  PORT_ID_SPI,
} PortId;

typedef struct
{
  int port_enabled;
  int tx_usage;
} PortBlock;

#endif  // FAKE_GNSS_SIMULATOR_DEFINES_H_

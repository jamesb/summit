#include <pebble.h>
#include "Model.h"


struct Model {
  // Data members
  char*    currencySymbol;         ///< eg. "$" for dollar, "¥" for yen, "£" for pounds (these are multi-byte character strings)
  uint32_t defaultMilliHourly;     ///< default hourly rate of an attendee (in thousandths of currency units, eg. 50000 = $50/hr)
  uint16_t numAttendees;           ///< number of persons currently attending the meeting
  uint32_t totalMilliHourly;       ///< total hourly rate of attendees (in thousandths of currency units, eg. 50000 = $50/hr)
  time_t   lastRateChangeTS;       ///< timestamp when we last changed the total salary
  uint32_t lastRateMilliCost;      ///< frozen cost of the meeting at the last rate change, in thousandths of currency units, eg. 12345678 = $12,345.678
  uint32_t currentRateMilliCost;   ///< running cost of the meeting time since the last rate change, in thousandths of currency units, eg. 12345678 = $12,345.678
  uint32_t totalMeetingMilliCost;  ///< running cost of the entire meeting (lastRateCost + currentRateCost), in thousandths of currency units, eg. 123456789 = $12,345.678

  Model_State status;              ///< state of the model (meeting started, stopped, reset)
};


MagPebApp_ErrCode Model_init(Model* this);
MagPebApp_ErrCode Model_calculateCost(Model* this);

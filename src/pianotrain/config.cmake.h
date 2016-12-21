#ifndef PIANOTRAIN_CONFIG_H_
#define PIANOTRAIN_CONFIG_H_

// This file is the very first file included by virtually all files in pianotrain, so it is also used as a
// general-purpose file for global definitions.

#include <QtCore/QMetaType>
#include <nxcommon/nxcommon_stdint.h>


Q_DECLARE_METATYPE(uint8_t)
Q_DECLARE_METATYPE(uint16_t)
Q_DECLARE_METATYPE(uint32_t)
Q_DECLARE_METATYPE(uint64_t)

Q_DECLARE_METATYPE(int8_t)
Q_DECLARE_METATYPE(int16_t)
Q_DECLARE_METATYPE(int32_t)
Q_DECLARE_METATYPE(int64_t)

#endif /* PIANOTRAIN_CONFIG_H_ */

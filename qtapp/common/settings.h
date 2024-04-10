#ifndef QTAPP_COMMON_SETTINGS_H_
#define QTAPP_COMMON_SETTINGS_H_

#include <QSettings>

#include "qtapp_common_export.h"

namespace qtapp {
namespace common {

/**
 * @brief Initialize QSettings to support JSON as default format.
 *
 * This function should be called when application is about to startup.
 */
QTAPP_COMMON_EXPORT bool InitSettings();

QTAPP_COMMON_EXPORT QSettings::Format GetSettingsFormat();

}  // namespace common
}  // namespace qtapp

#endif  // QTAPP_COMMON_SETTINGS_H_

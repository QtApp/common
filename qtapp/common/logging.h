#ifndef QTAPP_COMMON_LOGGING_H_
#define QTAPP_COMMON_LOGGING_H_
#include "qtapp_common_export.h"
#include "spdlog/spdlog.h"

namespace qtapp {
namespace common {

/**
 * @brief Start the logging system.
 *
 * Initialize the logging system according to the parameters written in the
 * application's QSettings file. Call this function before calling any Qt
 * logging macros like ```qDebug(), qInfo(), qWarning(), qCritical()``` ...
 *
 * Support settings (all the features have a default value):
 * - logs/level: minimum output logging level.
 * - logs/use_msvc: add a sink to output to visual studio output window.
 * - logs/pattern: message pattern used by file logging backend (current backend is spdlog).
 */
QTAPP_COMMON_EXPORT bool StartLogging();

}  // namespace common
}  // namespace qtapp
#endif  // QTAPP_COMMON_LOGGING_H_

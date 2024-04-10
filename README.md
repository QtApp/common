# QtAppCommon
Common tools library used in Qt industrial applications.

## Logging
Logging is always needed in any kinds of industrial applications. QtAppCommon library provides a logging API that:
- Support severity level.
- Can be call in different modules and different threads.
- Create a log file every day, archive all the log files every month.
- Use Qt's logging interface: ```qDebug(), qInfo(), qWarning(), qCritical(), qFatal()```.

## Settings
[QSettings][1] is used to store application's settings. It already supports the formats below: INI, property file and system registry. As the JSON format becoming more and more popular in these days, QtAppCommon library adds the JSON support in [QSettings][1].

# Build
- Set Qt6_DIR and append it to PATH enviroment variable

[1]: https://doc.qt.io/qt-6/qsettings.html


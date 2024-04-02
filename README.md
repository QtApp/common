# QtAppCommon
Common tools library used in Qt industrial applications.

## Tools

### Logging
Logging is always needed in any kinds of industrial applications. QtAppCommon library provides a logging API that:
- Support severity level.
- Can be call in different modules and different threads.
- Create a log file every day, archive all the log files every month.
- Use Qt's logging interface: ```qDebug(), qInfo(), qWarning(), qCritical(), qFatal()```.


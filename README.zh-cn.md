# QtAppCommon
Qt工业应用程序中通用的工具库。

## 日志
日志功能在工业应用程序中必不可少。QtAppCommon库提供的日志API有如下特征
- 支持严重等级；
- 能够在不同的模块、线程中调用；
- 每日创建日志文件，每月压缩归档当月的日志文件。
- 使用Qt的```qDebug(), qInfo(), qWarning(), qCritical(), qFatal()``` 接口

## 设置
[QSettings][1]是Qt程序中要来存储程序配置的类。它支持INI、property 文件和Windows系统注册表格式。随着JSON格式越来越流行，QtAppCommon库在[QSettings][1]中添加了对JSON的支持。

# 构建
- 正确设置Qt6_DIR，并将其添加到PATH环境变量中

[1]: https://doc.qt.io/qt-6/qsettings.html


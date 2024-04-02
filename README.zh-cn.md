# QtAppCommon
Qt工业应用程序中通用的工具库。

## 工具

### 日志
日志功能在工业应用程序中必不可少。QtAppCommon库提供的日志API有如下特征
- 支持严重等级；
- 能够在不同的模块、线程中调用；
- 每日创建日志文件，每月压缩归档当月的日志文件。
- 使用Qt的```qDebug(), qInfo(), qWarning(), qCritical(), qFatal()``` 接口


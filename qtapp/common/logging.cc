#include "qtapp/common/logging.h"

#include <QtCore>
#include <vector>

#include "minizip/zip.h"
#include "spdlog/async.h"
#include "spdlog/async_logger.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/msvc_sink.h"

bool CheckDir() {
  QDir dir(QCoreApplication::applicationDirPath());
  if (!dir.exists("logs")) {
    dir.mkdir("logs");
  }

  if (!dir.cd("logs")) {
    return false;
  }

  if (!dir.exists("archived")) {
    dir.mkdir("archived");
  }

  return dir.cd("archived");
}

bool ArchiveFiles(const QFileInfoList& files,
                  const QString& archive_file_name) {
  zipFile zip_file =
      zipOpen(archive_file_name.toStdString().c_str(), APPEND_STATUS_CREATE);
  if (zip_file == NULL) {
    return false;
  }

  for (int i = 0; i < files.size(); ++i) {
    zip_fileinfo zipinfo;
    QDateTime create_time = files[i].birthTime();
    zipinfo.tmz_date.tm_year = create_time.date().year();
    zipinfo.tmz_date.tm_mon = create_time.date().month();
    zipinfo.tmz_date.tm_mday = create_time.date().day();
    zipinfo.tmz_date.tm_hour = create_time.time().hour();
    zipinfo.tmz_date.tm_min = create_time.time().minute();
    zipinfo.tmz_date.tm_sec = create_time.time().second();
    zipinfo.dosDate = 0;
    zipinfo.external_fa = zipinfo.internal_fa = 0;
    zipOpenNewFileInZip(zip_file, files[i].fileName().toStdString().c_str(),
                        &zipinfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED,
                        Z_DEFAULT_COMPRESSION);

    QFile file(files[i].filePath());
    if (!file.open(QIODevice::ReadOnly)) {
      zipCloseFileInZip(zip_file);
      continue;
    }
    QByteArray buffer = file.readAll();
    zipWriteInFileInZip(zip_file, buffer.data(), buffer.size());
    zipCloseFileInZip(zip_file);
  }

  zipClose(zip_file, NULL);
  return true;
}

void ArchiveLastMonthLogFiles() {
  QDir app_dir(QCoreApplication::applicationDirPath());
  QDir logs_dir(app_dir);
  logs_dir.cd("logs");
  QDir archive_dir(logs_dir);
  archive_dir.cd("archived");

  // list log files
  QDate this_month = QDate::currentDate();
  QDate last_month = this_month.addMonths(-1);
  QString last_month_name = last_month.toString("yyyy.MM.zip");
  if (!archive_dir.exists(last_month_name)) {
    QStringList name_filter;
    name_filter << last_month.toString("yyyy.MM.*.log");
    QFileInfoList log_files = logs_dir.entryInfoList(name_filter, QDir::Files);
    if (log_files.size() > 0) {
      if (!ArchiveFiles(log_files, archive_dir.absoluteFilePath(
                                       last_month.toString("yyyy.MM.zip")))) {
        qWarning()
            << "Can not archive last month's logging files by zip compressor.";
      }
      // remove last month's logs
      for (const QFileInfo& file : log_files) {
        logs_dir.remove(file.fileName());
      }
    }
  }
}

void MessageHandler(QtMsgType type, const QMessageLogContext& context,
                    const QString& message) {
  auto logger = spdlog::get("logger");
  switch (type) {
    case QtDebugMsg:
      logger->debug(message.toStdString());
      break;
    case QtWarningMsg:
      logger->warn(message.toStdString());
      break;
    case QtCriticalMsg:
      logger->critical(message.toStdString());
      break;
    case QtFatalMsg:
      logger->error(message.toStdString());
      break;
    case QtInfoMsg:
      logger->info(message.toStdString());
      break;
    default:
      break;
  }
}

void Initialize() {
  QDir dir = QCoreApplication::applicationDirPath();
  dir.cd("logs");
  QString log_file = dir.absoluteFilePath(
      QDate::currentDate().toString("yyyy.MM.dd") + ".log");

  auto daily_sink = std::make_shared<spdlog::sinks::daily_file_format_sink_mt>(
      log_file.toStdString(), 0, 0, false);

  QSettings settings;
  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(daily_sink);

  bool use_msvc_sink = settings.value("logs/use_msvc", true).toBool();
  if (use_msvc_sink) {
    auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
    msvc_sink->set_pattern("%v");
    sinks.push_back(msvc_sink);
  }

  spdlog::init_thread_pool(8192, 1);
  auto logger = std::make_shared<spdlog::async_logger>(
      "logger", sinks.begin(), sinks.end(), spdlog::thread_pool());

  QString level = settings.value("logs/level", QString("debug")).toString();
  if (level == "info") {
    logger->set_level(spdlog::level::info);
  } else if (level == "debug") {
    logger->set_level(spdlog::level::debug);
  } else if (level == "warning") {
    logger->set_level(spdlog::level::warn);
  } else if (level == "critical") {
    logger->set_level(spdlog::level::critical);
  } else if (level == "fatal") {
    logger->set_level(spdlog::level::err);
  }

  QString pattern =
      settings.value("logs/pattern", "[%Y-%m-%d %H:%M:%S.%e][%8l]: %v")
          .toString();
  daily_sink->set_pattern(pattern.toStdString());
  spdlog::register_logger(logger);

  qInstallMessageHandler(MessageHandler);
}

bool qtapp::common::StartLogging() {
  if (!CheckDir()) {
    return false;
  }

  Initialize();
  ArchiveLastMonthLogFiles();

  return true;
}

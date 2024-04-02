#include "qtapp/common/logging.h"

#include <QtCore>
#include <vector>

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

bool ArchiveFiles(const QStringList& files, const QString& archive_file_name) {
  QStringList param;
  param << "-bsp0" << "-bso0" << "a" << archive_file_name << files;

  if (QProcess::execute("7z", param) < 0) {
    // compress failed
    QDir archive_dir = QFileInfo(archive_file_name).absolutePath();
    for (const QString& file_name : files) {
      QFileInfo info(file_name);
      QFile f(file_name);
      f.copy(archive_dir.absoluteFilePath(info.fileName()));
    }
    return false;
  }

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
    QStringList log_files = logs_dir.entryList(name_filter, QDir::Files);
    for (QString& f : log_files) {
      f = logs_dir.absoluteFilePath(f);
    }

    if (log_files.size() > 0) {
      if (!ArchiveFiles(log_files, archive_dir.absoluteFilePath(
                                       last_month.toString("yyyy.MM.7z")))) {
        qWarning()
            << "Can not archive last month's logging files by 7z compressor.";
      }
      // remove last month's logs
      for (const QString& file : log_files) {
        logs_dir.remove(QFileInfo(file).fileName());
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

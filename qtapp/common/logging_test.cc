#include "qtapp/common/logging.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>

#include "gtest/gtest.h"

TEST(Logging, Init) {
  EXPECT_TRUE(qtapp::common::StartLogging());
  QDir dir = QCoreApplication::applicationDirPath();

  EXPECT_TRUE(dir.cd("logs"));
  EXPECT_TRUE(dir.cd("archived"));

  QDate this_month = QDate::currentDate();
  QDate last_month = this_month.addMonths(-1);
  QString last_month_name = last_month.toString("yyyy.MM.7z");

  // achived file created by 7z
  EXPECT_TRUE(dir.exists(last_month_name));
  dir.cdUp();

  QStringList name_filter;
  name_filter << last_month.toString("yyyy.MM.*.log");
  QStringList log_files = dir.entryList(name_filter, QDir::Files);
  EXPECT_EQ(log_files.size(), 0);
}

TEST(Logging, Message) {
  qInfo() << "info" << "1" << 1.1 << 2;
  qWarning() << "warning";
  qDebug() << "debug";
  qCritical() << "critical";
}

void GenerateLogs() {
  // Remove logs folder before test.
  QDir dir;
  dir.setPath(QCoreApplication::applicationDirPath());
  if (dir.cd("logs")) {
    dir.removeRecursively();
  }

  // create last month
  dir.setPath(QCoreApplication::applicationDirPath());
  dir.mkdir("logs");
  dir.cd("logs");
  dir.mkdir("archived");
  QString date_str = QDate::currentDate().addMonths(-1).toString("yyyy.MM");

  QFile file1(dir.absoluteFilePath(date_str + ".01.log"));
  file1.open(QIODevice::WriteOnly | QIODevice::Text);
  file1.write("abc");
  file1.close();

  QFile file2(dir.absoluteFilePath(date_str + ".02.log"));
  file2.open(QIODevice::WriteOnly | QIODevice::Text);
  file2.write("def");
  file2.close();
}

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  GenerateLogs();

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#include "qtapp/common/settings.h"

#include <gtest/gtest.h>

#include <QColor>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QVariant>

TEST(settings, read) {
  QDir dir = QCoreApplication::applicationDirPath();
  QSettings settings(dir.absoluteFilePath("TestRead.json"),
                     qtapp::common::GetSettingsFormat());
  EXPECT_EQ(settings.value("a"), 12);
  EXPECT_EQ(settings.value("b"), 1.23);
  EXPECT_EQ(settings.value("c"), "d:/projects");
  QList<QVariant> l = settings.value("d").toList();
  QList<QVariant> r{1, 2, 3};
  EXPECT_EQ(l, r);

  EXPECT_EQ(settings.value("e/f"), 23);
  EXPECT_EQ(settings.value("e/g"), 23.4);
  EXPECT_EQ(settings.value("e/h"), "f:/k.bmp");
  l = settings.value("e/i").toList();
  r = QList<QVariant>{1.1, 3.3, 7.7};
  EXPECT_EQ(l, r);

  EXPECT_EQ(settings.value("e/j/k"), 734);
  l = settings.value("e/j/l").toList();
  r = QList<QVariant>{123, 3.3, "abc"};
  EXPECT_EQ(l, r);
  EXPECT_EQ(settings.value("e/j/m"), "ccc");
}

TEST(settings, modify) {
  QList<QVariant> colors;
  colors << QVariant::fromValue<QColor>(Qt::red);
  colors << QVariant::fromValue<QColor>(Qt::green);
  colors << QVariant::fromValue<QColor>(Qt::blue);

  QDir dir = QCoreApplication::applicationDirPath();
  {
    QSettings settings(dir.absoluteFilePath("TestModify.json"),
                       qtapp::common::GetSettingsFormat());
    settings.setValue("a", 3.3);
    settings.setValue("k", "abc");
    settings.setValue("d", QList<QVariant>{1, 1.2, "abc"});
    settings.setValue("e/f", "def");
    settings.setValue("e/j", 3.3);
    settings.setValue("k/l", 3.3);
    settings.setValue("color", QVariant::fromValue<QColor>(Qt::red));
    settings.setValue("colors", colors);
    settings.sync();
  }
  {
    QSettings settings(dir.absoluteFilePath("TestModify.json"),
                       qtapp::common::GetSettingsFormat());
    EXPECT_EQ(settings.value("a"), 3.3);
    EXPECT_EQ(settings.value("k"), "abc");
    EXPECT_EQ(settings.value("e/f"), "def");
    EXPECT_EQ(settings.value("e/j"), 3.3);
    EXPECT_EQ(settings.value("k/l"), 3.3);
    QList<QVariant> l = settings.value("d").toList();
    QList<QVariant> r{1, 1.2, "abc"};
    EXPECT_EQ(l, r);

    EXPECT_EQ(settings.value("color").value<QColor>(), Qt::red);
    l = settings.value("colors").toList();
    EXPECT_EQ(l, colors);
  }
}

static void CreateSettingsFile(const QString& fname);
static void RemoveSettingsFiles();

int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);

  RemoveSettingsFiles();
  qtapp::common::InitSettings();
  CreateSettingsFile("TestRead.json");
  CreateSettingsFile("TestModify.json");

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
/*
  {
    "a" : 12,
    "b" : 1.23,
    "c" : "d:/projects",
    "d" : [1, 2, 3],
    "e" :
    {
      "f" : 23,
      "g" : 23.4,
      "h" : "f:/k.bmp"
      "i" : [1.1, 3.3, 7.7]
      "j" : {
        "k" : 734,
        "l" : [
          123,
          3.3,
          "abc"
        ],
        "m" : "ccc"
      }
    }
  }
*/

void CreateSettingsFile(const QString& fname) {
  QDir dir = QCoreApplication::applicationDirPath();
  QSettings settings(dir.absoluteFilePath(fname),
                     qtapp::common::GetSettingsFormat());
  settings.setValue("a", 12);
  settings.setValue("b", 1.23);
  settings.setValue("c", "d:/projects");
  QList<QVariant> arr{1, 2, 3};
  settings.setValue("d", arr);
  settings.setValue("e/f", 23);
  settings.setValue("e/g", 23.4);
  settings.setValue("e/h", "f:/k.bmp");
  arr = {1.1, 3.3, 7.7};
  settings.setValue("e/i", arr);
  settings.setValue("e/j/k", 734);
  arr = {123, 3.3, "abc"};
  settings.setValue("e/j/l", arr);
  settings.setValue("e/j/m", "ccc");
  settings.sync();
}
void RemoveSettingsFiles() {
  QDir dir = QCoreApplication::applicationDirPath();
  dir.remove("TestRead.json");
  dir.remove("TestModify.json");
}

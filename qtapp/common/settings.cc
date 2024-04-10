#include "qtapp/common/settings.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>

static bool ReadJsonSettingsFile(QIODevice& device,
                                 QSettings::SettingsMap& map);
static bool WriteJsonSettingsFile(QIODevice& device,
                                  const QSettings::SettingsMap& map);

QSettings::Format kSettingFormat = QSettings::InvalidFormat;

bool qtapp::common::InitSettings() {
  kSettingFormat = QSettings::registerFormat(
      "json", ReadJsonSettingsFile, WriteJsonSettingsFile, Qt::CaseSensitive);
  if (kSettingFormat != QSettings::InvalidFormat) {
    QSettings::setDefaultFormat(kSettingFormat);
    QString path = QCoreApplication::applicationDirPath();
    QSettings::setPath(kSettingFormat, QSettings::UserScope, path);
  }
  return (kSettingFormat != QSettings::InvalidFormat);
}

QSettings::Format qtapp::common::GetSettingsFormat() { return kSettingFormat; }

void FlattenVariantMap(const QVariantMap& input, const QString& prefix,
                       QSettings::SettingsMap* settings) {
  for (auto iter = input.cbegin(); iter != input.cend(); ++iter) {
    if (iter->type() == QMetaType::QVariantMap) {
      FlattenVariantMap(iter->toMap(), prefix + iter.key() + "/", settings);
    } else {
      (*settings)[prefix + iter.key()] = (*iter);
    }
  }
}

bool ReadJsonSettingsFile(QIODevice& device, QSettings::SettingsMap& settings) {
  QByteArray data = device.readAll();
  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (doc.object().isEmpty()) {  // is array
    return false;
  }

  QVariantMap values = doc.object().toVariantMap();
  FlattenVariantMap(values, "", &settings);
  return true;
}

void SetValueToJson(QJsonObject& obj, const QStringList& names, int index,
                    const QVariant& value) {
  if (index == names.size() - 1) {
    obj[names.last()] = QJsonValue::fromVariant(value);
    return;
  }

  if (!obj.contains(names[index])) {
    obj.insert(names[index], QJsonObject());
  }

  QJsonObject new_obj = obj[names[index]].toObject();
  SetValueToJson(new_obj, names, index + 1, value);
  obj[names[index]] = new_obj;
}

bool WriteValueToJson(QJsonObject& doc, const QString& key,
                      const QVariant& value) {
  QStringList names = key.split("/", Qt::SkipEmptyParts);

  if (names.size() == 0) {
    return true;
  }

  SetValueToJson(doc, names, 0, value);

  return true;
}

bool WriteJsonSettingsFile(QIODevice& device,
                           const QSettings::SettingsMap& settings) {
  QJsonDocument doc;
  QJsonObject root = doc.object();

  for (QString& key : settings.keys()) {
    if (!WriteValueToJson(root, key, settings[key])) {
      return false;
    }
  }

  doc.setObject(root);
  device.write(doc.toJson());

  return true;
}

#include "jwt.h"
#include <botan_wrapper.h>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QDebug>

jwt::jwt()
{

}

void jwt::set_iss(const QString & value)
{
  iss = value;
}

void jwt::set_scope(const QString & value)
{
  scope = value;
}

void jwt::set_aud(const QString & value)
{
  aud = value;
}

void jwt::set_private_key_pem(const QString & pem)
{
  priv_key_pem = pem;
  priv_key_pem.remove('\n');
}

QByteArray jwt::compose() const
{
  const qint64 cur_secs = QDateTime::currentDateTimeUtc().currentSecsSinceEpoch();
  const QByteArray header_b64 = QByteArray("{\"alg\":\"RS256\",\"typ\":\"JWT\"}").toBase64(QByteArray::Base64UrlEncoding);
  QJsonObject jo;
  jo.insert("iss", iss);
  jo.insert("scope", scope);
  jo.insert("aud", aud);
  jo.insert("exp", cur_secs + 3500);
  jo.insert("iat", cur_secs);
  const auto json = QJsonDocument(jo).toJson(QJsonDocument::Compact);
  //QByteArray json = "{\"aud\":\"https://oauth2.googleapis.com/token\",\"exp\":1238167,\"iat\":1234567,\"iss\":\"761326798069-r5mljlln1rd4lrbhg75efgigp36m78j5@developer.gserviceaccount.com\",\"scope\":\"https://www.googleapis.com/auth/devstorage.readonly\"}";
  //qDebug().noquote() << json;
  QByteArray ret = header_b64 + "." + json.toBase64(QByteArray::Base64UrlEncoding);

  botan_wrapper bw;
  QByteArray sign_b64;
  bw.rsa_sign(ret, priv_key_pem, &sign_b64);
  ret += "." + sign_b64;

  return ret;
}

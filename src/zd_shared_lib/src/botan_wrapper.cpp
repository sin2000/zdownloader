#include "botan_wrapper.h"
#include "botan_wrapper_private.h"
#include <botan/pem.h>
#include <botan/pubkey.h>
#include <botan/pkcs8.h>
#include <botan/data_src.h>
#include <QByteArray>
#include <QDebug>

botan_wrapper::botan_wrapper()
  :priv(new botan_wrapper_private)
{
}

botan_wrapper::~botan_wrapper()
{
  delete priv;
}

void botan_wrapper::rsa_sign(const QByteArray & input_data, const QString & privkey_pem, QByteArray * sign_b64_url) const
{
  std::string label = "";
  std::vector<uint8_t> data(input_data.begin(), input_data.end());

  try
  {
    auto decoded = Botan::PEM_Code::decode(privkey_pem.toStdString(), label);
    Botan::DataSource_Memory memkey(decoded);

    std::unique_ptr<Botan::Private_Key> pk(Botan::PKCS8::load_key(memkey, *priv->get_rng()));
    Botan::PK_Signer signer(*pk, *priv->get_rng(), "EMSA3(SHA-256)");
    signer.update(data);
    const std::vector<uint8_t> sign_vec = signer.signature(*priv->get_rng());

    *sign_b64_url = QByteArray(reinterpret_cast<const char *>(sign_vec.data()), static_cast<int>(sign_vec.size()));
    *sign_b64_url = sign_b64_url->toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
  }
  catch(const Botan::Exception & ex)
  {
    qDebug() << "exception: " << ex.what();
    *sign_b64_url = QByteArray();
  }
}


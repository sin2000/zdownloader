#ifndef BOTAN_WRAPPER_TESTS_H
#define BOTAN_WRAPPER_TESTS_H

#include "../botan_wrapper.h"
#include <QTest>
#include <QObject>

class botan_wrapper_tests : public QObject
{
  Q_OBJECT
private slots:
  void rsa_sign_returns_correct()
  {
    const QByteArray input = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJhdWQiOiJodHRwczovL29hdXRoMi5nb29nbGVhcGlzLmNvbS90b2tlbiIsImV4cCI6MTIzODE2NywiaWF0IjoxMjM0NTY3LCJpc3MiOiI3NjEzMjY3OTgwNjktcjVtbGpsbG4xcmQ0bHJiaGc3NWVmZ2lncDM2bTc4ajVAZGV2ZWxvcGVyLmdzZXJ2aWNlYWNjb3VudC5jb20iLCJzY29wZSI6Imh0dHBzOi8vd3d3Lmdvb2dsZWFwaXMuY29tL2F1dGgvZGV2c3RvcmFnZS5yZWFkb25seSJ9";

    QString pk = "-----BEGIN PRIVATE KEY-----\nMIIE CUTTED LOL";

    QByteArray signature;
    botan_wrapper bw;
    bw.rsa_sign(input, pk, &signature);

    qDebug() << signature;
  }

//    void cryppp()
//    {
//      try
//      {
//      std::string plaintext("Your great-grandfather gave this");
//      std::vector<uint8_t> pt(plaintext.data(),plaintext.data()+plaintext.length());
//      std::unique_ptr<Botan::RandomNumberGenerator> rng(new Botan::System_RNG);

//      Botan::RSA_PrivateKey key(*rng.get(), 1024);

//      std::string pub = Botan::X509::PEM_encode(key);
//      std::string priv = Botan::PKCS8::PEM_encode(key);

//      Botan::DataSource_Memory key_pub(pub);
//      Botan::DataSource_Memory key_priv(priv);

//      //load keypair
//      std::unique_ptr<Botan::Private_Key> kp(Botan::PKCS8::load_key(key_priv,*rng.get()));
//      std::unique_ptr<Botan::Public_Key> pubk(Botan::X509::load_key(key_pub));

//      //encrypt with pk
//      Botan::PK_Encryptor_EME enc(*pubk,*rng.get(), "EME1(SHA-256)");
//      std::vector<uint8_t> ct = enc.encrypt(pt,*rng.get());

//      //decrypt with sk
//      Botan::PK_Decryptor_EME dec(*kp,*rng.get(), "EME1(SHA-256)");

//      std::string encs = Botan::hex_encode(ct);
//      auto decv = dec.decrypt(ct);
//      auto dddd = QString::fromUtf8(reinterpret_cast<const char *>(decv.data()), static_cast<int>(decv.size()));
//      }
//      catch(Botan::Exception ex)
//      {
//        qDebug() << ex.what();
//      }
//      qDebug() << "booo";
//    }
};

#endif // BOTAN_WRAPPER_TESTS_H

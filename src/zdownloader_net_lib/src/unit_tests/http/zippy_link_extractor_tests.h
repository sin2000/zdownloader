#ifndef ZIPPY_LINK_EXTRACTOR_TESTS_H
#define ZIPPY_LINK_EXTRACTOR_TESTS_H

#include "../../http/zippyshare/zippy_link_extractor.h"
#include <QTest>

class zippy_link_extractor_tests : public QObject
{
  Q_OBJECT
private slots:
  void parse_html_returns_true()
  {
    const QString src_link = "https://www66.zippyshare.com/v/DDDDDDDD/file.html";

    const QString input =
        R"(
        <a id="dlbutton"  href="#">
        <div class="download"></div>
        </a>

        <script type="text/javascript">
            document.getElementById('dlbutton').href = "/d/DDDDDDDD/" + (413648 % 51245 + 413648 % 913) + "/DD%2b.2.0.x264.rar";
            if (document.getElementById('fimage')) {
                document.getElementById('fimage').href = "/i/DDDDDDDD/" + (413648 % 51245 + 413648 % 913) + "/DD%2b.2.0.x264.rar";
            }
        </script>
        )";

    zippy_link_extractor extractor;
    bool result = extractor.parse_html(input, src_link);
    QString exlink = extractor.get_extracted_link();
    QString exfilename = extractor.get_extracted_filename();

    QCOMPARE(result, true);
    QCOMPARE(exlink, QString("https://www66.zippyshare.com/d/DDDDDDDD/3747/DD%2B.2.0.x264.rar"));
    QCOMPARE(exfilename, QString("DD+.2.0.x264.rar"));
  }

  void parse_html_returns_true_test2()
  {
    const QString src_link = "https://www120.zippyshare.com/v/DDDDDDDD/file.html";

    const QString input =
        R"(
        <a id="dlbutton"  href="#">
        <script type="text/javascript">
            var a = 540622;
            var b = 742589;
            document.getElementById('dlbutton').omg = "f";
            if (document.getElementById('dlbutton').omg != 'f') {
               a = Math.ceil(a/3);
            } else {
               a = Math.floor(a/3);
            }
            document.getElementById('dlbutton').href = "/d/DDDDDDDD/"+(a + 540622%b)+"/dd.003";
            if (document.getElementById('fimage')) {
                document.getElementById('fimage').href = "/i/DDDDDDDD/"+(a + 540622%b)+"/dd.003";
            }
        </script>
        )";

    zippy_link_extractor extractor;
    bool result = extractor.parse_html(input, src_link);
    QString exlink = extractor.get_extracted_link();
    QString exfilename = extractor.get_extracted_filename();

    QCOMPARE(result, true);
    QCOMPARE(exlink, QString("https://www120.zippyshare.com/d/DDDDDDDD/720829/dd.003"));
    QCOMPARE(exfilename, QString("dd.003"));
  }

  void parse_html_returns_true_test3()
  {
    const QString src_link = "https://www17.zippyshare.com/v/AAAAAAAA/file.html";

    const QString input =
        R"(
        <a id="dlbutton"  href="#">
        <script type="text/javascript">
            var a = 630974%900;
            var b = 630974%53;
            var c = 8;
            if (false) {
                c = 9;
                var d = 9;
            }
            var d = 630974%13;
            document.getElementById('dlbutton').href = "/d/AAAAAAAA/"+(a * b + c + d)+"/%5bBAGAS31%5d%20IDM.rar";
            if (document.getElementById('fimage')) {
                document.getElementById('fimage').href = "/i/AAAAAAAA/"+(a * b + c + d)+"/%5bBAGAS31%5d%20IDM.rar";
            }
        </script>
        )";

    zippy_link_extractor extractor;
    bool result = extractor.parse_html(input, src_link);
    QString exlink = extractor.get_extracted_link();
    QString exfilename = extractor.get_extracted_filename();

    QCOMPARE(result, true);
    QCOMPARE(exlink, QString("https://www17.zippyshare.com/d/AAAAAAAA/680/%5BBAGAS31%5D IDM.rar"));
    QCOMPARE(exfilename, QString("[BAGAS31] IDM.rar"));
  }

  void parse_html_returns_true_test4()
  {
    const QString src_link = "https://www66.zippyshare.com/v/AAAAAAAA/file.html";

    const QString input =
        R"(
        <a id="dlbutton"  href="#">
        <script type="text/javascript">
            var n = 2;
            var b = 155870;
            document.getElementById('dlbutton').href = "/d/AAAAAAAA/"+(n + n * 2 + b)+"3/DD%2b.2.0.x264.rar";
            if (document.getElementById('fimage')) {
                document.getElementById('fimage').href = "/i/AAAAAAAA/"+(n + n * 2 + b)+"3/DD%2b.2.0.x264.rar";
            }
        </script>
        )";

    zippy_link_extractor extractor;
    bool result = extractor.parse_html(input, src_link);
    QString exlink = extractor.get_extracted_link();
    QString exfilename = extractor.get_extracted_filename();

    QCOMPARE(result, true);
    QCOMPARE(exlink, QString("https://www66.zippyshare.com/d/AAAAAAAA/1558763/DD%2B.2.0.x264.rar"));
    QCOMPARE(exfilename, QString("DD+.2.0.x264.rar"));
  }

  void parse_html_returns_true_test5()
  {
    const QString src_link = "https://www17.zippyshare.com/v/AAAAAAAA/file.html";

    const QString input =
        R"(
        <a id="dlbutton"  href="#">
        <script type="text/javascript">
            var a = 86;
            document.getElementById('dlbutton').omg = "asdasd".substr(0, 3);
            var b = document.getElementById('dlbutton').omg.length;
            document.getElementById('dlbutton').href = "/d/AAAAAAAA/"+(Math.pow(a, 3)+b)+"/20022003.zip";
            if (document.getElementById('fimage')) {
                document.getElementById('fimage').href = "/i/AAAAAAAA/"+(Math.pow(a, 3)+b)+"/20022003.zip";
            }
        </script>
        )";

    zippy_link_extractor extractor;
    bool result = extractor.parse_html(input, src_link);
    QString exlink = extractor.get_extracted_link();
    QString exfilename = extractor.get_extracted_filename();

    QCOMPARE(result, true);
    QCOMPARE(exlink, QString("https://www17.zippyshare.com/d/AAAAAAAA/636059/20022003.zip"));
    QCOMPARE(exfilename, QString("20022003.zip"));
  }

  void parse_html_returns_true_test6()
  {
    const QString src_link = "https://www17.zippyshare.com/v/AAAAAAAA/file.html";

    const QString input =
        R"(
        <script type="text/javascript">
          var fake_script = "grumpy admin";
        </script>

        <a id="dlbutton"  href="#">
        <span id="omg" class="2" style="display:none;"></span>
        <script type="text/javascript">
            var a = function() {return 1};
            var b = function() {return a() + 1};
            var c = function() {return b() + 1};
            var d = document.getElementById('omg').getAttribute('class');
            if (true) { d = d*2;}
            document.getElementById('dlbutton').href = "/d/AAAAAAAA/"+(462509%1000 + a() + b() + c() + d + 5/5)+"/20022003.zip";
            if (document.getElementById('fimage')) {
                document.getElementById('fimage').href = "/i/AAAAAAAA/"+(462509%1000 + a() + b() + c() + d + 5/5)+"/20022003.zip";
            }
        </script>
        )";

    zippy_link_extractor extractor;
    bool result = extractor.parse_html(input, src_link);
    QString exlink = extractor.get_extracted_link();
    QString exfilename = extractor.get_extracted_filename();

    QCOMPARE(result, true);
    QCOMPARE(exlink, QString("https://www17.zippyshare.com/d/AAAAAAAA/520/20022003.zip"));
    QCOMPARE(exfilename, QString("20022003.zip"));
  }
};
#endif // ZIPPY_LINK_EXTRACTOR_TESTS_H

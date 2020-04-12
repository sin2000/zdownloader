#ifndef SEGMENT_CALCULATOR_TESTS_H
#define SEGMENT_CALCULATOR_TESTS_H

#include "../../http/segment_calculator.h"
#include <QTest>

class segment_calculator_tests : public QObject
{
  Q_OBJECT
private slots:
  void get_segment_metadata_list_returns_correct_list()
  {
    segment_calculator seg_calc;
    seg_calc.compute(4, 5, 2);

    auto & list = seg_calc.get_segment_metadata_list();
    QCOMPARE(list.size(), 1);
    QCOMPARE(list[0].start_pos, 0);
    QCOMPARE(list[0].end_pos, 3);
  }

  void get_segment_metadata_list_returns_correct_list2()
  {
    segment_calculator seg_calc;
    seg_calc.compute(9, 5, 2);

    auto & list = seg_calc.get_segment_metadata_list();
    QCOMPARE(list.size(), 1);
    QCOMPARE(list[0].start_pos, 0);
    QCOMPARE(list[0].end_pos, 8);
  }

  void get_segment_metadata_list_returns_correct_list3()
  {
    segment_calculator seg_calc;
    seg_calc.compute(16, 5, 2);

    auto & list = seg_calc.get_segment_metadata_list();
    QCOMPARE(list.size(), 2);
    QCOMPARE(list[0].start_pos, 0);
    QCOMPARE(list[0].end_pos, 7);
    QCOMPARE(list[1].start_pos, 8);
    QCOMPARE(list[1].end_pos, 15);
  }

  void get_segment_metadata_list_returns_correct_list4()
  {
    segment_calculator seg_calc;
    seg_calc.compute(16, 5, 3);

    auto & list = seg_calc.get_segment_metadata_list();
    QCOMPARE(list.size(), 3);
    QCOMPARE(list[0].start_pos, 0);
    QCOMPARE(list[0].end_pos, 4);
    QCOMPARE(list[1].start_pos, 5);
    QCOMPARE(list[1].end_pos, 9);
    QCOMPARE(list[2].start_pos, 10);
    QCOMPARE(list[2].end_pos, 15);
  }

  void get_segment_metadata_list_returns_correct_list5()
  {
    segment_calculator seg_calc;
    seg_calc.compute(16, 5, 4);

    auto & list = seg_calc.get_segment_metadata_list();
    QCOMPARE(list.size(), 3);
    QCOMPARE(list[0].start_pos, 0);
    QCOMPARE(list[0].end_pos, 4);
    QCOMPARE(list[1].start_pos, 5);
    QCOMPARE(list[1].end_pos, 9);
    QCOMPARE(list[2].start_pos, 10);
    QCOMPARE(list[2].end_pos, 15);
  }

  void get_segment_metadata_list_returns_correct_list6()
  {
    segment_calculator seg_calc;
    seg_calc.compute(14, 5, 3);

    auto & list = seg_calc.get_segment_metadata_list();
    QCOMPARE(list.size(), 2);
    QCOMPARE(list[0].start_pos, 0);
    QCOMPARE(list[0].end_pos, 4);
    QCOMPARE(list[1].start_pos, 5);
    QCOMPARE(list[1].end_pos, 13);
  }

  void get_segment_metadata_list_returns_correct_list7()
  {
    segment_calculator seg_calc;
    seg_calc.compute(14, 3, 4);

    auto & list = seg_calc.get_segment_metadata_list();
    QCOMPARE(list.size(), 4);
    QCOMPARE(list[0].start_pos, 0);
    QCOMPARE(list[0].end_pos, 2);
    QCOMPARE(list[1].start_pos, 3);
    QCOMPARE(list[1].end_pos, 5);
    QCOMPARE(list[2].start_pos, 6);
    QCOMPARE(list[2].end_pos, 8);
    QCOMPARE(list[3].start_pos, 9);
    QCOMPARE(list[3].end_pos, 13);
  }

  void get_segment_metadata_list_returns_correct_list8()
  {
    segment_calculator seg_calc;
    seg_calc.compute(14, 3, 5);

    auto & list = seg_calc.get_segment_metadata_list();
    QCOMPARE(list.size(), 4);
    QCOMPARE(list[0].start_pos, 0);
    QCOMPARE(list[0].end_pos, 2);
    QCOMPARE(list[1].start_pos, 3);
    QCOMPARE(list[1].end_pos, 5);
    QCOMPARE(list[2].start_pos, 6);
    QCOMPARE(list[2].end_pos, 8);
    QCOMPARE(list[3].start_pos, 9);
    QCOMPARE(list[3].end_pos, 13);
  }

  void get_segment_metadata_list_returns_correct_list9()
  {
    segment_calculator seg_calc;
    seg_calc.compute(50, 2, 15);

    auto & list = seg_calc.get_segment_metadata_list();
    QCOMPARE(list.size(), 15);
    QCOMPARE(list[0].start_pos, 0);
    QCOMPARE(list[0].end_pos, 2);
    QCOMPARE(list[14].start_pos, 42);
    QCOMPARE(list[14].end_pos, 49);
  }

  void get_segment_metadata_list_returns_correct_list10()
  {
    segment_calculator seg_calc;
    seg_calc.compute(50, 2, 1);

    auto & list = seg_calc.get_segment_metadata_list();
    QCOMPARE(list.size(), 1);
    QCOMPARE(list[0].start_pos, 0);
    QCOMPARE(list[0].end_pos, 49);
  }

  void get_segment_metadata_list_returns_correct_list11()
  {
    segment_calculator seg_calc;
    seg_calc.compute(50, 2, 50);

    auto & list = seg_calc.get_segment_metadata_list();
    QCOMPARE(list.size(), 25);
    QCOMPARE(list[0].start_pos, 0);
    QCOMPARE(list[0].end_pos, 1);
    QCOMPARE(list[24].start_pos, 48);
    QCOMPARE(list[24].end_pos, 49);
  }

  void get_segment_metadata_list_returns_correct_list12()
  {
    segment_calculator seg_calc;
    seg_calc.compute(50, 1, 50);

    auto & list = seg_calc.get_segment_metadata_list();
    QCOMPARE(list.size(), 50);
    QCOMPARE(list[0].start_pos, 0);
    QCOMPARE(list[0].end_pos, 0);
    QCOMPARE(list[49].start_pos, 49);
    QCOMPARE(list[49].end_pos, 49);
  }

  void get_segment_metadata_list_returns_correct_list13()
  {
    segment_calculator seg_calc;
    seg_calc.compute(50, 50, 50);

    auto & list = seg_calc.get_segment_metadata_list();
    QCOMPARE(list.size(), 1);
    QCOMPARE(list[0].start_pos, 0);
    QCOMPARE(list[0].end_pos, 49);
  }

  void get_segment_metadata_list_returns_correct_list14()
  {
    segment_calculator seg_calc;
    seg_calc.compute(10, 50, 50);

    auto & list = seg_calc.get_segment_metadata_list();
    QCOMPARE(list.size(), 1);
    QCOMPARE(list[0].start_pos, 0);
    QCOMPARE(list[0].end_pos, 9);
  }

  void get_segment_metadata_list_returns_correct_list15()
  {
    segment_calculator seg_calc;
    seg_calc.compute(10, 2, 3);

    auto & list = seg_calc.get_segment_metadata_list();
    QCOMPARE(list.size(), 3);
    QCOMPARE(list[0].start_pos, 0);
    QCOMPARE(list[0].end_pos, 2);
    QCOMPARE(list[1].start_pos, 3);
    QCOMPARE(list[1].end_pos, 5);
    QCOMPARE(list[2].start_pos, 6);
    QCOMPARE(list[2].end_pos, 9);
  }

  void get_segment_metadata_list_returns_correct_list16()
  {
    segment_calculator seg_calc;

    for(int i = 3; i <= 16; ++i)
    {
      seg_calc.compute(50, 2, i);
      auto & list = seg_calc.get_segment_metadata_list();
      auto diff_first = list[0].end_pos - list[0].start_pos;
      int last_idx = list.size() - 1;
      auto diff_last = list[last_idx].end_pos - list[last_idx].start_pos;

      qDebug() << "max seg:" << i << "act. seg:" << list.size() << "first diff:" << diff_first << "last diff:" << diff_last;
    }
  }
};

#endif // SEGMENT_CALCULATOR_TESTS_H

/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  v11sctests.cpp
 *  @brief: Tests for the v11::CRingScalerItem class.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingScalerItem.h"
#include "DataFormat.h"



class v11sctest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11sctest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    
    CPPUNIT_TEST(starttime_1);   // non body header.
    CPPUNIT_TEST(starttime_2);
    CPPUNIT_TEST(starttime_3);   // Body header
    CPPUNIT_TEST(starttime_4);
    CPPUNIT_TEST(starttime_5);   // Compute no body header.
    CPPUNIT_TEST(starttime_6);   // Compute with body header.
    
    CPPUNIT_TEST(endtime_1);   // non body header.
    CPPUNIT_TEST(endtime_2);
    CPPUNIT_TEST(endtime_3);   // Body header
    CPPUNIT_TEST(endtime_4);
    CPPUNIT_TEST(endtime_5);   // Compute no body header.
    CPPUNIT_TEST(endtime_6);   // Compute with body header.
    
    CPPUNIT_TEST(div_1);
    CPPUNIT_TEST(div_2);

    CPPUNIT_TEST(stamp_1);
    CPPUNIT_TEST(stamp_2);
    CPPUNIT_TEST(stamp_3);
    CPPUNIT_TEST(stamp_4);
    
    CPPUNIT_TEST(incremental_1);
    CPPUNIT_TEST(incremental_2);
    CPPUNIT_TEST(incremental_3);
    CPPUNIT_TEST(incremental_4);
    
    CPPUNIT_TEST(setscaler_1);
    CPPUNIT_TEST(setscaler_2);
    CPPUNIT_TEST(setscaler_3);
    CPPUNIT_TEST(setscaler_4);
    
        
    CPPUNIT_TEST(getscaler_1);
    CPPUNIT_TEST(getscaler_2);
    CPPUNIT_TEST(getscaler_3);
    CPPUNIT_TEST(getscaler_4);

    CPPUNIT_TEST_SUITE_END();
    
    
    
    
private:

public:
    void setUp() {
        
    }
    void tearDown() {
        
    }
protected:
    void construct_1();
    void construct_2();
    void construct_3();
    
    void starttime_1();
    void starttime_2();
    void starttime_3();
    void starttime_4();
    void starttime_5();
    void starttime_6();
    
    void endtime_1();
    void endtime_2();
    void endtime_3();
    void endtime_4();
    void endtime_5();
    void endtime_6();
    
    void div_1();
    void div_2();
    
    void stamp_1();
    void stamp_2();
    void stamp_3();
    void stamp_4();
    
    void incremental_1();
    void incremental_2();
    void incremental_3();
    void incremental_4();
    
    void setscaler_1();
    void setscaler_2();
    void setscaler_3();
    void setscaler_4();
    
    void getscaler_1();
    void getscaler_2();
    void getscaler_3();
    void getscaler_4();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v11sctest);

// only have the number of scalers present - no body header.
void v11sctest::construct_1()
{
    time_t now = time(nullptr);
    v11::CRingScalerItem item(32);
    const v11::ScalerItem* pItem =
        reinterpret_cast<const v11::ScalerItem*>(item.getItemPointer());
    EQ(sizeof(v11::RingItemHeader) + 33*sizeof(uint32_t) + sizeof(v11::ScalerItemBody),
       size_t(pItem->s_header.s_size));
    EQ(v11::PERIODIC_SCALERS, pItem->s_header.s_type);
    EQ(uint32_t(0), pItem->s_body.u_noBodyHeader.s_mbz);
    
    const v11::ScalerItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(0), pBody->s_intervalStartOffset);
    EQ(uint32_t(0), pBody->s_intervalEndOffset);
    ASSERT(pBody->s_timestamp - now <= 1);
    EQ(uint32_t(1), pBody->s_intervalDivisor);
    EQ(uint32_t(32), pBody->s_scalerCount);
    EQ(uint32_t(1), pBody->s_isIncremental);
    for (int i = 0; i < pBody->s_scalerCount; i++) {
        EQ(uint32_t(0), pBody->s_scalers[i]);
    }
    
}
void v11sctest::construct_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i =0;i < 32; i++) {
        scalers.push_back(i*200);
    }
    v11::CRingScalerItem item(10, 20, now, scalers);
    const v11::ScalerItem* pItem =
        reinterpret_cast<const v11::ScalerItem*>(item.getItemPointer());
    EQ(v11::PERIODIC_SCALERS, pItem->s_header.s_type);
    EQ(
        sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader)
        + sizeof(v11::ScalerItemBody) + scalers.size()*sizeof(uint32_t),
        size_t(pItem->s_header.s_size)
    );
    
    const v11::BodyHeader* pH =
        reinterpret_cast<const v11::BodyHeader*>(&pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(sizeof(v11::BodyHeader), size_t(pH->s_size));
    EQ(uint64_t(0xffffffffffffffff), pH->s_timestamp);
    EQ(uint32_t(0), pH->s_sourceId);
    EQ(uint32_t(0), pH->s_barrier);
    
    const v11::ScalerItemBody* pBody =
        reinterpret_cast<const v11::ScalerItemBody*>(
            &pItem->s_body.u_hasBodyHeader.s_body
        );
    EQ(uint32_t(10), pBody->s_intervalStartOffset);
    EQ(uint32_t(20), pBody->s_intervalEndOffset);
    EQ(uint32_t(now), pBody->s_timestamp);
    EQ(uint32_t(1), pBody->s_intervalDivisor);
    EQ(uint32_t(scalers.size()), pBody->s_scalerCount);
    EQ(uint32_t(1), pBody->s_isIncremental);
    for (int i = 0; i < scalers.size(); i++) {
        EQ(scalers[i], pBody->s_scalers[i]);
    }
}
// full construcxtion
void v11sctest::construct_3()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i*123);
    }
    v11::CRingScalerItem item(
        0x1234567890, 1, 0, 10, 20, now, scalers
    );
    
     const v11::ScalerItem* pItem =
        reinterpret_cast<const v11::ScalerItem*>(item.getItemPointer());
    EQ(v11::PERIODIC_SCALERS, pItem->s_header.s_type);
    EQ(
        sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader)
        + sizeof(v11::ScalerItemBody) + scalers.size()*sizeof(uint32_t),
        size_t(pItem->s_header.s_size)
    );
    
    const v11::BodyHeader* pH =
        reinterpret_cast<const v11::BodyHeader*>(&pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(sizeof(v11::BodyHeader), size_t(pH->s_size));
    EQ(uint64_t(0x1234567890), pH->s_timestamp);
    EQ(uint32_t(1), pH->s_sourceId);
    EQ(uint32_t(0), pH->s_barrier);
    
    const v11::ScalerItemBody* pBody =
        reinterpret_cast<const v11::ScalerItemBody*>(
            &pItem->s_body.u_hasBodyHeader.s_body
        );
    EQ(uint32_t(10), pBody->s_intervalStartOffset);
    EQ(uint32_t(20), pBody->s_intervalEndOffset);
    EQ(uint32_t(now), pBody->s_timestamp);
    EQ(uint32_t(1), pBody->s_intervalDivisor);
    EQ(uint32_t(scalers.size()), pBody->s_scalerCount);
    EQ(uint32_t(1), pBody->s_isIncremental);
    for (int i = 0; i < scalers.size(); i++) {
        EQ(scalers[i], pBody->s_scalers[i]);
    }                       
    
}
// Get start time when there's no body header:

void
v11sctest::starttime_1()
{
    v11::CRingScalerItem item(32);
    EQ(uint32_t(0), item.getStartTime());
}
// Set start time when there's no body header
void
v11sctest::starttime_2()
{
    v11::CRingScalerItem item(32);
    item.setStartTime(10);
    EQ(uint32_t(10), item.getStartTime());
}

// get start time when there's a body header:

void v11sctest::starttime_3()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i*123);
    }
    v11::CRingScalerItem item(
        0x1234567890, 1, 0, 10, 20, now, scalers
    );
    EQ(uint32_t(10), item.getStartTime());
}
// set start time when there's a body header

void v11sctest::starttime_4()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i*123);
    }
    v11::CRingScalerItem item(
        0x1234567890, 1, 0, 10, 20, now, scalers
    );
    
    item.setStartTime(5);
    EQ(uint32_t(5), item.getStartTime());
}
// Compute start time for non body header/
void v11sctest::starttime_5()
{
    v11::CRingScalerItem item(32);
    v11::pScalerItem pItem = reinterpret_cast<v11::pScalerItem>(item.getItemPointer());
    
    // set non zero non-one start and divisor.
    
    pItem->s_body.u_noBodyHeader.s_body.s_intervalStartOffset =10;
    pItem->s_body.u_noBodyHeader.s_body.s_intervalDivisor =2;
    
    EQ(float(5.0), item.computeStartTime());
}
// Compute start time for body header:

void v11sctest::starttime_6()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i*123);
    }
    v11::CRingScalerItem item(
        0x1234567890, 1, 0, 10, 20, now, scalers, 2
    );
    EQ(float(5.0), item.computeStartTime());
}
// get end time from non-body header:

void v11sctest::endtime_1()
{
    v11::CRingScalerItem item(32);
    v11::pScalerItem pItem = reinterpret_cast<v11::pScalerItem>(item.getItemPointer());    
    pItem->s_body.u_noBodyHeader.s_body.s_intervalEndOffset =20;
    
    EQ(uint32_t(20), item.getEndTime());
}
// set en time from non body header:

void v11sctest::endtime_2()
{
    v11::CRingScalerItem item(32);
    item.setEndTime(20);
    EQ(uint32_t(20), item.getEndTime());
}
// compute end time - no body header.
void v11sctest::endtime_3()
{
    v11::CRingScalerItem item(32);
    item.setEndTime(20);
    v11::pScalerItem pItem = reinterpret_cast<v11::pScalerItem>(item.getItemPointer());    
    pItem->s_body.u_noBodyHeader.s_body.s_intervalDivisor =2;
    EQ(float(10), item.computeEndTime());
}
// get end time for body header item:

void v11sctest::endtime_4()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i*123);
    }
    v11::CRingScalerItem item(
        0x1234567890, 1, 0, 10, 20, now, scalers, 2
    );
    EQ(uint32_t(20), item.getEndTime());
}
// set tend time for body header item:

void v11sctest::endtime_5()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i*123);
    }
    v11::CRingScalerItem item(
        0x1234567890, 1, 0, 10, 20, now, scalers, 2
    );
    
    item.setEndTime(25);
    EQ(uint32_t(25), item.getEndTime());
}
// Compute end time with body header:

void v11sctest::endtime_6()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i*123);
    }
    v11::CRingScalerItem item(
        0x1234567890, 1, 0, 10, 20, now, scalers, 2
    );
    EQ(float(10), item.computeEndTime());
}

// divisor from non body header item:

void v11sctest::div_1()
{
    v11::CRingScalerItem item(32);
    EQ(uint32_t(1), item.getTimeDivisor());
    
}
// get divisor from body header item
//
void v11sctest::div_2()
{
  time_t now = time(nullptr);
  std::vector<uint32_t> scalers;
  for (int i =0; i < 32; i++) {
      scalers.push_back(i*123);
  }
  v11::CRingScalerItem item(
      0x1234567890, 1, 0, 10, 20, now, scalers, 2
  );
  EQ(uint32_t(2), item.getTimeDivisor());
}
// get timestamp from a non body header.
void v11sctest::stamp_1()
{
    time_t now = time(nullptr);
    v11::CRingScalerItem item(32);
    v11::pScalerItem pItem = reinterpret_cast<v11::pScalerItem>(item.getItemPointer());
    pItem->s_body.u_noBodyHeader.s_body.s_timestamp = now;
    
    EQ(now, item.getTimestamp());
  
}
/// Set timestamp nono body header.

void v11sctest::stamp_2()
{
    v11::CRingScalerItem item(32);
    time_t t = time(nullptr) + 10;
    item.setTimestamp(t);
    EQ(t, item.getTimestamp());
}

// GET timestamp from body header item.
void v11sctest::stamp_3()
{
    time_t now = time(nullptr) + 5;
  std::vector<uint32_t> scalers;
  for (int i =0; i < 32; i++) {
      scalers.push_back(i*123);
  }
  v11::CRingScalerItem item(
      0x1234567890, 1, 0, 10, 20, now, scalers, 2
  );
  EQ(now, item.getTimestamp());
}
// set timestamp in body header item.
void v11sctest::stamp_4()
{
  time_t now = time(nullptr);
  std::vector<uint32_t> scalers;
  for (int i =0; i < 32; i++) {
      scalers.push_back(i*123);
  }
  v11::CRingScalerItem item(
      0x1234567890, 1, 0, 10, 20, now, scalers, 2
  );
  item.setTimestamp(now+10);
  EQ(now+10, item.getTimestamp());
}
// incremental nonbody heaer.

void v11sctest::incremental_1()
{
    v11::CRingScalerItem item(32);
    v11::pScalerItem pItem = reinterpret_cast<v11::pScalerItem>(item.getItemPointer());
    pItem->s_body.u_noBodyHeader.s_body.s_isIncremental =1;
    
    ASSERT(item.isIncremental());
}
// not incremental no body header

void v11sctest::incremental_2()
{
    v11::CRingScalerItem item(32);
    v11::pScalerItem pItem = reinterpret_cast<v11::pScalerItem>(item.getItemPointer());
    pItem->s_body.u_noBodyHeader.s_body.s_isIncremental = 0;
    
    ASSERT(!item.isIncremental());    
}
// incremental with body header.
void v11sctest::incremental_3()
{
    time_t now = time(nullptr);
  std::vector<uint32_t> scalers;
  for (int i =0; i < 32; i++) {
      scalers.push_back(i*123);
  }
  v11::CRingScalerItem item(
      0x1234567890, 1, 0, 10, 20, now, scalers, 2, true
  );
  ASSERT(item.isIncremental());
}
// not incremental with body header.

void v11sctest::incremental_4()
{
  time_t now = time(nullptr);
  std::vector<uint32_t> scalers;
  for (int i =0; i < 32; i++) {
      scalers.push_back(i*123);
  }
  v11::CRingScalerItem item(
      0x1234567890, 1, 0, 10, 20, now, scalers, 2, false
  );
  ASSERT(!item.isIncremental());
}
// set scaler valid index no body header.
void v11sctest::setscaler_1()
{
    v11::CRingScalerItem item(32);
    item.setScaler(3, 12345);
    v11::pScalerItem pItem = reinterpret_cast<v11::pScalerItem>(item.getItemPointer());
    EQ(
        uint32_t(12345),
        pItem->s_body.u_noBodyHeader.s_body.s_scalers[3]
    );
    
}
// set scaler invalid index no body header.
void v11sctest::setscaler_2()
{
    v11::CRingScalerItem item(32);
    CPPUNIT_ASSERT_THROW(
        item.setScaler(32, 1234),
        std::out_of_range
    );
}

// set scaler valid index - body header.

void v11sctest::setscaler_3()
{
  time_t now = time(nullptr);
  std::vector<uint32_t> scalers;
  for (int i =0; i < 32; i++) {
      scalers.push_back(i*123);
  }
  v11::CRingScalerItem item(
      0x1234567890, 1, 0, 10, 20, now, scalers, 2, false
  );
  item.setScaler(3, 12345);
  v11::pScalerItem pItem = reinterpret_cast<v11::pScalerItem>(item.getItemPointer());
  EQ(
    uint32_t(12345),
    pItem->s_body.u_hasBodyHeader.s_body.s_scalers[3]
  );
}
// setscaler invalid index body header.

void v11sctest::setscaler_4()
{
  time_t now = time(nullptr);
  std::vector<uint32_t> scalers;
  for (int i =0; i < 32; i++) {
      scalers.push_back(i*123);
  }
  v11::CRingScalerItem item(
      0x1234567890, 1, 0, 10, 20, now, scalers, 2, false
  );
  CPPUNIT_ASSERT_THROW(
    item.setScaler(32, 1234),
    std::out_of_range
  );
}

// getscaler in range nonbody header.

void v11sctest::getscaler_1()
{
    v11::CRingScalerItem item(32);
    v11::pScalerItem pItem = reinterpret_cast<v11::pScalerItem>(item.getItemPointer());
    pItem->s_body.u_noBodyHeader.s_body.s_scalers[5] = 12345;
    EQ(uint32_t(12345), item.getScaler(5));
}
// get scaler out of range non body header.

void v11sctest::getscaler_2()
{
    v11::CRingScalerItem item(32);
    CPPUNIT_ASSERT_THROW(
        item.getScaler(32),
        std::out_of_range
    );
}
// get scaler for body header:

void v11sctest::getscaler_3()
{
  time_t now = time(nullptr);
  std::vector<uint32_t> scalers;
  for (int i =0; i < 32; i++) {
      scalers.push_back(i*123);
  }
  v11::CRingScalerItem item(
      0x1234567890, 1, 0, 10, 20, now, scalers, 2, false
  );
  for (int i =0; i < scalers.size(); i++) {
    EQ(scalers[i], item.getScaler(i));
  }
}
// Get scaler for body header out of range.
void v11sctest::getscaler_4()
{
  time_t now = time(nullptr);
  std::vector<uint32_t> scalers;
  for (int i =0; i < 32; i++) {
      scalers.push_back(i*123);
  }
  v11::CRingScalerItem item(
      0x1234567890, 1, 0, 10, 20, now, scalers, 2, false
  );
  CPPUNIT_ASSERT_THROW(
    item.getScaler(32),
    std::out_of_range
  );
}
/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#include "fboss/agent/FbossError.h"
#include "fboss/agent/hw/sai/api/SaiApiTable.h"
#include "fboss/agent/hw/sai/fake/FakeSai.h"
#include "fboss/agent/hw/sai/switch/SaiManagerTable.h"
#include "fboss/agent/hw/sai/switch/SaiNeighborManager.h"
#include "fboss/agent/hw/sai/switch/tests/ManagerTestBase.h"
#include "fboss/agent/state/ArpEntry.h"
#include "fboss/agent/state/NdpEntry.h"
#include "fboss/agent/types.h"

using namespace facebook::fboss;
class NeighborManagerTest : public ManagerTestBase {
 public:
  void SetUp() override {
    setupStage = SetupStage::PORT | SetupStage::VLAN | SetupStage::INTERFACE;
    ManagerTestBase::SetUp();
    intf0 = testInterfaces[0];
    h0 = intf0.remoteHosts[0];
  }

  template <typename NeighborEntryT>
  void checkEntry(
      const NeighborEntryT& neighborEntry,
      const folly::MacAddress& expectedDstMac) {
    auto saiEntry =
        saiManagerTable->neighborManager().saiEntryFromSwEntry(neighborEntry);
    auto gotMac = saiApiTable->neighborApi().getAttribute(
        NeighborApiParameters::Attributes::DstMac(), saiEntry);
    EXPECT_EQ(gotMac, expectedDstMac);
    auto saiNeighbor = saiManagerTable->neighborManager().getNeighbor(saiEntry);
    EXPECT_TRUE(saiNeighbor);
  }

  template <typename NeighborEntryT>
  void checkMissing(const NeighborEntryT& neighborEntry) {
    auto saiEntry =
        saiManagerTable->neighborManager().saiEntryFromSwEntry(neighborEntry);
    auto saiNeighbor = saiManagerTable->neighborManager().getNeighbor(saiEntry);
    EXPECT_FALSE(saiNeighbor);
  }

  TestInterface intf0;
  TestRemoteHost h0;
};

TEST_F(NeighborManagerTest, addResolvedNeighbor) {
  auto arpEntry = makeArpEntry(intf0.id, h0);
  saiManagerTable->neighborManager().addNeighbor(arpEntry);
  checkEntry(arpEntry, h0.mac);
}

TEST_F(NeighborManagerTest, removeResolvedNeighbor) {
  auto arpEntry = makeArpEntry(intf0.id, h0);
  saiManagerTable->neighborManager().addNeighbor(arpEntry);
  checkEntry(arpEntry, h0.mac);
  saiManagerTable->neighborManager().removeNeighbor(arpEntry);
  checkMissing(arpEntry);
}

TEST_F(NeighborManagerTest, addUnresolvedNeighbor) {
  // TODO (D13604051)
}

TEST_F(NeighborManagerTest, removeUnresolvedNeighbor) {
  // TODO (D13604051)
}

TEST_F(NeighborManagerTest, resolveNeighbor) {
  // TODO (D13604051)
}

TEST_F(NeighborManagerTest, unresolveNeighbor) {
  // TODO (D13604051)
}

TEST_F(NeighborManagerTest, getNonexistentNeighbor) {
  auto arpEntry = makeArpEntry(intf0.id, h0);
  checkMissing(arpEntry);
}

TEST_F(NeighborManagerTest, removeNonexistentNeighbor) {
  auto arpEntry = makeArpEntry(intf0.id, h0);
  EXPECT_THROW(
      saiManagerTable->neighborManager().removeNeighbor(arpEntry), FbossError);
}

TEST_F(NeighborManagerTest, addDuplicateResolvedNeighbor) {
  auto arpEntry = makeArpEntry(intf0.id, h0);
  saiManagerTable->neighborManager().addNeighbor(arpEntry);
  EXPECT_THROW(
      saiManagerTable->neighborManager().addNeighbor(arpEntry), FbossError);
}

TEST_F(NeighborManagerTest, addDuplicateUnresolvedNeighbor) {
  // TODO (D13604051)
}

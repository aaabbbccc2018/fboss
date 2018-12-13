/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#pragma once

#include "fboss/agent/gen-cpp2/switch_config_types.h"
#include "fboss/agent/gen-cpp2/switch_state_types.h"
#include "fboss/agent/state/Mirror.h"
#include "fboss/agent/state/PortQueue.h"
#include "fboss/agent/state/Thrifty.h"
#include "fboss/agent/types.h"

#include <boost/container/flat_map.hpp>
#include <string>
#include <vector>

namespace facebook { namespace fboss {

class SwitchState;

struct PortFields {
  struct VlanInfo {
    explicit VlanInfo(bool emitTags) : tagged(emitTags) {}
    bool operator==(const VlanInfo& other) const {
      return tagged == other.tagged;
    }
    bool operator!=(const VlanInfo& other) const {
      return !(*this == other);
    }
    state::VlanInfo toThrift() const;
    static VlanInfo fromThrift(state::VlanInfo const&);
    bool tagged;
  };

  using VlanMembership = boost::container::flat_map<VlanID, VlanInfo>;

  enum class OperState {
    DOWN = 0,
    UP = 1,
  };

  PortFields(PortID id, std::string name)
    : id(id),
      name(name) {}

  template <typename Fn>
  void forEachChild(Fn /*fn*/) {}

  static PortFields fromThrift(state::PortFields const& pf);

  state::PortFields toThrift() const;

  const PortID id{0};
  std::string name;
  std::string description;
  cfg::PortState adminState{cfg::PortState::DISABLED}; // is the port enabled
  OperState operState{OperState::DOWN}; // is the port actually up
  VlanID ingressVlan{0};
  cfg::PortSpeed speed{cfg::PortSpeed::DEFAULT};
  cfg::PortPause pause;
  VlanMembership vlans;
  // settings for ingress/egress sFlow sampling rate; we sample every 1:N'th
  // packets randomly based on those settings. Zero means no sampling.
  int64_t sFlowIngressRate{0};
  int64_t sFlowEgressRate{0};
  QueueConfig queues;
  cfg::PortFEC fec{cfg::PortFEC::OFF};  // TODO: should this default to ON?
  cfg::PortLoopbackMode loopbackMode{cfg::PortLoopbackMode::NONE};
  folly::Optional<std::string> ingressMirror;
  folly::Optional<std::string> egressMirror;
  folly::Optional<std::string> qosPolicy;
};

/*
 * Port stores state about one of the physical ports on the switch.
 */
class Port : public ThriftyBaseT<state::PortFields, Port, PortFields> {
 public:
  using VlanInfo = PortFields::VlanInfo;
  using VlanMembership = PortFields::VlanMembership;
  using OperState = PortFields::OperState;

  Port(PortID id, const std::string& name);

  /*
   * Initialize a cfg::Port object with the default settings
   * that would be applied for this port. Port identifiers
   * like port ID and name are retained.
   *
   * The resulting cfg::Port object can be passed to applyConfig() to return
   * the port to a state as if it had been newly constructed.
   */
  void initDefaultConfigState(cfg::Port* config) const;

  PortID getID() const {
    return getFields()->id;
  }

  const std::string& getName() const {
    return getFields()->name;
  }

  void setName(const std::string& name) {
    writableFields()->name = name;
  }

  const std::string& getDescription() const {
    return getFields()->description;
  }

  void setDescription(const std::string& description) {
    writableFields()->description = description;
  }

  cfg::PortState getAdminState() const {
    return getFields()->adminState;
  }

  void setAdminState(cfg::PortState adminState) {
    writableFields()->adminState = adminState;
  }

  OperState getOperState() const {
    return getFields()->operState;
  }

  void setOperState(bool isUp) {
    writableFields()->operState = isUp ? OperState::UP : OperState::DOWN;
  }

  bool isEnabled() const {
    return getFields()->adminState == cfg::PortState::ENABLED;
  }

  bool isUp() const {
    return getFields()->operState == OperState::UP;
  }

  /**
   * Tells you Oper+Admin state of port. Will be UP only if both admin and
   * oper state is UP.
   */
  bool isPortUp() const {
    return isEnabled() && isUp();
  }

  const VlanMembership& getVlans() const {
    return getFields()->vlans;
  }
  void setVlans(VlanMembership vlans) {
    writableFields()->vlans.swap(vlans);
  }

  const QueueConfig& getPortQueues() {
    return getFields()->queues;
  }

  void resetPortQueues(QueueConfig queues) {
    writableFields()->queues.swap(queues);
  }

  VlanID getIngressVlan() const {
    return getFields()->ingressVlan;
  }
  void setIngressVlan(VlanID id) {
    writableFields()->ingressVlan = id;
  }

  cfg::PortSpeed getSpeed() const {
    return getFields()->speed;
  }
  void setSpeed(cfg::PortSpeed speed) {
    writableFields()->speed = speed;
  }

  cfg::PortPause getPause() const {
    return getFields()->pause;
  }
  void setPause(cfg::PortPause pause) {
    writableFields()->pause = pause;
  }

  cfg::PortFEC getFEC() const {
    return getFields()->fec;
  }
  void setFEC(cfg::PortFEC fec) {
    writableFields()->fec = fec;
  }

  cfg::PortLoopbackMode getLoopbackMode() const {
    return getFields()->loopbackMode;
  }
  void setLoopbackMode(cfg::PortLoopbackMode loopbackMode) {
    writableFields()->loopbackMode = loopbackMode;
  }

  int64_t getSflowIngressRate() const {
    return getFields()->sFlowIngressRate;
  }
  void setSflowIngressRate(int64_t ingressRate) {
    writableFields()->sFlowIngressRate = ingressRate;
  }

  int64_t getSflowEgressRate() const {
    return getFields()->sFlowEgressRate;
  }
  void setSflowEgressRate(int64_t egressRate) {
    writableFields()->sFlowEgressRate = egressRate;
  }

  folly::Optional<std::string> getIngressMirror() const {
    return getFields()->ingressMirror;
  }

  void setIngressMirror(folly::Optional<std::string> mirror) {
    writableFields()->ingressMirror.assign(mirror);
  }

  folly::Optional<std::string> getEgressMirror() const {
    return getFields()->egressMirror;
  }

  void setEgressMirror(folly::Optional<std::string> mirror) {
    writableFields()->egressMirror.assign(mirror);
  }

  folly::Optional<std::string> getQosPolicy() const {
    return getFields()->qosPolicy;
  }

  void setQosPolicy(folly::Optional<std::string> qosPolicy) {
    writableFields()->qosPolicy.assign(qosPolicy);
  }

  Port* modify(std::shared_ptr<SwitchState>* state);

 private:
  // Inherit the constructors required for clone()
  using ThriftyBaseT<state::PortFields, Port, PortFields>::ThriftyBaseT;
  friend class CloneAllocator;
};

}} // facebook::fboss

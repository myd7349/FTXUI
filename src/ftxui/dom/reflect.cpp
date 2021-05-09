#include <ext/alloc_traits.h>  // for __alloc_traits<>::value_type
#include <memory>              // for make_shared, __shared_ptr_access
#include <utility>             // for move
#include <vector>              // for vector

#include "ftxui/dom/elements.hpp"     // for Element, unpack, Decorator, reflect
#include "ftxui/dom/node.hpp"         // for Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box

namespace ftxui {

// Helper class.
class Reflect : public Node {
 public:
  Reflect(Element child, Box& box)
      : Node(unpack(std::move(child))), box_(box) {}
  ~Reflect() override {}

  void ComputeRequirement() final {
    Node::ComputeRequirement();
    requirement_ = children[0]->requirement();
  }

  void SetBox(Box box) final {
    box_ = box;
    Node::SetBox(box_);
    children[0]->SetBox(box_);
  }

 private:
  Box& box_;
};

Decorator reflect(Box& box) {
  return [&](Element child) -> Element {
    return std::make_shared<Reflect>(std::move(child), box);
  };
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

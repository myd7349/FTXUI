#include <stddef.h>   // for size_t
#include <algorithm>  // for max, min
#include <memory>     // for shared_ptr, allocator_traits<>::value_type
#include <utility>    // for move

#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowLeft, Event::ArrowRight, Event::Return, Event::Tab, Event::TabReverse
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/component/toggle.hpp"

namespace ftxui {

Component Toggle(const std::vector<std::wstring>* entries, int* selected) {
  return Make<ToggleBase>(entries, selected);
}

// static
ToggleBase* ToggleBase::From(Component component) {
  return static_cast<ToggleBase*>(component.get());
}

ToggleBase::ToggleBase(const std::vector<std::wstring>* entries, int* selected)
    : entries_(entries), selected_(selected) {}

Element ToggleBase::Render() {
  Elements children;
  bool is_toggle_focused = Focused();
  boxes_.resize(entries_->size());
  for (size_t i = 0; i < entries_->size(); ++i) {
    // Separator.
    if (i != 0)
      children.push_back(separator());

    bool is_focused = (focused == int(i)) && is_toggle_focused;
    bool is_selected = (*selected_ == int(i));

    auto style = is_selected
                     ? (is_focused ? selected_focused_style : selected_style)
                     : (is_focused ? focused_style : normal_style);
    auto focus_management = !is_selected        ? nothing
                            : is_toggle_focused ? focus
                                                : select;
    children.push_back(text(entries_->at(i)) | style | focus_management |
                       reflect(boxes_[i]));
  }
  return hbox(std::move(children));
}

bool ToggleBase::OnEvent(Event event) {
  if (event.is_mouse())
    return OnMouseEvent(event);

  int old_selected = *selected_;
  if (event == Event::ArrowLeft || event == Event::Character('h'))
    (*selected_)--;
  if (event == Event::ArrowRight || event == Event::Character('l'))
    (*selected_)++;
  if (event == Event::Tab && entries_->size())
    *selected_ = (*selected_ + 1) % entries_->size();
  if (event == Event::TabReverse && entries_->size())
    *selected_ = (*selected_ + entries_->size() - 1) % entries_->size();

  *selected_ = std::max(0, std::min(int(entries_->size()) - 1, *selected_));

  if (old_selected != *selected_) {
    focused = *selected_;
    on_change();
    return true;
  }

  if (event == Event::Return) {
    on_enter();
    return true;
  }

  return false;
}

bool ToggleBase::OnMouseEvent(Event event) {
  if (!CaptureMouse(event))
    return false;
  for (int i = 0; i < boxes_.size(); ++i) {
    if (!boxes_[i].Contain(event.mouse().x, event.mouse().y))
      continue;

    TakeFocus();
    focused = i;
    if (event.mouse().button == Mouse::Left &&
        event.mouse().motion == Mouse::Pressed) {
      TakeFocus();
      if (*selected_ != i) {
        *selected_ = i;
        on_change();
      }
      return true;
    }
  }
  return false;
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

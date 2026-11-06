///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_GUI_GUI_HPP
#define POLOS_GUI_GUI_HPP

#include "polos/polos_api.hpp"
#include "polos/utils/string_id.hpp"

#include <cstdint>
#include <span>
#include <string_view>

namespace polos::gui
{

struct Vec2
{
    float x{0.0F};
    float y{0.0F};
};

struct Color
{
    float r{1.0F};
    float g{1.0F};
    float b{1.0F};
    float a{1.0F};
};

enum class Condition
{
    kAlways,
    kOnce,
    kFirstUse,
    kAppearing,
};

enum class WindowFlags : std::uint32_t
{
    kNone         = 0,
    kNoTitleBar   = 1U << 0,
    kNoResize     = 1U << 1,
    kNoMove       = 1U << 2,
    kNoScrollbar  = 1U << 3,
    kNoCollapse   = 1U << 4,
    kAutoResize   = 1U << 5,
    kNoBackground = 1U << 6,
    kMenuBar      = 1U << 7,
    kNoInputs     = 1U << 8,
    kNoDocking    = 1U << 9,
};

enum class TableFlags : std::uint32_t
{
    kNone        = 0,
    kResizable   = 1U << 0,
    kReorderable = 1U << 1,
    kHideable    = 1U << 2,
    kStripedRows = 1U << 3,
    kBorders     = 1U << 4,
    kScrollY     = 1U << 5,
};

enum class StyleColor
{
    kText,
    kWindowBackground,
    kBorder,
    kFrameBackground,
    kButton,
    kButtonHovered,
    kButtonActive,
    kHeader,
};

// Call End and EndChild after their matching Begin calls, even when Begin returns false.
// For the other Begin/End pairs, call End only when Begin returns true.
auto POLOS_API operator|(WindowFlags tLeft, WindowFlags tRight) -> WindowFlags;
auto POLOS_API operator|(TableFlags tLeft, TableFlags tRight) -> TableFlags;

// Windows and docking
auto POLOS_API Begin(char const* tTitle, bool* tOpen = nullptr, WindowFlags tFlags = WindowFlags::kNone) -> bool;
auto POLOS_API End() -> void;
auto POLOS_API
BeginChild(char const* tId, Vec2 tSize = {}, bool tBorder = false, WindowFlags tFlags = WindowFlags::kNone) -> bool;
auto POLOS_API EndChild() -> void;
auto POLOS_API SetNextWindowPosition(Vec2 tPosition, Condition tCondition = Condition::kAlways, Vec2 tPivot = {})
    -> void;
auto POLOS_API SetNextWindowSize(Vec2 tSize, Condition tCondition = Condition::kAlways) -> void;
auto POLOS_API SetNextWindowFocus() -> void;
auto POLOS_API SetNextWindowBackgroundAlpha(float tAlpha) -> void;
auto POLOS_API IsWindowHovered() -> bool;
auto POLOS_API IsWindowFocused() -> bool;
auto POLOS_API GetWindowSize() -> Vec2;
auto POLOS_API DockSpaceOverMainViewport(bool tPassthroughBackground = false) -> void;
auto POLOS_API ShowDemoWindow(bool* tOpen = nullptr) -> void;

// Layout and IDs
auto POLOS_API Separator() -> void;
auto POLOS_API Spacing() -> void;
auto POLOS_API NewLine() -> void;
auto POLOS_API BeginGroup() -> void;
auto POLOS_API EndGroup() -> void;
auto POLOS_API PopID() -> void;
auto POLOS_API Unindent() -> void;
auto POLOS_API SetItemDefaultFocus() -> void;
auto POLOS_API SameLine(float tOffset = 0.0F, float tSpacing = -1.0F) -> void;
auto POLOS_API Indent(float tWidth = 0.0F) -> void;
auto POLOS_API Dummy(Vec2 tSize) -> void;
auto POLOS_API SetNextItemWidth(float tWidth) -> void;
auto POLOS_API PushID(char const* tId) -> void;
auto POLOS_API PushID(int tId) -> void;
auto POLOS_API GetContentRegionAvailable() -> Vec2;
auto POLOS_API GetCursorScreenPosition() -> Vec2;
auto POLOS_API SetCursorScreenPosition(Vec2 tPosition) -> void;

// Text and widgets
auto POLOS_API Text(std::string_view tText) -> void;
auto POLOS_API TextColored(Color tColor, std::string_view tText) -> void;
auto POLOS_API TextWrapped(std::string_view tText) -> void;
auto POLOS_API SeparatorText(char const* tLabel) -> void;
auto POLOS_API Button(char const* tLabel, Vec2 tSize = {}) -> bool;
auto POLOS_API SmallButton(char const* tLabel) -> bool;
auto POLOS_API Checkbox(char const* tLabel, bool& tValue) -> bool;
auto POLOS_API RadioButton(char const* tLabel, bool tActive) -> bool;
auto POLOS_API Selectable(char const* tLabel, bool tSelected = false) -> bool;
auto POLOS_API SliderFloat(char const* tLabel, float& tValue, float tMin, float tMax, char const* tFormat = "%.3f")
    -> bool;
auto POLOS_API SliderInt(char const* tLabel, int& tValue, int tMin, int tMax) -> bool;
auto POLOS_API DragFloat(char const* tLabel, float& tValue, float tSpeed = 1.0F, float tMin = 0.0F, float tMax = 0.0F)
    -> bool;
auto POLOS_API DragInt(char const* tLabel, int& tValue, float tSpeed = 1.0F, int tMin = 0, int tMax = 0) -> bool;
auto POLOS_API InputInt(char const* tLabel, int& tValue) -> bool;
auto POLOS_API InputFloat(char const* tLabel, float& tValue) -> bool;
// The text buffer is writable, NUL-terminated, and includes space for the terminator.
auto POLOS_API InputText(char const* tLabel, std::span<char> tBuffer) -> bool;
auto POLOS_API InputTextMultiline(char const* tLabel, std::span<char> tBuffer, Vec2 tSize = {}) -> bool;
auto POLOS_API ColorEdit(char const* tLabel, Color& tColor) -> bool;
auto POLOS_API ProgressBar(float tFraction, Vec2 tSize = {-1.0F, 0.0F}, char const* tOverlay = nullptr) -> void;
auto POLOS_API PlotLines(char const* tLabel, std::span<float const> tValues, Vec2 tSize = {}) -> void;
// Displays an image from the resource catalog. Returns false when the texture cannot be displayed.
auto POLOS_API Image(utils::string_id tTexture, Vec2 tSize, Vec2 tUvMin = {}, Vec2 tUvMax = {1.0F, 1.0F}) -> bool;

// Lists, trees, tabs, and tables
auto POLOS_API BeginCombo(char const* tLabel, char const* tPreview) -> bool;
auto POLOS_API EndCombo() -> void;
auto POLOS_API BeginListBox(char const* tLabel, Vec2 tSize = {}) -> bool;
auto POLOS_API EndListBox() -> void;
auto POLOS_API BeginTabBar(char const* tId) -> bool;
auto POLOS_API EndTabBar() -> void;
auto POLOS_API BeginTabItem(char const* tLabel, bool* tOpen = nullptr) -> bool;
auto POLOS_API EndTabItem() -> void;
auto POLOS_API TreeNode(char const* tLabel) -> bool;
auto POLOS_API TreePop() -> void;
auto POLOS_API CollapsingHeader(char const* tLabel) -> bool;
auto POLOS_API SetNextItemOpen(bool tOpen, Condition tCondition = Condition::kAlways) -> void;
auto POLOS_API BeginTable(char const* tId, int tColumns, TableFlags tFlags = TableFlags::kNone, Vec2 tOuterSize = {})
    -> bool;
auto POLOS_API EndTable() -> void;
auto POLOS_API TableSetupColumn(char const* tLabel) -> void;
auto POLOS_API TableHeadersRow() -> void;
auto POLOS_API TableNextRow() -> void;
auto POLOS_API TableNextColumn() -> bool;
auto POLOS_API TableSetColumnIndex(int tIndex) -> bool;

// Menus, popups, and tooltips
auto POLOS_API BeginMainMenuBar() -> bool;
auto POLOS_API EndMainMenuBar() -> void;
auto POLOS_API BeginMenuBar() -> bool;
auto POLOS_API EndMenuBar() -> void;
auto POLOS_API BeginTooltip() -> bool;
auto POLOS_API EndTooltip() -> void;
auto POLOS_API BeginMenu(char const* tLabel, bool tEnabled = true) -> bool;
auto POLOS_API EndMenu() -> void;
auto POLOS_API
MenuItem(char const* tLabel, char const* tShortcut = nullptr, bool tSelected = false, bool tEnabled = true) -> bool;
auto POLOS_API OpenPopup(char const* tId) -> void;
auto POLOS_API BeginPopup(char const* tId, WindowFlags tFlags = WindowFlags::kNone) -> bool;
auto POLOS_API BeginPopupModal(char const* tTitle, bool* tOpen = nullptr, WindowFlags tFlags = WindowFlags::kNone)
    -> bool;
auto POLOS_API EndPopup() -> void;
auto POLOS_API CloseCurrentPopup() -> void;

// Interaction and style
auto POLOS_API IsItemHovered() -> bool;
auto POLOS_API IsItemActive() -> bool;
auto POLOS_API IsItemEdited() -> bool;
auto POLOS_API IsItemDeactivatedAfterEdit() -> bool;
auto POLOS_API IsItemClicked() -> bool;
auto POLOS_API BeginDisabled(bool tDisabled = true) -> void;
auto POLOS_API EndDisabled() -> void;
auto POLOS_API PushStyleColor(StyleColor tTarget, Color tColor) -> void;
auto POLOS_API PopStyleColor(int tCount = 1) -> void;
auto POLOS_API SetDarkTheme() -> void;
auto POLOS_API SetLightTheme() -> void;
auto POLOS_API WantsMouse() -> bool;
auto POLOS_API WantsKeyboard() -> bool;
auto POLOS_API WantsTextInput() -> bool;

}// namespace polos::gui

#endif// POLOS_GUI_GUI_HPP

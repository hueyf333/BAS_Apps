#pragma once

namespace TopoShift {

class Application;

class AlignmentPanel {
public:
    AlignmentPanel();
    ~AlignmentPanel() = default;

    void render(Application& app);

    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }
    void toggle() { m_visible = !m_visible; }

private:
    bool m_visible;
};

} // namespace TopoShift

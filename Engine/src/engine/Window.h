#pragma once

#include <glm/vec2.hpp>

class Window
{
public:
    Window() = default;
    virtual ~Window() = default;

    virtual bool isValid() const = 0;

    // The frame buffer size in pixels.
    // Can be bigger then the window size on Retina displays.
    virtual glm::ivec2 getFramebufferSize() const = 0;
    
    // The window size in pixels
    virtual glm::ivec2 getWindowSize() const = 0;
    virtual float getAspectRatio() const = 0;
};

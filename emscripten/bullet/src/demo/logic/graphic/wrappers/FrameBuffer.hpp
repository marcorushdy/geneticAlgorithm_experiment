
#pragma once

#include "Texture.hpp"
#include "RenderBuffer.hpp"

#include "demo/helpers/OpenGLES.hpp"
#include "demo/helpers/GLMath.hpp"

class FrameBuffer
{
private:
    GLuint _frameBufferId = 0;

public:
    FrameBuffer() = default;
    ~FrameBuffer();

public:
    void initialise();
    void attachColorTexture(Texture& texture);
    void attachDepthRenderBuffer(RenderBuffer& buffer);

public:
    void executeCheck() const;

public:
    void bind() const;

public:
    static void unbind();

};

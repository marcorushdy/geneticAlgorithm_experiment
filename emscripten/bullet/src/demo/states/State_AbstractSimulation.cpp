
#include "demo/defines.hpp"

#include "State_AbstractSimulation.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/Scene.hpp"

#include "demo/helpers/GLMath.hpp"

#include <limits> // std::numeric_limits<T>::max();
#include <cmath> // std::ceil

void State_AbstractSimulation::enter()
{
    // D_MYLOG("step");
}

void State_AbstractSimulation::leave()
{
    // D_MYLOG("step");
}

//

void State_AbstractSimulation::handleEvent(const SDL_Event& event)
{
    auto& data = Data::get();
    auto& keys = data.inputs.keys;
    auto& mouse = data.inputs.mouse;

    switch (event.type)
    {
        case SDL_KEYDOWN:
        {
            keys[event.key.keysym.sym] = true;
            break;
        }
        case SDL_KEYUP:
        {
            keys[event.key.keysym.sym] = false;
            break;
        }

        case SDL_MOUSEBUTTONDOWN:
        {
            mouse.position = { event.motion.x, event.motion.y };
            mouse.delta = { 0, 0 };
            mouse.tracking = true;
            mouse.wasTracking = false;
            break;
        }
        case SDL_MOUSEBUTTONUP:
        {
            mouse.tracking = false;
            mouse.wasTracking = true;
            break;
        }
        case SDL_MOUSEMOTION:
        {
            if (mouse.tracking)
            {
                // mouse/touch events are 4 times more sentitive in web build
                // this bit ensure the mouse/touch are even in native/web build
#if defined D_WEB_BUILD
                int coef = 1;
#else
                int coef = 4;
#endif

                glm::ivec2 newPosition(event.motion.x, event.motion.y);
                mouse.delta = (newPosition - mouse.position) * coef;
                mouse.position = newPosition;
            }
            break;
        }

        default:
            break;
    }
}

void State_AbstractSimulation::update(int deltaTime)
{
    float elapsedTime = float(deltaTime) / 1000.0f;

    auto& data = Data::get();
    auto& graphic = data.graphic;
    auto& camera = graphic.camera;

    { // events

        { // mouse/touch event(s)

            auto& mouse = data.inputs.mouse;

            if (mouse.tracking)
            {
                camera.rotations.theta -= float(mouse.delta.x) * 0.5f * elapsedTime;
                camera.rotations.phi += float(mouse.delta.y) * 0.5f * elapsedTime;
                mouse.delta = { 0, 0 };
            }

        } // mouse/touch event(s)

        { // keyboard event(s)

            auto& keys = data.inputs.keys;

            bool rotateLeft = (
                keys[SDLK_LEFT] ||  // ARROW
                keys[SDLK_q] ||     // QWERTY (UK-US keyboard layout)
                keys[SDLK_a]        // AZERTY (FR keyboard layout)
            );

            bool rotateRight = (
                keys[SDLK_RIGHT] || // ARROW
                keys[SDLK_d]
            );

            bool rotateUp = (
                keys[SDLK_UP] ||    // ARROW
                keys[SDLK_w] ||     // QWERTY (UK-US keyboard layout)
                keys[SDLK_z]        // AZERTY (FR keyboard layout)
            );

            bool rotateDown = (
                keys[SDLK_DOWN] ||  // ARROW
                keys[SDLK_s]
            );

            if (rotateLeft)
                camera.rotations.theta += 2.0f * elapsedTime;
            else if (rotateRight)
                camera.rotations.theta -= 2.0f * elapsedTime;

            if (rotateUp)
                camera.rotations.phi -= 1.0f * elapsedTime;
            else if (rotateDown)
                camera.rotations.phi += 1.0f * elapsedTime;

            // only pthread builds support this
            data.logic.isAccelerated = (keys[SDLK_SPACE]); // spacebar

        } // keyboard event(s)

    } // events
}


void State_AbstractSimulation::render(const SDL_Window& window)
{
    static_cast<void>(window); // <= unused

    Scene::renderAll();
}

void State_AbstractSimulation::resize(int width, int height)
{
    auto& data = Data::get();
    auto& graphic = data.graphic;

    graphic.camera.viewportSize = { width, height };

    {
        const auto& vSize = graphic.camera.viewportSize;

        struct Vertex
        {
            glm::vec3 position;
            glm::vec2 texCoord;
        };

        std::array<Vertex, 4> quadVertices{{
            { { vSize.x * 1.0f, vSize.y * 0.0f, 0.0f }, { 1.0f, 0.0f } },
            { { vSize.x * 0.0f, vSize.y * 0.0f, 0.0f }, { 0.0f, 0.0f } },
            { { vSize.x * 1.0f, vSize.y * 1.0f, 0.0f }, { 1.0f, 1.0f } },
            { { vSize.x * 0.0f, vSize.y * 1.0f, 0.0f }, { 0.0f, 1.0f } }
        }};

        std::array<int, 6> indices{{ 1,0,2,  1,3,2 }};

        std::vector<Vertex> vertices;
        vertices.reserve(indices.size()); // pre-allocate
        for (int index : indices)
            vertices.push_back(quadVertices[index]);

        graphic.geometries.hudPerspective.geometry.updateBuffer(0, vertices);
        graphic.geometries.hudPerspective.geometry.setPrimitiveCount(vertices.size());
    }

    {
        auto& hud = graphic.hudComponents;

        hud.frameBuffer.initialise();
        hud.frameBuffer.bind();

        hud.colorTexture.allocateBlank({ width, height }, false, false);
        hud.colorTexture.bind();
        hud.frameBuffer.attachColorTexture(hud.colorTexture);

        hud.depthRenderBuffer.allocateDepth({ width, height });
        hud.depthRenderBuffer.bind();
        hud.frameBuffer.attachDepthRenderBuffer(hud.depthRenderBuffer);

        hud.frameBuffer.executeCheck();
        FrameBuffer::unbind();
    }
}

void State_AbstractSimulation::visibility(bool visible)
{
    auto* stateManager = StateManager::get();
    StateManager::States currentState = stateManager->getState();

    if (currentState != StateManager::States::Paused && !visible)
    {
        Data::get().logic.state.previousState = currentState;
        stateManager->changeState(StateManager::States::Paused);
    }
}

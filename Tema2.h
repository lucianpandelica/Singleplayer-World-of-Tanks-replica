#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/lab_camera.h"
#include "lab_m1/Tema2/Battlefield.h"
#include "components/text_renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(implemented::CameraTema2* camera, Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void RenderSimpleMesh(implemented::CameraTema2* camera, Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, float hp);
        Mesh* CreateBuilding(const std::string& name, glm::vec3 color, glm::vec3 shadow_color);
        Mesh* CreateGround(const std::string& name, glm::vec3 color);
        void RenderTank(implemented::CameraTema2* camera, Tank* p);
        void RenderScene(implemented::CameraTema2* camera);

    protected:
        implemented::CameraTema2* camera;
        implemented::CameraTema2* camera_minimap;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        // perspective
        float fovy;
        float aspect;
        float near;
        float far;

        // ortho
        float left;
        float right;
        float bottom;
        float top;
        float z_near;
        float z_far;

        bool perspective;

        Battlefield b;
        glm::vec3 global_init_dir;
        glm::vec3 global_ref_dir;

        float total_deltaX;
        float total_deltaY;

        int points;
        gfxc::TextRenderer* points_number;

        // toon shader
        glm::vec3 lightPosition;
        glm::vec3 lightDirection;
        unsigned int materialShininess;
        float materialKd;
        float materialKs;

        int isSpotlight;
        float cut_off_angle;
        float x;
        float y;

        glm::vec3 secondLightPosition;
        int toon_shader;

        // mini-map
        ViewportArea miniViewportArea;

        // time
        double start;
    };
}   // namespace m1

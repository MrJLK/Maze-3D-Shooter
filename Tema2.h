#pragma once

#include "lab_m1/Tema2/CameraView.h"
#include "lab_m1/Tema2/transform3D.h"
#include "components/simple_scene.h"
#include "components/transform.h"
#include <fstream>

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();
        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1));
        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

    protected:
        glm::mat4 modelMatrix;
        glm::mat4 projectionMatrix;
        glm::vec3 lightPosition;
        glm::vec3 randomize_exit;
        glm::mat4 Labirint;
        implemented::Camera1* camera;
        implemented::Camera1* camera2;
        bool renderCameraTarget;
        bool renderOrtho;
        bool renderFOV;
        float translateX, translateY, translateZ;
        float newtranslateX, newtranslateY, newtranslateZ;
        float enemytranslateX, enemytranslateY, enemytranslateZ;
        float left, right, top, bottom, zNear, zFar;
        float bottomOrtho, topOrtho;
        float projectionMin, projectionMax, projectionAngle;
        bool projectionCheck;
        float leftOrtho, rightOrtho;
        unsigned int materialShininess;
        float materialKd;
        float materialKs;
        bool fireCheck;
        float projectileDistance;
        float projectileDistanceMax;
        bool positionCheck;
        bool FirstPerson;
        bool ThirdPerson;
        float PerspectiveCheck;
        float Timer;
        float Rotation;
        int matrice[10][10];
        float GameTimer;
        bool RotationPerspective;
        float checkMoves;
        float x, z;
    };
}
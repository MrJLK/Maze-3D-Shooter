#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/Tema2/transform3D.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;

Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    renderCameraTarget = false;

    camera = new implemented::Camera1();
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    projectionAngle = 60.0f;
    // TODO(student): After you implement the changing of the projection
    // parameters, remove hardcodings of these parameters
    projectionMatrix = glm::perspective(RADIANS(projectionAngle), window->props.aspectRatio, 0.01f, 200.0f);

    renderOrtho = false;
    renderFOV = false;
    projectionCheck = false;
    leftOrtho = 0.0f;
    rightOrtho = 10.0f;
    zNear = 0.01f;
    zFar = 200.0f;
    bottomOrtho = 0.0f;
    topOrtho = 10.0f;
    projectionMin = 0.01f;
    projectionMax = 200.0f;

    translateX = 0;
    translateY = 0;
    translateZ = 0;
}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    // Podea
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0.00f, 0.0f, 0.0f);
    modelMatrix *= transform3D::Scale(11.0f, 0.1f, 11.0f);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // Player
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    modelMatrix *= transform3D::Translate(0.04f, 0.5f, 0.0f);
    modelMatrix *= transform3D::Scale(0.28f, 0.6f, 0.3f);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);    // Picior stanga

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    modelMatrix *= transform3D::Translate(0.36f, 0.5f, 0.0f);
    modelMatrix *= transform3D::Scale(0.28f, 0.6f, 0.3f);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);    // Picior dreapta

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    modelMatrix *= transform3D::Translate(0.2f, 1.18f, 0.0f);
    modelMatrix *= transform3D::Scale(0.6f, 0.7f, 0.3f);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);    // Trunchi

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    modelMatrix *= transform3D::Translate(0.67f, 1.32f, 0.0f);
    modelMatrix *= transform3D::Scale(0.3f, 0.42f, 0.3f);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);    // Mana stanga - 1

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    modelMatrix *= transform3D::Translate(0.67f, 0.955f, 0.0f);
    modelMatrix *= transform3D::Scale(0.3f, 0.25f, 0.3f);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);    // Mana stanga - 2

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    modelMatrix *= transform3D::Translate(-0.27f, 1.32f, 0.0f);
    modelMatrix *= transform3D::Scale(0.3f, 0.42f, 0.3f);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);    // Mana dreapta - 1

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    modelMatrix *= transform3D::Translate(-0.27f, 0.955f, 0.0f);
    modelMatrix *= transform3D::Scale(0.3f, 0.25f, 0.3f);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);    // Mana dreapta - 2

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    modelMatrix *= transform3D::Translate(0.2f, 1.67f, 0.0f);
    modelMatrix *= transform3D::Scale(0.3f, 0.25f, 0.3f);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);    // Cap

    if (renderCameraTarget)
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
    }

    if (renderOrtho)
    {
        projectionMatrix = glm::ortho(leftOrtho, rightOrtho, bottomOrtho, topOrtho, zNear, zFar);
    }

    if (renderFOV)
    {
        projectionMatrix = glm::perspective(RADIANS(projectionAngle), window->props.aspectRatio, projectionMin, projectionMax);
    }
}


void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float cameraSpeed = 2.0f;

        if (window->KeyHold(GLFW_KEY_W)) {
            // TODO(student): Translate the camera forward
            camera->TranslateForward(deltaTime);
            translateZ -= 1.25 * deltaTime;
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            // TODO(student): Translate the camera to the left
            camera->TranslateRight(-deltaTime);
            translateX -= 1.25 * deltaTime;
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            // TODO(student): Translate the camera backward
            camera->TranslateForward(-deltaTime);
            translateZ += 1.25 * deltaTime;
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            // TODO(student): Translate the camera to the right
            camera->TranslateRight(deltaTime);
            translateX += 1.25 * deltaTime;
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            // TODO(student): Translate the camera downward
            camera->TranslateUpward(-deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            // TODO(student): Translate the camera upward
            camera->TranslateUpward(deltaTime);
        }
    }

    // TODO(student): Change projection parameters. Declare any extra
    // variables you might need in the class header. Inspect this file
    // for any hardcoded projection arguments (can you find any?) and
    // replace them with those extra variables.
    if (projectionCheck)
    {
        if (window->KeyHold(GLFW_KEY_F)) {
            projectionAngle -= deltaTime * 10;
            projectionMatrix = glm::perspective(RADIANS(projectionAngle), window->props.aspectRatio, projectionMin, projectionMax);
        }

        if (window->KeyHold(GLFW_KEY_G)) {
            projectionAngle += deltaTime * 10;
            projectionMatrix = glm::perspective(RADIANS(projectionAngle), window->props.aspectRatio, projectionMin, projectionMax);
        }
    }

    if (window->KeyHold(GLFW_KEY_V)) {
        rightOrtho += deltaTime * 10;
        topOrtho += deltaTime * 10;
        projectionMatrix = glm::ortho(leftOrtho, rightOrtho, bottomOrtho, topOrtho, zNear, zFar);
    }

    if (window->KeyHold(GLFW_KEY_B)) {
        rightOrtho -= deltaTime * 10;
        topOrtho -= deltaTime * 10;
        projectionMatrix = glm::ortho(leftOrtho, rightOrtho, bottomOrtho, topOrtho, zNear, zFar);
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }

    // TODO(student): Switch projections
    if (key == GLFW_KEY_O)
    {
        renderOrtho = true;
        projectionCheck = false;
        renderFOV = false;
    }

    if (key == GLFW_KEY_P)
    {
        renderFOV = true;
        projectionCheck = true;
        renderOrtho = false;
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensitivityOX = 0.001f;
        float sensitivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = false;
            // TODO(student): Rotate the camera in first-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateFirstPerson_OX(-sensitivityOX * deltaY);
            camera->RotateFirstPerson_OY(-sensitivityOY * deltaX);
        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCameraTarget = true;
            // TODO(student): Rotate the camera in third-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateThirdPerson_OX(-sensitivityOX * deltaY);
            camera->RotateThirdPerson_OY(-sensitivityOY * deltaX);
        }
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}

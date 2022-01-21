#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>
#include <ctime>

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
    // Camera Render First + Third Person
    {
    renderCameraTarget = false;
    camera = new implemented::Camera1();
    camera->Set(glm::vec3(0, 1.0f, 3.5f), glm::vec3(0, 0.5f, 0), glm::vec3(0, 1, 0));
    }

    // Box Mesh
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Sphere Mesh
    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Plane Mesh
    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Shaders
    {
        Shader* shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Light & material properties
    {
        lightPosition = glm::vec3(camera->GetTargetPosition().x, camera->GetTargetPosition().y, camera->GetTargetPosition().z);
        materialShininess = 10;
        materialKd = 0.5;
        materialKs = 0.5;
    }

    // FOV
    {
    projectionAngle = 60.0f;
    projectionMatrix = glm::perspective(RADIANS(projectionAngle), window->props.aspectRatio, 0.01f, 200.0f);
    }

    // Initialize maze
    {
        ifstream read_maze;
        read_maze.open("mazes\\Labirint_" + to_string(rand()%5+1) + ".txt");
        while (!read_maze.eof())
        {
            for (int i = 0; i < 10; i++)
                for (int j = 0; j < 10; j++)
                    read_maze >> matrice[i][j];
        }
        read_maze.close();
    }

    // Variables
    {
    projectileDistance = 0;
    projectileDistanceMax = 25.0f;
    translateX = 0;
    translateY = 0;
    translateZ = 0;
    newtranslateX = 0;
    newtranslateY = 0;
    newtranslateZ = 0;
    enemytranslateX = 0;
    enemytranslateZ = 0;
    Timer = 0;
    fireCheck = false;
    positionCheck = false;
    Rotation = 0;
    FirstPerson = false;
    ThirdPerson = true;
    RotationPerspective = false;
    PerspectiveCheck = 0;
    GameTimer = 2;
    checkMoves = 0;
    x = -2;
    z = -2;
    }
}


void Tema2::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    // GUI Timer Joc
    {
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y * (-0.1f), camera->GetTargetPosition().z);
        modelMatrix *= transform3D::RotateOY(Rotation);
        modelMatrix *= transform3D::Translate(-0.5f, 1.4f, 0.05f);
        modelMatrix *= transform3D::Scale(0.3f + GameTimer, 0.3f, 0.1f);
        GameTimer -= 0.001f;
        modelMatrix *= transform3D::Scale(0.3f, 0.3f, 0.1f);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.890, 0.945, 0.294)); // Timer joc
        
        if (GameTimer <= -0.3)
        {
            cout << "----------" << endl;
            cout << "Game Over - Out of time" << endl;
            Exit();
        }
    }

    // GUI Viata
    {
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y * (-0.1f), camera->GetTargetPosition().z);
        modelMatrix *= transform3D::RotateOY(Rotation);
        modelMatrix *= transform3D::Translate(1.03f, 1.2f, 0.05f);
        modelMatrix *= transform3D::Scale(0.3f, 0.09f, 0.1f);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.964, 0.156, 0.156)); // Viata1

        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y * (-0.1f), camera->GetTargetPosition().z);
        modelMatrix *= transform3D::RotateOY(Rotation);
        modelMatrix *= transform3D::Translate(0.68f, 1.2f, 0.05f);
        modelMatrix *= transform3D::Scale(0.3f, 0.09f, 0.1f);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.964, 0.156, 0.156)); // Viata2
    }

    // Ground
    {
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(0, 0.01f, 0);
        modelMatrix *= transform3D::Scale(3.0f, 0.1f, 3.0f);
        RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, glm::vec3(0.192, 0.866, 0.509));    // Pamant
    }

    // Lumina (corp fizic)
    /*
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, lightPosition);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);
    }
    */

    // First Person
    if (FirstPerson == true)
    {
        // Proiectil
        {
            if (fireCheck == true)
            {
                if (positionCheck == true)
                {
                    newtranslateX = camera->GetTargetPosition().x;
                    newtranslateY = camera->GetTargetPosition().y;
                    newtranslateZ = camera->GetTargetPosition().z;
                    positionCheck = false;
                    Timer = 0;
                }
                else
                {
                    modelMatrix = glm::mat4(1);
                    //modelMatrix *= transform3D::Translate(Timer * camera->forward, Timer * Rotation, Timer * Rotation);
                    modelMatrix *= translate(modelMatrix, camera->forward * Timer);
                    modelMatrix *= transform3D::Translate(newtranslateX, 0.45f, newtranslateZ);
                    modelMatrix *= transform3D::Scale(0.1f, 0.05f, 0.3f);
                    Timer += 0.05f;
                    projectileDistance += 0.05f;
                    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.984, 0.756, 0.756));
                }

                if (projectileDistance > projectileDistanceMax)
                {
                    Timer = 0;
                    projectileDistance = 0;
                    positionCheck = true;
                    fireCheck = false;
                }
            }
        }
    }

    // Third Person
    if (ThirdPerson == true)
    {
        // Player
        {
            modelMatrix = glm::mat4(1);
            modelMatrix *= transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y * (-0.1f), camera->GetTargetPosition().z);
            modelMatrix *= transform3D::RotateOY(Rotation);
            modelMatrix *= transform3D::Translate(-0.075f, 0.19f, 0.0f);
            modelMatrix *= transform3D::Scale(0.05f, 0.2f, 0.1f);
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.050, 0, 0.819));    // Picior stanga

            modelMatrix = glm::mat4(1);
            modelMatrix *= transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y * (-0.1f), camera->GetTargetPosition().z);
            modelMatrix *= transform3D::RotateOY(Rotation);
            modelMatrix *= transform3D::Translate(0.075f, 0.19f, 0.0f);
            modelMatrix *= transform3D::Scale(0.05f, 0.2f, 0.1f);
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.050, 0, 0.819));    // Picior dreapta

            modelMatrix = glm::mat4(1);
            modelMatrix *= transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y * (-0.1f), camera->GetTargetPosition().z);
            modelMatrix *= transform3D::RotateOY(Rotation);
            modelMatrix *= transform3D::Translate(0.0f, 0.45f, 0.0f);
            modelMatrix *= transform3D::Scale(0.2f, 0.3f, 0.1f);
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.956, 0.003, 0.015));    // Trunchi

            modelMatrix = glm::mat4(1);
            modelMatrix *= transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y * (-0.1f), camera->GetTargetPosition().z);
            modelMatrix *= transform3D::RotateOY(Rotation);
            modelMatrix *= transform3D::Translate(0.14f, 0.5f, 0.0f);
            modelMatrix *= transform3D::Scale(0.05f, 0.2f, 0.1f);
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.956, 0.003, 0.015));    // Mana stanga - 1

            modelMatrix = glm::mat4(1);
            modelMatrix *= transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y * (-0.1f), camera->GetTargetPosition().z);
            modelMatrix *= transform3D::RotateOY(Rotation);
            modelMatrix *= transform3D::Translate(0.14f, 0.34f, 0.0f);
            modelMatrix *= transform3D::Scale(0.05f, 0.07f, 0.1f);
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.964, 0.760, 0.415));    // Mana stanga - 2

            modelMatrix = glm::mat4(1);
            modelMatrix *= transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y * (-0.1f), camera->GetTargetPosition().z);
            modelMatrix *= transform3D::RotateOY(Rotation);
            modelMatrix *= transform3D::Translate(-0.14f, 0.5f, 0.0f);
            modelMatrix *= transform3D::Scale(0.05f, 0.2f, 0.1f);
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.956, 0.003, 0.015));    // Mana dreapta - 1

            modelMatrix = glm::mat4(1);
            modelMatrix *= transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y * (-0.1f), camera->GetTargetPosition().z);
            modelMatrix *= transform3D::RotateOY(Rotation);
            modelMatrix *= transform3D::Translate(-0.14f, 0.34f, 0.0f);
            modelMatrix *= transform3D::Scale(0.05f, 0.07f, 0.1f);
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.964, 0.760, 0.415));    // Mana dreapta - 2

            modelMatrix = glm::mat4(1);
            modelMatrix *= transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y * (-0.1f), camera->GetTargetPosition().z);
            modelMatrix *= transform3D::RotateOY(Rotation);
            modelMatrix *= transform3D::Translate(0.0f, 0.65f, 0.0f);
            modelMatrix *= transform3D::Scale(0.1f, 0.07f, 0.1f);
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.964, 0.760, 0.415));    // Cap

            modelMatrix = glm::mat4(1);
            modelMatrix *= transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y * (-0.1f), camera->GetTargetPosition().z);
            modelMatrix *= transform3D::RotateOY(Rotation);
            modelMatrix *= transform3D::Translate(0.0f, 0.7f, 0.0f);
            modelMatrix *= transform3D::Scale(0.03f, 0.05f, 0.1f);
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.890, 0.894, 0.047));    // Freza
        }
    }

    // Enemy Movement
    {
        if (checkMoves == 0)
        {
            enemytranslateX = enemytranslateX + 1.2 * deltaTimeSeconds;
            if (enemytranslateX > 0.5)
            {
                checkMoves = 1;
            }
        }
        if (checkMoves == 1)
        {
            enemytranslateZ = enemytranslateZ + 1.2 * deltaTimeSeconds;
            if (enemytranslateZ >= 0.5)
            {
                checkMoves = 2;
            }
        }
        if (checkMoves == 2)
        {
            enemytranslateX = enemytranslateX - 1.2 * deltaTimeSeconds;
            if (enemytranslateX < 0)
            {
                checkMoves = 3;
            }
        }
        if (checkMoves == 3)
        {
            enemytranslateZ = enemytranslateZ - 1.2 * deltaTimeSeconds;
            if (enemytranslateZ < 0)
            {
                checkMoves = 0;
            }
        }
    }

    // Maze
    {
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                if (matrice[i][j] == 1)
                {
                    modelMatrix = glm::mat4(1);
                    modelMatrix *= transform3D::Translate(i-3, 0.0f, j-3);
                    modelMatrix *= transform3D::Scale(1.0f, 2.0f, 1.0f);
                    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.866, 0.192, 0.807)); // Wall
                }

                if (matrice[i][j] == 0 && i == 1 && j == 1)
                {
                    modelMatrix = glm::mat4(1);
                    modelMatrix *= transform3D::Translate(enemytranslateX, 0.25f, enemytranslateZ);
                    modelMatrix *= transform3D::Translate(i-3.25, 0.25f, j-3.25);
                    modelMatrix *= transform3D::Scale(0.3f, 0.5f, 0.3f);
                    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.937, 0.729, 0.341)); // Inamic 1
                }

                if (matrice[i][j] == 0 && i == 5 && j == 7)
                {
                    modelMatrix = glm::mat4(1);
                    modelMatrix *= transform3D::Translate(enemytranslateX, 0.25f, enemytranslateZ);
                    modelMatrix *= transform3D::Translate(i - 3.25, 0.25f, j - 3.25);
                    modelMatrix *= transform3D::Scale(0.3f, 0.5f, 0.3f);
                    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.937, 0.729, 0.341)); // Inamic 2
                }
            }
        }
    }
}

void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);
    int locLightPos = glGetUniformLocation(shader->program, "light_position");
    glUniform3fv(locLightPos, 1, glm::value_ptr(lightPosition));
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int locEyePos = glGetUniformLocation(shader->program, "eye_position");
    glUniform3fv(locEyePos, 1, glm::value_ptr(eyePosition));
    int locMaterial = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(locMaterial, materialShininess);
    int locMaterialKd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(locMaterialKd, materialKd);
    int locMaterialKs = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(locMaterialKs, materialKs);
    int locObject = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(locObject, 1, glm::value_ptr(color));

    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
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

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float speed = 2;

    // Control player position using on W, A, S, D
    if (window->KeyHold(GLFW_KEY_W))
    {
        camera->MoveForward(deltaTime * speed);
    }
    if (window->KeyHold(GLFW_KEY_A))
    {
        camera->TranslateRight(-deltaTime * speed);
    }
    if (window->KeyHold(GLFW_KEY_S))
    {
        camera->MoveForward(-deltaTime * speed);
    }
    if (window->KeyHold(GLFW_KEY_D))
    {
        camera->TranslateRight(deltaTime * speed);
    }
}

void Tema2::OnKeyPress(int key, int mods)
{
    // Change camera perspective
    if (key == GLFW_KEY_LEFT_CONTROL)
    {
        if (ThirdPerson == true)
        {
            camera->MoveForward(2.5f);
            camera->TranslateUpward(-0.25f);
            FirstPerson = true;
            ThirdPerson = false;
            RotationPerspective = true;
        }
        else
        {
            camera->MoveForward(-2.5f);
            camera->TranslateUpward(0.25f);
            FirstPerson = false;
            ThirdPerson = true;
            RotationPerspective = false;
        }
    }
}

void Tema2::OnKeyRelease(int key, int mods)
{

}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensitivityOX = 0.001f;
        float sensitivityOY = 0.001f;

        if (RotationPerspective == false)
        {
            camera->RotateThirdPerson_OX(-sensitivityOX * deltaY);
            camera->RotateThirdPerson_OY(-sensitivityOY * deltaX);
        }
        else
        {
            camera->RotateFirstPerson_OX(-sensitivityOX * deltaY);
            camera->RotateFirstPerson_OY(-sensitivityOY * deltaX);
        }

        Rotation -= sensitivityOY * deltaX;
    }
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT))
    {
        if (Timer == 0 || Timer > projectileDistance)
        {
            fireCheck = true;
            positionCheck = true;
        }
    }
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{

}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema2::OnWindowResize(int width, int height)
{
}


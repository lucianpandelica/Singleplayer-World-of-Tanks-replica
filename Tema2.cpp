#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/Tema2/Helpers.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    renderCameraTarget = false;

    camera = new implemented::CameraTema2();
    camera->Set(glm::vec3(0, 1, 3.0), glm::vec3(0, 0.5, 0), glm::vec3(0, 1, 0), 1.0);

    camera_minimap = new implemented::CameraTema2();
    camera_minimap->Set(glm::vec3(0, 3, 0), glm::vec3(0, 0.5, 0), glm::vec3(0, 0, -1), 3.0);

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

    {
        Mesh* mesh = new Mesh("tank_wheels");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "tank_wheels_OG.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("tank_body");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "tank_body_OG.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("tank_turret");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "tank_turret_OG.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("tank_gun");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "tank_gun_OG.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = CreateBuilding("building", glm::vec3(0.1), glm::vec3(0.3));
        AddMeshToList(mesh);
    }

    {
        Mesh* mesh = CreateGround("ground", glm::vec3(0.30, 0.38, 0.40));
        AddMeshToList(mesh);
    }

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("TankShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Light & material properties
    {
        lightPosition = glm::vec3(0, 10, 0);
        lightDirection = glm::vec3(0, -1, 0);
        materialShininess = 30;
        materialKd = 0.5;
        materialKs = 0.5;
    }

    isSpotlight = 0;
    cut_off_angle = 30.0;
    x = 0.0;
    y = 0.0;

    secondLightPosition = glm::vec3(0, 2, 0);
    toon_shader = 1; 

    // perspective
    fovy = RADIANS(60);
    aspect = window->props.aspectRatio;
    near = 0.01f;
    far = 200.0f;

    // ortho
    left = -1;
    right = 1;
    bottom = -1;
    top = 1;
    z_near = near;
    z_far = far;

    perspective = true;

    projectionMatrix = glm::perspective(fovy, aspect, near, far);

    b = Battlefield();
    b.init(15, 4, 10, 25);

    global_init_dir = glm::vec3(0, 0, -1);
    global_ref_dir = glm::vec3(-1, 0, 0);

    total_deltaX = 0.0;
    total_deltaY = 0.0;

    points = 0;

    points_number = new gfxc::TextRenderer(".", 1280, 720);
    points_number->Load("assets\\fonts\\Hack-Bold.ttf", 30);

    GetCameraInput()->SetActive(false);
    window->DisablePointer();
    ToggleGroundPlane();

    // mini-map
    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 4.f, resolution.y / 4.f);

    // time
    start = glfwGetTime();
}

Mesh* Tema2::CreateGround(
    const std::string& name,
    glm::vec3 color) {

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(glm::vec3(-25, 0, -25), color),
        VertexFormat(glm::vec3(-25, 0, 25), color),
        VertexFormat(glm::vec3(25, 0, 25), color),
        VertexFormat(glm::vec3(25, 0, -25), color),
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2,
                                         0, 2, 3};

    square->SetDrawMode(GL_TRIANGLES);
    square->InitFromData(vertices, indices);
    return square;
}

Mesh* Tema2::CreateBuilding(
    const std::string& name,
    glm::vec3 color,
    glm::vec3 shadow_color)
{
    glm::vec3 corner = glm::vec3(0);

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(1, 0, 0), color),
        VertexFormat(corner + glm::vec3(1, 0, 1), color),
        VertexFormat(corner + glm::vec3(0, 0, 1), color),
        VertexFormat(corner + glm::vec3(0, 1, 0), color),
        VertexFormat(corner + glm::vec3(1, 1, 0), color),
        VertexFormat(corner + glm::vec3(1, 1, 1), color),
        VertexFormat(corner + glm::vec3(0, 1, 1), color),
        VertexFormat(corner, shadow_color),
        VertexFormat(corner + glm::vec3(1, 0, 0), shadow_color),
        VertexFormat(corner + glm::vec3(1, 0, 1), shadow_color),
        VertexFormat(corner + glm::vec3(0, 0, 1), shadow_color),
        VertexFormat(corner + glm::vec3(0, 1, 0), shadow_color),
        VertexFormat(corner + glm::vec3(1, 1, 0), shadow_color),
        VertexFormat(corner + glm::vec3(1, 1, 1), shadow_color),
        VertexFormat(corner + glm::vec3(0, 1, 1), shadow_color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 2, 1,
                                        0, 3, 2,
                                        4, 5, 6,
                                        4, 6, 7,
                                        0, 1, 5,
                                        0, 5, 4,
                                        2, 3, 6,
                                        3, 7, 6,
                                        9, 14, 13,
                                        9, 10, 14,
                                        11, 8, 12,
                                        11, 12, 15};

    square->SetDrawMode(GL_TRIANGLES);
    square->InitFromData(vertices, indices);
    return square;
}

void Tema2::RenderTank(implemented::CameraTema2* camera, Tank* p)
{
    float hp = p->hp;

    float tank_angle = Helpers::GetAngle(p->fwd_tank, global_init_dir, global_ref_dir);

    float turret_angle = Helpers::GetAngle(p->fwd_turret, p->init_dir, p->ref_dir);

    float gun_angle = Helpers::GetAngle(p->fwd_gun, p->init_dir, p->ref_dir);

    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, p->pos);
    modelMatrix = glm::rotate(modelMatrix, tank_angle, glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(p->scale));

    RenderSimpleMesh(camera, meshes["tank_wheels"], shaders["TankShader"], modelMatrix, p->wheels_color, hp);
    RenderSimpleMesh(camera, meshes["tank_body"], shaders["TankShader"], modelMatrix, p->body_color, hp);

    modelMatrix = glm::rotate(modelMatrix, turret_angle, glm::vec3(0, 1, 0));
    RenderSimpleMesh(camera, meshes["tank_turret"], shaders["TankShader"], modelMatrix, p->turret_color, hp);

    modelMatrix = glm::rotate(modelMatrix, gun_angle, glm::vec3(1, 0, 0));
    RenderSimpleMesh(camera, meshes["tank_gun"], shaders["TankShader"], modelMatrix, p->gun_color, hp);
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

void Tema2::RenderScene(implemented::CameraTema2* camera)
{
    RenderTank(camera, b.player);

    glm::mat4 modelMatrix = glm::mat4(1);
    RenderMesh(camera, meshes["ground"], shaders["VertexColor"], modelMatrix);

    for (Building* build : b.buildings) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(build->ox, 0, build->oz));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(build->W, build->H, build->L));
        RenderMesh(camera, meshes["building"], shaders["VertexColor"], modelMatrix);
    }

    for (Tank* enemy : b.enemies) {
        RenderTank(camera, enemy);
    }

    for (Bullet* blt : b.bullets) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, blt->pos);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(blt->scale));
        RenderMesh(camera, meshes["sphere"], shaders["VertexColor"], modelMatrix);
    }
}

void Tema2::Update(float deltaTimeSeconds)
{
    /* set camera */
    glm::vec3 camera_pos = glm::vec3(b.player->pos.x, b.player->center_y + 1.2, b.player->pos.z) -
        b.player->fwd_tank * static_cast<float>(3);
    glm::vec3 camera_center = glm::vec3(b.player->pos.x, b.player->center_y + 0.5, b.player->pos.z);
    camera->Set(camera_pos, camera_center, glm::vec3(0, 1, 0), 3.0);
    camera->RotateThirdPerson_OY(-0.001f * total_deltaX);
    
    b.CheckBltTankColl();
    b.CheckTanksColl();
    b.CheckBltBuildColl();
    b.CheckTankBuildColl();
    b.UpdateEnemiesPos(deltaTimeSeconds);
    b.EnemyFire(deltaTimeSeconds);
    b.CheckMapLimits();
    b.UpdateBattlefield(deltaTimeSeconds);

    RenderScene(camera);

    /* check time */
    double current_time = glfwGetTime();

    /* set text */
    {
        string str = "Points: " + to_string(b.points);
        points_number->RenderText(str, 10, 30, 1.0, glm::vec3(1));

        string str_hp = "HP: " + to_string(static_cast<int>(b.player->hp));
        if (b.player->hp > 50) {
            points_number->RenderText(str_hp, 1100, 30, 1.0, glm::vec3(0, 1, 0));
        } else if (b.player->hp <= 50 && b.player->hp >= 30) {
            points_number->RenderText(str_hp, 1100, 30, 1.0, glm::vec3(0, 1, 1));
        }
        else {
            points_number->RenderText(str_hp, 1100, 30, 1.0, glm::vec3(1, 0, 0));
        }
        
        string time = "Time: " + to_string(static_cast<int>(current_time));
        points_number->RenderText(time, 1100, 680, 1.0, glm::vec3(0));
    }

    if (b.player_dead == true) {
        exit(1);
    }

    if (current_time - start >= 60) {
        exit(1);
    }

    /* draw minimap */
    float x = b.player->pos.x;
    float z = b.player->pos.z;
    
    camera_minimap->Set(glm::vec3(x, 15.0, z), glm::vec3(x, 0, z), b.player->fwd_tank, 15.0);

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);

    RenderScene(camera_minimap);
    // DrawCoordinateSystem();
}


void Tema2::FrameEnd()
{
    // DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderMesh(implemented::CameraTema2* camera, Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
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

void Tema2::RenderSimpleMesh(implemented::CameraTema2* camera, Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, float hp)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();

    // Set shader uniforms for light properties
    int light_position = glGetUniformLocation(shader->program, "light_position");
    glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

    int second_light_position = glGetUniformLocation(shader->program, "second_light_position");
    glUniform3f(second_light_position, secondLightPosition.x, secondLightPosition.y, secondLightPosition.z);

    int light_direction = glGetUniformLocation(shader->program, "light_direction");
    glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

    int toon_shader_loc = glGetUniformLocation(shader->program, "toon_shader");
    glUniform1i(toon_shader_loc, toon_shader);

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = camera->position;
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    // Set material property uniforms (shininess, kd, ks, object color) 
    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, materialShininess);

    int material_kd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd, materialKd);

    int material_ks = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks, materialKs);

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);

    int light_type = glGetUniformLocation(shader->program, "light_type");
    glUniform1i(light_type, isSpotlight);

    float cut_angle = glGetUniformLocation(shader->program, "cut_angle");
    glUniform1f(cut_angle, cut_off_angle);

    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(color));

    // Bind HP
    int loc_hp = glGetUniformLocation(shader->program, "HP");
    glUniform1f(loc_hp, hp);

    mesh->Render();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float speed_rot = 50.0;
    float speed_move = 5.0;

    // tank
    Tank* p = b.player;

    if (window->KeyHold(GLFW_KEY_W)) {
        p->pos += p->fwd_tank * speed_move * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        p->pos -= p->fwd_tank * speed_move * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        p->fwd_tank = Helpers::RotateVec(p->fwd_tank, - speed_rot, deltaTime, glm::vec3(0, 1, 0));
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        p->fwd_tank = Helpers::RotateVec(p->fwd_tank, speed_rot, deltaTime, glm::vec3(0, 1, 0));
    }


    if (window->KeyHold(GLFW_KEY_K) && perspective == true) {
        fovy -= deltaTime;
        projectionMatrix = glm::perspective(fovy, aspect, near, far);
    }

    if (window->KeyHold(GLFW_KEY_L) && perspective == true) {
        fovy += deltaTime;
        projectionMatrix = glm::perspective(fovy, aspect, near, far);
    }

    if (window->KeyHold(GLFW_KEY_V) && perspective == false) {
        left += deltaTime / 2.0;
        right -= deltaTime / 2.0;
        projectionMatrix = glm::ortho(left, right, bottom, top, z_near, z_far);
    }

    if (window->KeyHold(GLFW_KEY_B) && perspective == false) {
        left -= deltaTime / 2.0;
        right += deltaTime / 2.0;
        projectionMatrix = glm::ortho(left, right, bottom, top, z_near, z_far);
    }

    if (window->KeyHold(GLFW_KEY_N) && perspective == false) {
        bottom += deltaTime / 2.0;
        top -= deltaTime / 2.0;
        projectionMatrix = glm::ortho(left, right, bottom, top, z_near, z_far);
    }

    if (window->KeyHold(GLFW_KEY_M) && perspective == false) {
        bottom -= deltaTime / 2.0;
        top += deltaTime / 2.0;
        projectionMatrix = glm::ortho(left, right, bottom, top, z_near, z_far);
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
    if (key == GLFW_KEY_O) {
        projectionMatrix = glm::ortho(left, right, bottom, top, z_near, z_far);
        perspective = false;
    }

    if (key == GLFW_KEY_P) {
        projectionMatrix = glm::perspective(fovy, aspect, near, far);
        perspective = true;
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    float sensivityOX = 0.001f;
    float sensivityOY = 0.001f;

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        total_deltaY += deltaY;
        total_deltaX += deltaX;
    }
    else {
        sensivityOX = 0.25;
        sensivityOY = 0.1;

        Tank* p = b.player;
        p->fwd_turret = Helpers::RotateVec(p->fwd_turret, sensivityOX, deltaX, glm::vec3(0, 1, 0));
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (button == GLFW_MOUSE_BUTTON_2) {
        b.Fire(b.player);
    }
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
    points_number = new gfxc::TextRenderer(".", width, height);
    points_number->Load("assets\\fonts\\Hack-Bold.ttf", 30);
}

#include <format>
#include <vector>
#include <random>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GLSL/Project.hpp"

static void GLFW_ErrorCallback(int error_code, const char *description)
{
    std::cerr << std::format("GLFW error: {:#x} - {}", error_code, description) << std::endl;
}

static void GLFW_FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    glfwSetErrorCallback(GLFW_ErrorCallback);

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1000, 1000, "Physarum Simulation", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, GLFW_FramebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cerr << "GLAD failed to load OpenGL loader." << std::endl;
        glfwTerminate();
        return 3;
    }

    // ImGui

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle &style = ImGui::GetStyle();

    style.Colors[ImGuiCol_WindowBg].w = 0.75f;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    // ImGui End

    GLint max_compute_work_group_x, max_compute_work_group_y, max_compute_work_group_z;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_compute_work_group_x);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &max_compute_work_group_y);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &max_compute_work_group_z);

    std::cout << max_compute_work_group_x << '\n' << max_compute_work_group_y << '\n' << max_compute_work_group_z << "\n\n";

    GLint max_ssbo_block_size;
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &max_ssbo_block_size);
    std::cout << "Maximum SSBO block size: " << max_ssbo_block_size << '\n';

    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
    // TEXTURE SIZE
    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////

    const GLuint TEXTURE_WIDTH = 2048, TEXTURE_HEIGHT = 2048;
    const GLuint NUM_AGENTS    = 20000;

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    GLSL::Project project(
            "../../shaders",
            {
                    {"Vertex.glsl",   GLSL::ShaderType::Vertex},
                    {"Fragment.glsl", GLSL::ShaderType::Fragment},

                    {"Common.glsl",   GLSL::ShaderType::Header},
                    {"Random.glsl",   GLSL::ShaderType::Header},
                    {"Agent.glsl",    GLSL::ShaderType::Header},
                    {"Sensor.glsl",   GLSL::ShaderType::Header},

                    {"Navigate.glsl", GLSL::ShaderType::Compute},
                    {"Move.glsl",     GLSL::ShaderType::Compute},
                    {"Diffuse.glsl",  GLSL::ShaderType::Compute},
                    {"Decay.glsl",    GLSL::ShaderType::Compute},
            }
    );

    project.Preprocess();

    GLSL::Program draw_step, move_step, navigate_step, decay_step, diffuse_step;

    project.Build(draw_step, {"Vertex.glsl", "Fragment.glsl"});
    project.Build(move_step, {"Move.glsl"});
    project.Build(navigate_step, {"Navigate.glsl"});
    project.Build(diffuse_step, {"Diffuse.glsl"});
    project.Build(decay_step, {"Decay.glsl"});

    uint32_t vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    const float TEXTURE_RENDER_COORDS = 1.0f;

    const GLfloat vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, TEXTURE_RENDER_COORDS,
            1.0f, 1.0f, 0.0f, TEXTURE_RENDER_COORDS, TEXTURE_RENDER_COORDS,
            1.0f, -1.0f, 0.0f, TEXTURE_RENDER_COORDS, 0.0f,
    };

    const GLuint indices[] = {
            0, 1, 2,
            2, 3, 0
    };

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // COMPUTE SHADER SSBO

    struct Vector2
    {
        GLfloat x, y;
    };

    struct alignas(16) Sensor
    {
        GLfloat angle;
        GLfloat extent;
    };

    struct alignas(32) Agent
    {
        Vector2 position;
        GLfloat heading;
        GLfloat cooldown;
        GLint   sensor_offset;
        GLint   sensor_number;
    };

    std::vector<Agent>  agents;
    std::vector<Sensor> sensors;

    std::random_device                      rd;
    std::mt19937                            mt(rd());
    std::uniform_int_distribution<GLint>    random_position_x(0, TEXTURE_WIDTH - 1);
    std::uniform_int_distribution<GLint>    random_position_y(0, TEXTURE_HEIGHT - 1);
    std::uniform_real_distribution<GLfloat> random_angle(-M_PI_2, M_PI_2);
    std::uniform_real_distribution<GLfloat> random_extent(2.0f, 10.0f);
    std::uniform_int_distribution<GLint>    random_sensor_number(3, 10);

    GLint sensor_offset = 0;

    for (size_t i = 0; i < NUM_AGENTS; ++i)
    {
        Agent agent{};

        agent.position.x = (GLfloat) random_position_x(mt);
        agent.position.y = (GLfloat) random_position_y(mt);
        agent.heading    = random_angle(mt);

        agent.cooldown = 0.0f;

        agent.sensor_offset = sensor_offset;
        agent.sensor_number = random_sensor_number(mt);

        for (size_t j = 0; j < agent.sensor_number; ++j)
        {
            sensors.push_back((Sensor) {.angle  = random_angle(mt), .extent = random_extent(mt)});
        }

        agents.push_back(agent);

        sensor_offset += agent.sensor_number;
    }

    GLuint SSBOs[2];
    glGenBuffers(2, SSBOs);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[0]);
    {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizeiptr) (agents.size() * sizeof(Agent)), agents.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSBOs[0]);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[1]);
    {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizeiptr) (sensors.size() * sizeof(Sensor)), sensors.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SSBOs[1]);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    navigate_step.Use();
    navigate_step.SetUniform("u_Resolution", TEXTURE_WIDTH, TEXTURE_HEIGHT);
    navigate_step.SetUniform("u_SensorAngle", M_PI / 4.0f);
    navigate_step.SetUniform("u_RotateAngle", M_PI / 4.0f);
    navigate_step.SetUniform("u_RSensorExtent", 30.0f);
    navigate_step.SetUniform("u_CSensorExtent", 50.0f);
    navigate_step.SetUniform("u_LSensorExtent", 30.0f);

    move_step.Use();
    move_step.SetUniform("u_Resolution", TEXTURE_WIDTH, TEXTURE_HEIGHT);
    move_step.SetUniform("u_Speed", 5.0f);

    diffuse_step.Use();
    diffuse_step.SetUniform("u_Resolution", TEXTURE_WIDTH, TEXTURE_HEIGHT);

    decay_step.Use();
    decay_step.SetUniform("u_Resolution", TEXTURE_WIDTH, TEXTURE_HEIGHT);
    decay_step.SetUniform("u_Decay", 0.995f);

    GLSL::Program::Unuse();

    float last_time = 0.0f, delta_time;
    while (!glfwWindowShouldClose(window))
    {
        auto current_time = (float) glfwGetTime();
        delta_time = current_time - last_time;
        last_time  = current_time;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("Hello!"))
        {
            ImGui::Text("Hello, World!");
            ImGui::End();
        }

        ImGui::Render();

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        navigate_step.Use();
        glDispatchCompute(NUM_AGENTS, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        move_step.Use();
        glDispatchCompute(NUM_AGENTS, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        diffuse_step.Use();
        glDispatchCompute(TEXTURE_WIDTH, TEXTURE_HEIGHT, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        decay_step.Use();
        glDispatchCompute(TEXTURE_WIDTH, TEXTURE_HEIGHT, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        draw_step.Use();
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}

#include <iostream>
#include <format>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <random>
#include <thread>

#include "FileReader.h"

static void GLFW_ErrorCallback(int error_code, const char *description)
{
    std::cerr << std::format("GLFW error: {:#x} - {}", error_code, description) << std::endl;
}

static void GLFW_FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

enum ShaderType : GLenum
{
    Vertex   = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    Compute  = GL_COMPUTE_SHADER
};

class Shader
{
public:

    explicit Shader(ShaderType type)
    {
        m_ID = glCreateShader(type);
    };

    ~Shader()
    {
        glDeleteShader(m_ID);
    }

public:

    void Compile(const std::string &path) const
    {
        std::ifstream file(path);
        std::string   source(std::istreambuf_iterator<char>(file), {});

        const char *source_c_str = source.c_str();
        glShaderSource(m_ID, 1, &source_c_str, nullptr);

        glCompileShader(m_ID);

        GLint compiled;
        glGetShaderiv(m_ID, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLchar message[1024];
            glGetShaderInfoLog(m_ID, 1024, nullptr, message);
            std::cerr << "[ERROR] " << path << " | Compilation failed: " << message << '\n';
        }
    }

    [[nodiscard]] GLuint GetID() const
    {
        return m_ID;
    }

private:
    GLuint m_ID;
};

class ShaderProgram
{
public:

    explicit ShaderProgram(const std::initializer_list<Shader> &shaders)
    {
        m_ID = glCreateProgram();

        for (const auto &shader: shaders)
        {
            glAttachShader(m_ID, shader.GetID());
        }

        glLinkProgram(m_ID);

        GLint program_linked;
        glGetProgramiv(m_ID, GL_LINK_STATUS, &program_linked);

        if (!program_linked)
        {
            GLchar message[1024];
            glGetProgramInfoLog(m_ID, 1024, nullptr, message);
            std::cerr << "Error linking program: " << message << '\n';
        }
    }

    ~ShaderProgram()
    {
        glDeleteProgram(m_ID);
    }

public:

    void Use() const
    {
        glUseProgram(m_ID);
    }

private:
    GLuint m_ID;
};

void CreateShader(GLuint *program, GLuint *comp_prog_agents, GLuint *comp_prog_decay, GLuint *comp_prog_diff)
{
    Shader vertex(ShaderType::Vertex);
    Shader fragment(ShaderType::Fragment);
    Shader compute_agents(ShaderType::Compute);
    Shader compute_decay(ShaderType::Compute);
    Shader compute_diff(ShaderType::Compute);

    vertex.Compile("../../shaders/Vertex.vert");
    fragment.Compile("../../shaders/Fragment.frag");
    compute_agents.Compile("../../shaders/Agents.comp");
    compute_decay.Compile("../../shaders/Decay.comp");
    compute_diff.Compile("../../shaders/Diffuse.comp");

    *program          = glCreateProgram();
    *comp_prog_agents = glCreateProgram();
    *comp_prog_decay  = glCreateProgram();
    *comp_prog_diff   = glCreateProgram();

    glAttachShader(*program, vertex.GetID());
    glAttachShader(*program, fragment.GetID());
    glAttachShader(*comp_prog_agents, compute_agents.GetID());
    glAttachShader(*comp_prog_decay, compute_decay.GetID());
    glAttachShader(*comp_prog_diff, compute_diff.GetID());

    glLinkProgram(*program);

    GLint linked;
    glGetProgramiv(*program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLchar message[1024];
        glGetProgramInfoLog(*program, 1024, nullptr, message);
        std::cerr << "Failed to link program: " << message << "\n";
    }

    glLinkProgram(*comp_prog_agents);

    glGetProgramiv(*comp_prog_agents, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLchar message[1024];
        glGetProgramInfoLog(*comp_prog_agents, 1024, nullptr, message);
        std::cerr << "Failed to link program: " << message << "\n";
    }

    glLinkProgram(*comp_prog_decay);

    glGetProgramiv(*comp_prog_decay, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLchar message[1024];
        glGetProgramInfoLog(*comp_prog_decay, 1024, nullptr, message);
        std::cerr << "Failed to link program: " << message << "\n";
    }

    glLinkProgram(*comp_prog_diff);

    glGetProgramiv(*comp_prog_diff, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLchar message[1024];
        glGetProgramInfoLog(*comp_prog_diff, 1024, nullptr, message);
        std::cerr << "Failed to link program: " << message << "\n";
    }
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

    GLuint prog, comp_prog_agents, comp_prog_decay, comp_prog_diff;
    CreateShader(&prog, &comp_prog_agents, &comp_prog_decay, &comp_prog_diff);

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

    struct alignas(8) Vector2
    {
        GLfloat x, y;
    };

    struct alignas(16) Vector3
    {
        GLfloat x, y, z;
    };

    struct alignas(16) Sensor
    {
        GLfloat angle;
        GLfloat extent;
    };

    struct alignas(64) Agent
    {
        Vector2 position;
        GLfloat heading;

        GLfloat deposit;
        Vector3 pheromone;

        GLfloat speed;

        GLfloat cooldown;

        GLint sensor_offset;
        GLint sensor_number;
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

        if (i % 2 == 0)
        {
            agent.deposit   = 1.0f;
            agent.pheromone = {1.0f, 0.5f, 0.0f};
        }
        else
        {
            agent.deposit   = 1.0f;
            agent.pheromone = {0.0f, 0.5f, 1.0f};
        }

        agent.speed    = 5.0f;
        agent.cooldown = 0.0f;

        agent.sensor_offset = sensor_offset;
        agent.sensor_number = random_sensor_number(mt);

        agents.push_back(agent);

        sensor_offset += agent.sensor_number;
    }

    sensors.push_back((Sensor) {.angle  = -M_PI / 6.0f, .extent = 40.0f});
    sensors.push_back((Sensor) {.angle  = 0.0f, .extent = 100.0f});
    sensors.push_back((Sensor) {.angle  = M_PI / 6.0f, .extent = 40.0f});

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

    glUseProgram(comp_prog_agents);
    {
        glUniform2i(glGetUniformLocation(comp_prog_agents, "u_Resolution"), TEXTURE_WIDTH, TEXTURE_HEIGHT);
    }
    glUseProgram(comp_prog_decay);
    {
        glUniform2i(glGetUniformLocation(comp_prog_decay, "u_Resolution"), TEXTURE_WIDTH, TEXTURE_HEIGHT);
    }
    glUseProgram(0);

    // Shader<Vertex, Fragment> main_shader("../../Vertex.vert", "../../Fragment.frag");
    // Shader main_shader({Vertex, "../../Vertex.vert"}, {Fragment, "../../Fragment.frag"});

    // Compute decay("../../Decay.comp");

    float last_time = 0.0f, delta_time;
    while (!glfwWindowShouldClose(window))
    {
        auto current_time = (float) glfwGetTime();
        delta_time = current_time - last_time;
        last_time  = current_time;

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(comp_prog_decay);
        glDispatchCompute(TEXTURE_WIDTH, TEXTURE_HEIGHT, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        //auto t1 = std::chrono::high_resolution_clock::now();
        glUseProgram(comp_prog_agents);
        glUniform1f(glGetUniformLocation(comp_prog_agents, "u_DeltaTime"), delta_time);
        glUniform1f(glGetUniformLocation(comp_prog_agents, "u_Time"), current_time);
        glDispatchCompute(TEXTURE_WIDTH / 16, TEXTURE_HEIGHT / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        //auto t2 = std::chrono::high_resolution_clock::now();

        //std::chrono::duration<double, std::milli> t = t2 - t1;
        //std::cout << t.count() << "ms\n";

        glUseProgram(comp_prog_diff);
        glDispatchCompute(TEXTURE_WIDTH, TEXTURE_HEIGHT, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(prog); // main_shader.Use();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

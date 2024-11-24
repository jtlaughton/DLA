#include "../headers/renderer.h"

Renderer::Renderer() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "DLA Program", NULL, NULL);
    VBO = 0;
    VAO = 0;
    EBO = 0;
    texture1 = 0;
    shader = nullptr;
}

int Renderer::Init(){
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    shader = new Shader("../src/shaders/vertex.glsl", "../src/shaders/Fragment.glsl");

    // generate buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // set up VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // set up EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // setup texture
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // read texture data
    const std::string dataStr = generator.GetSharpByteStream();
    const GLchar* data = dataStr.c_str();

    // set texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, generator.width, generator.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    shader->use();
    shader->setInt("texture1", 0);

    start = std::chrono::high_resolution_clock::now();

    return 0;
}

bool Renderer::Running() const {
    return !glfwWindowShouldClose(window);
}


void Renderer::Update(){
    //input
    processInput(window);

    Draw();

    // check and call events and swap the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Renderer::Draw() {
    //render commands
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!iterationsFinished) {
        iterationsFinished = true;

        start = std::chrono::high_resolution_clock::now();
        generator.RunSequence();
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << duration.count() << "ms" << std::endl;

        // read texture data
        const std::string dataStr = generator.GetSharpByteStream();
        const GLchar* data = dataStr.c_str();

        std::cout << generator.blurredWidth << std::endl;

        // set texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, generator.width, generator.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // // update texture
    // if (!iterationsFinished) {
    //     iterationsFinished = !generator.RunIteration();
    //
    //
    // }
    // else if (!scalingApplied){
    //     scalingApplied = true;
    //
    //     generator.BiLinearInterpolationBy2();
    //     generator.Blur();
    //
    //     generator.SharpUpscale();
    //
    //     generator.CombineBlurredAndSharp();
    //
    //     // read texture data
    //     const std::string dataStr2 = generator.GetBlurredByteStream();
    //     const GLchar* data2 = dataStr2.c_str();
    //
    //     // set texture data
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, generator.blurredWidth, generator.blurredHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
    //     glGenerateMipmap(GL_TEXTURE_2D);
    //
    //     // for (int i = 0; i < 3; i++) {
    //     //     generator.BiLinearInterpolationBy2();
    //     //     generator.Blur();
    //     // }
    //
    //     // for (int i = 0; i < 2; i++) {
    //     //     generator.SharpUpscale();
    //     // }
    //
    //
    // }
    // else if (currentIteration != iterations - 1) {
    //     currentIteration++;
    //     iterationsFinished = false;
    //     auto stop = std::chrono::high_resolution_clock::now();
    //     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    //     timeStamps.push_back(duration);
    //
    //     generator.Reset();
    //
    //     start = std::chrono::high_resolution_clock::now();
    // }
    // else if (!printed) {
    //     printed = true;
    //
    //     int sum = 0;
    //     for (auto t: timeStamps) {
    //         sum += t.count();
    //     }
    //
    //     const double average = static_cast<double>(sum) / timeStamps.size();
    //
    //     std::cout << "Average after " << iterations << " iterations: " << average << std::endl;
    // }

    //bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // use shader program
    shader->use();
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::End() const {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    delete shader;
}





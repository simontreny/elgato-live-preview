#include <cstdlib>
#include <cstdio>
#include "gl_includes.h"
#include "frame_client.h"
#include "frame_queue.h"
#include "frame_renderer.h"
#include "post_process.h"

static void onGlfwError(int error, const char* description);
static void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

int main(int argc, char* argv[]) {
    glfwSetErrorCallback(onGlfwError);
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Elgato Live Preview", NULL, NULL);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetKeyCallback(window, onKeyEvent);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewInit();

    FrameQueue frames(1);
    FrameRenderer frameRenderer;

    FrameClient frameClient;
    frameClient.setFrameReceivedHandler([&frames](const Frame& frame) { frames.enqueue(frame); });
    frameClient.start();

    PostProcess sharpenFilter(1920, 1080, Shader::fromFile(GL_FRAGMENT_SHADER, "shaders/sharpen.glsl"));

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Frame frame;
        if (frames.tryDequeue(&frame, 50)) {
            sharpenFilter.bind();
            frameRenderer.render(frame, (float)width / height);
            sharpenFilter.draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    frameClient.stop();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

static void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void onGlfwError(int error, const char* description) {
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
    exit(EXIT_FAILURE);
}

#include <cstdlib>
#include <cstdio>
#include <GLFW/glfw3.h>
#include "frame_client.h"
#include "frame_queue.h"
#include "frame_renderer.h"

static void onGlfwError(int error, const char* description);

int main(int argc, char* argv[]) {
    glfwSetErrorCallback(onGlfwError);
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Elgato Live Preview", NULL, NULL);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    FrameQueue frames(1);
    FrameRenderer frameRenderer;

    FrameClient frameClient;
    frameClient.setFrameReceivedHandler([&frames](const Frame& frame) { frames.enqueue(frame); });
    frameClient.start();

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Frame frame;
        if (frames.tryDequeue(&frame, 50)) {
            frameRenderer.render(frame, (float)width / height);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    frameClient.stop();

    glfwTerminate();
    return 0;
}

static void onGlfwError(int error, const char* description) {
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
    exit(EXIT_FAILURE);
}

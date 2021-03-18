#include <GLFW/glfw3.h>

int main(void)
{
    GLFWwindow* window;

    /* 初始化glfw */
    if (!glfwInit())
        return -1;

    /* 创建一个Window 和 OpenGL上下文 */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        //创建失败就退出
        glfwTerminate();
        return -1;
    }

    /* 激活上面创建的OpenGL上下文 */
    glfwMakeContextCurrent(window);

    /* 进入游戏引擎主循环 */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* 处理鼠标 键盘事件 */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
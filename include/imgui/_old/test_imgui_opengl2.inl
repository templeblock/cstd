// ImGui GLFW binding with OpenGL
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.
// If your context is GL3/GL3 then prefer using the code in opengl3_example.
// You *might* use this code with a GL3/GL4 context but make sure you disable the programmable pipeline by calling "glUseProgram(0)" before ImGui_Render().
// We cannot do that from GL2 code because the function doesn't exist.
// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui_Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui
// ImGui GLFW binding with OpenGL
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.
// If your context is GL3/GL3 then prefer using the code in opengl3_example.
// You *might* use this code with a GL3/GL4 context but make sure you disable the programmable pipeline by calling "glUseProgram(0)" before ImGui_Render().
// We cannot do that from GL2 code because the function doesn't exist.
// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui_Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui
#include <gl/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#pragma comment(lib, "glut.lib")
#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "glew32.lib")
// GLFW
#include "gl/GLFW/glfw3.h"
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
//#include <GLFW/glfw3native.h>
#include "gl/GLFW/glfw3native.h"
#endif
#include "gl/glfw/libglfw3.inl"
// Data
static GLFWwindow* g_Window = NULL;
static double g_Time = 0.0f;
static bool g_MousePressed[3] = { false, false, false };
static float g_MouseWheel = 0.0f;
static GLuint g_FontTexture = 0;
// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
void ImGui_ImplGlfw_RenderDrawLists(ImDrawData* draw_data)
{
  // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
  ImGuiIO* io = ImGui_GetIO();
  int fb_width = (int)(io->DisplaySize.x * io->DisplayFramebufferScale.x);
  int fb_height = (int)(io->DisplaySize.y * io->DisplayFramebufferScale.y);
  if (fb_width == 0 || fb_height == 0) {
    return;
  }
  //draw_data->ScaleClipRects(io->DisplayFramebufferScale);
  // We are using the OpenGL fixed pipeline to make the example code simpler to read!
  // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.
  GLint last_texture;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  GLint last_viewport[4];
  glGetIntegerv(GL_VIEWPORT, last_viewport);
  GLint last_scissor_box[4];
  glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
  glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnable(GL_TEXTURE_2D);
  //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context
  // Setup viewport, orthographic projection matrix
  glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0f, io->DisplaySize.x, io->DisplaySize.y, 0.0f, -1.0f, +1.0f);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  // Render command lists
#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
  for (int n = 0; n < draw_data->CmdListsCount; n++) {
    const ImDrawList* cmd_list = draw_data->CmdLists[n];
    const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
    const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
    glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)((char*)vtx_buffer + OFFSETOF(ImDrawVert, pos)));
    glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)((char*)vtx_buffer + OFFSETOF(ImDrawVert, uv)));
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)((char*)vtx_buffer + OFFSETOF(ImDrawVert, col)));
    for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
      const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
      if (pcmd->UserCallback) {
        pcmd->UserCallback(cmd_list, pcmd);
      }
      else {
        glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
        glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
        glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
      }
      idx_buffer += pcmd->ElemCount;
    }
  }
#undef OFFSETOF
  // Restore modified state
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glBindTexture(GL_TEXTURE_2D, (GLuint)last_texture);
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPopAttrib();
  glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
  glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}
static const char* ImGui_ImplGlfw_GetClipboardText()
{
  return glfwGetClipboardString(g_Window);
}
static void ImGui_ImplGlfw_SetClipboardText(const char* text)
{
  glfwSetClipboardString(g_Window, text);
}
void ImGui_ImplGlfw_MouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/)
{
  if (action == GLFW_PRESS && button >= 0 && button < 3) {
    g_MousePressed[button] = true;
  }
}
void ImGui_ImplGlfw_ScrollCallback(GLFWwindow*, double /*xoffset*/, double yoffset)
{
  g_MouseWheel += (float)yoffset; // Use fractional mouse wheel, 1.0 unit 5 lines.
}
void ImGui_ImplGlFw_KeyCallback(GLFWwindow*, int key, int, int action, int mods)
{
  ImGuiIO* io = ImGui_GetIO();
  if (action == GLFW_PRESS) {
    io->KeysDown[key] = true;
  }
  if (action == GLFW_RELEASE) {
    io->KeysDown[key] = false;
  }
  (void)mods; // Modifiers are not reliable across systems
  io->KeyCtrl = io->KeysDown[GLFW_KEY_LEFT_CONTROL] || io->KeysDown[GLFW_KEY_RIGHT_CONTROL];
  io->KeyShift = io->KeysDown[GLFW_KEY_LEFT_SHIFT] || io->KeysDown[GLFW_KEY_RIGHT_SHIFT];
  io->KeyAlt = io->KeysDown[GLFW_KEY_LEFT_ALT] || io->KeysDown[GLFW_KEY_RIGHT_ALT];
  io->KeySuper = io->KeysDown[GLFW_KEY_LEFT_SUPER] || io->KeysDown[GLFW_KEY_RIGHT_SUPER];
}
void ImGui_ImplGlfw_CharCallback(GLFWwindow*, unsigned int c)
{
  ImGuiIO* io = ImGui_GetIO();
  if (c > 0 && c < 0x10000) {
    io->AddInputCharacter((unsigned short)c);
  }
}
bool ImGui_ImplGlfw_CreateDeviceObjects()
{
  // Build texture atlas
  ImGuiIO* io = ImGui_GetIO();
  unsigned char* pixels;
  int width, height;
  io->Fonts->GetTexDataAsRGBA32(&pixels, &width, &height); // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
  // Upload texture to graphics system
  GLint last_texture;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  glGenTextures(1, &g_FontTexture);
  glBindTexture(GL_TEXTURE_2D, g_FontTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  // Store our identifier
  io->Fonts->TexID = (void*)(intptr_t)g_FontTexture;
  // Restore state
  glBindTexture(GL_TEXTURE_2D, last_texture);
  return true;
}
void ImGui_ImplGlfw_InvalidateDeviceObjects()
{
  if (g_FontTexture) {
    glDeleteTextures(1, &g_FontTexture);
    ImGui_GetIO()->Fonts->TexID = 0;
    g_FontTexture = 0;
  }
}
bool ImGui_ImplGlfw_Init(GLFWwindow* window, bool install_callbacks)
{
  g_Window = window;
  ImGuiIO* io = ImGui_GetIO();
  io->KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB; // Keyboard mapping. ImGui will use those indices to peek into the io->KeyDown[] array.
  io->KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
  io->KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
  io->KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
  io->KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
  io->KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
  io->KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
  io->KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
  io->KeyMap[ImGuiKey_End] = GLFW_KEY_END;
  io->KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
  io->KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
  io->KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
  io->KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
  io->KeyMap[ImGuiKey_A] = GLFW_KEY_A;
  io->KeyMap[ImGuiKey_C] = GLFW_KEY_C;
  io->KeyMap[ImGuiKey_V] = GLFW_KEY_V;
  io->KeyMap[ImGuiKey_X] = GLFW_KEY_X;
  io->KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
  io->KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
  io->RenderDrawListsFn = ImGui_ImplGlfw_RenderDrawLists; // Alternatively you can set this to NULL and call ImGui_GetDrawData() after ImGui_Render() to get the same ImDrawData pointer.
  io->SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
  io->GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
#ifdef _WIN32
  io->ImeWindowHandle = glfwGetWin32Window(g_Window);
#endif
  if (install_callbacks) {
    glfwSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback);
    glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
    glfwSetKeyCallback(window, ImGui_ImplGlFw_KeyCallback);
    glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
  }
  return true;
}
void ImGui_ImplGlfw_Shutdown()
{
  ImGui_ImplGlfw_InvalidateDeviceObjects();
  ImGui_Shutdown();
}
void ImGui_ImplGlfw_NewFrame()
{
  if (!g_FontTexture) {
    ImGui_ImplGlfw_CreateDeviceObjects();
  }
  ImGuiIO* io = ImGui_GetIO();
  // Setup display size (every frame to accommodate for window resizing)
  int w, h;
  int display_w, display_h;
  glfwGetWindowSize(g_Window, &w, &h);
  glfwGetFramebufferSize(g_Window, &display_w, &display_h);
  io->DisplaySize = fVec2((float)w, (float)h);
  io->DisplayFramebufferScale = fVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);
  // Setup time step
  double current_time = glfwGetTime();
  io->DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
  g_Time = current_time;
  // Setup inputs
  // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
  if (glfwGetWindowAttrib(g_Window, GLFW_FOCUSED)) {
    double mouse_x, mouse_y;
    glfwGetCursorPos(g_Window, &mouse_x, &mouse_y);
    io->MousePos = fVec2((float)mouse_x, (float)mouse_y); // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
  }
  else {
    io->MousePos = fVec2(-1, -1);
  }
  for (int i = 0; i < 3; i++) {
    io->MouseDown[i] = g_MousePressed[i] || glfwGetMouseButton(g_Window, i) != 0; // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
    g_MousePressed[i] = false;
  }
  io->MouseWheel = g_MouseWheel;
  g_MouseWheel = 0.0f;
  // Hide OS mouse cursor if ImGui is drawing it
  glfwSetInputMode(g_Window, GLFW_CURSOR, io->MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
  // Start the frame
  ImGui_NewFrame();
}
// ImGui - standalone example application for Glfw + OpenGL 2, using fixed pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}
int test_imgui_opengl2(int, char**)
{
  // Setup window
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    return 1;
  }
  GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL2 example", NULL, NULL);
  glfwMakeContextCurrent(window);
  // Setup ImGui binding
  ImGui_ImplGlfw_Init(window, true);
  // Load Fonts
  // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
  //ImGuiIO* io = ImGui_GetIO();
  //io->Fonts->AddFontDefault();
  //io->Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
  //io->Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
  //io->Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
  //io->Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
  //io->Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io->Fonts->GetGlyphRangesJapanese());
  bool show_main_window = true;
  bool show_test_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImColor(114, 144, 154);
  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplGlfw_NewFrame();
    // 1. Show a simple window
    // Tip: if we don't call ImGui_Begin()/ImGui_End() the widgets appears in a window automatically called "Debug"
    {
      static float f = 0.0f;
      ImGui_Begin("Main Window", &show_main_window, 0);
      ImGui_Text("Hello, world!");
      ImGui_SliderFloat("float", &f, 0.0f, 1.0f);
      ImGui_ColorEdit3("clear color", (float*)&clear_color);
      if (ImGui_Button("Test Window")) {
        show_test_window ^= 1;
      }
      if (ImGui_Button("Another Window")) {
        show_another_window ^= 1;
      }
      ImGui_Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui_GetIO()->Framerate, ImGui_GetIO()->Framerate);
      ImGui_End();
    }
    // 2. Show another simple window, this time using an explicit Begin/End pair
    if (show_another_window) {
      ImGui_SetNextWindowSize(fVec2(200, 100), ImGuiSetCond_FirstUseEver);
      ImGui_Begin("Another Window", &show_another_window, 0);
      ImGui_Text("Hello");
      ImGui_End();
    }
    // 3. Show the ImGui test window. Most of the sample code is in ImGui_ShowTestWindow()
    if (show_test_window) {
      ImGui_SetNextWindowPos(fVec2(650, 20), ImGuiSetCond_FirstUseEver);
      ImGui_ShowTestWindow(&show_test_window);
    }
    // Rendering
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_Render();
    glfwSwapBuffers(window);
  }
  // Cleanup
  ImGui_ImplGlfw_Shutdown();
  glfwTerminate();
  return 0;
}


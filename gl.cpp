#include "gl.h"
#include <windowsx.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

static const int TextBase = 1000;

GL::GL(Frame* parent)
  : WindowFrame(parent)
  , hDC(NULL)
  , hRC(NULL)
  , hFont(NULL)
  , active(false)
  , quat(0, 0, 0, 1)
  , dist(10)
  , dragX(0)
  , dragY(0)
  , drag(false)
  , northo(0)
  , color(0)
  , perspective(true)
{
  static std::string regClass;
  if (WNDCLASSEX* wcx = createclass("GlViewClass")) {
    wcx->hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassEx(wcx);
  }
  create("", WS_CHILD | WS_VISIBLE, WS_EX_CLIENTEDGE);

  typedef Image::Format Fmt;
  PIXELFORMATDESCRIPTOR pfd;
  memset(&pfd, 0, sizeof pfd);
  pfd.nSize = sizeof pfd;
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = sizeof(Fmt::color_t) * 8;
  pfd.cRedBits = Fmt::red::bits;
  pfd.cRedShift = Fmt::red::shift;
  pfd.cGreenBits = Fmt::green::bits;
  pfd.cGreenShift = Fmt::green::shift;
  pfd.cBlueBits = Fmt::blue::bits;
  pfd.cBlueShift = Fmt::blue::shift;
  pfd.cDepthBits = 16;
  pfd.cStencilBits = 8;

  GLuint pixelFormat;
  hDC = GetDC(hWnd);
  pixelFormat = ChoosePixelFormat(hDC, &pfd);
  if (!pixelFormat) throw Exception("failed to choose pixel format");
  if (!SetPixelFormat(hDC, pixelFormat, &pfd)) throw Exception("failed to set pixel format");

  hRC = wglCreateContext(hDC);
  if (!hRC) throw Exception("failed to create GL context");
  if (!wglMakeCurrent(hDC, hRC)) throw Exception("failed to select GL context");

  glClearColor(0, 0, 1, 0);
  glClearDepth(1);
  glClearStencil(0);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.01f);
  glEnable(GL_BLEND);
  glEnable(GL_NORMALIZE);
  glDepthFunc(GL_LEQUAL);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glEnable(GL_CULL_FACE);

  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);

  hFont = CreateFont(-MulDiv(10, GetDeviceCaps(hDC, LOGPIXELSY), 72),
    0, 0, 0, FW_LIGHT, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Georgia");
  SelectObject(hDC, hFont);
  if (wglUseFontBitmaps(hDC, 0, 256, TextBase) == FALSE) throw Exception("failed to create font bitmaps");
  for (size_t i = 0; i < 256; ++i) lgen.set(i);
  SetBkMode(hDC, TRANSPARENT);
  wglMakeCurrent(NULL, NULL);

  ShowWindow(hWnd, SW_SHOW);
}
GL::~GL() {
  if (hRC) wglDeleteContext(hRC);
  if (hFont) DeleteObject(hFont);
  if (hWnd && hDC) ReleaseDC(hWnd, hDC);
}

void GL::render() {
  wglMakeCurrent(hDC, hRC);

  float _r = (color & 0xFF) / 255.0f, _g = ((color >> 8) & 0xFF) / 255.0f,
    _b = ((color >> 16) & 0xFF) / 255.0f;
  glClearColor(_r, _g, _b, 0);

  RECT rc;
  GetClientRect(hWnd, &rc);
  glViewport(0, 0, rc.right, rc.bottom);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (perspective) {
    gluPerspective(90, 1.0 * rc.right / rc.bottom, 1.0f, 150.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    static Matrix mfix(0, 1, 0, 0,
                        0, 0, 1, 0,
                        1, 0, 0, 0,
                        0, 0, 0, 1);
    glMultMatrixf((Matrix::translate(0, 0, -dist) * quat.matrix() * mfix).transposed());
  } else {
    double hdist = dist * rc.right / rc.bottom;
    glOrtho(-hdist, hdist, -dist, dist, -10 * dist, 10 * dist);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    static Matrix mfix(0, 1, 0, 0,
                       0, 0, 1, 0,
                       1, 0, 0, 0,
                       0, 0, 0, 1);
    glMultMatrixf((quat.matrix() * mfix).transposed());
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3d(1, 1, 1);
  active = true;

  doRender();
  //uint32 time = GetTickCount();
  //while (frames.size() > 1 && time - frames[1] > 1000) {
  //  frames.pop_front();
  //}
  //if (!frames.empty() && time > frames.front()) {
  //  char buf[256];
  //  sprintf(buf, "FPS: %.1lf", 1000.0 * frames.size() / (time - frames.front()));
  //  glColor3d(1 - _r, 1 - _g, 1 - _b);
  //  text(10, 10, buf);
  //}
  //frames.push_back(time);

  active = false;
  glFlush();
  SwapBuffers(hDC);
  wglMakeCurrent(NULL, NULL);
}

void GL::startOrtho() {
  if (!northo++) {
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    RECT rc;
    GetClientRect(hWnd, &rc);
    glOrtho(0, rc.right, rc.bottom, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
  }
}
void GL::endOrtho() {
  if (!--northo) {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
  }
}
void GL::text(int x, int y, wchar_t const* str) {
  size_t length = wcslen(str);
  for (size_t i = 0; i < length; ++i) {
    uint16 chr = str[i];
    if (!lgen.test(chr)) {
      wglUseFontBitmapsW(hDC, chr, 1, TextBase + chr);
      lgen.set(chr);
    }
  }
  startOrtho();
  glRasterPos2i(x, y + 2);
  glListBase(TextBase);
  glCallLists(length, GL_UNSIGNED_SHORT, str);
  endOrtho();
}
void GL::text(int x, int y, char const* str) {
  size_t length = strlen(str);
  startOrtho();
  glRasterPos2i(x, y + 2);
  glListBase(TextBase);
  glCallLists(length, GL_UNSIGNED_BYTE, str);
  endOrtho();
}
void GL::text(Vector const& v, char const* str) {
  size_t length = strlen(str);
  glRasterPos3fv(v);
  glListBase(TextBase);
  glCallLists(length, GL_UNSIGNED_BYTE, str);
}

uint32 GL::genTexture(Image const& img) {
  if (!active) wglMakeCurrent(hDC, hRC);

  uint32 id;
  glGenTextures(1, (GLuint*) &id);
  glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, img.width(), img.height(), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.bits());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glPopAttrib();

  if (!active) wglMakeCurrent(NULL, NULL);

  return id;
}

static float toSphere(float r, float x, float y) {
  static const float s2 = 1.0f / sqrt(2.0f);
  float d = sqrt(x * x + y * y);
  if (d < r * s2) {
    return sqrt(r * r - d * d);
  } else {
    return r * r * 0.5f / d;
  }
}

LRESULT GL::onMessage(uint32 uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
  case WM_LBUTTONDOWN:
    SetFocus(hWnd);
    dragX = GET_X_LPARAM(lParam);
    dragY = GET_Y_LPARAM(lParam);
    drag = true;
    break;
  case WM_MOUSEMOVE:
    if (wParam & MK_LBUTTON) {
      int x = GET_X_LPARAM(lParam);
      int y = GET_Y_LPARAM(lParam);
      if (drag && (x != dragX || y != dragY)) {
        const float tbSize = 0.7f;
        RECT rc;
        GetClientRect(hWnd, &rc);
        float x0 = float(rc.right) / 2, y0 = float(rc.bottom) / 2;
        float ws = float(std::min(rc.right, rc.bottom)) / 2;
        float x1 = (float(dragX) - x0) / ws, x2 = (float(x) - x0) / ws,
              y1 = (y0 - float(dragY)) / ws, y2 = (y0 - float(y)) / ws;
        float z1 = toSphere(tbSize, x1, y1), z2 = toSphere(tbSize, x2, y2);
        Vector p1(x1, y1, z1);
        Vector p2(x2, y2, z2);
        Vector dir = p1 ^ p2;
        dir.normalize();
        float dist = (p1 - p2).length() / (2.0f * tbSize);
        if (dist > 1.0f) dist = 1.0f;
        if (dist < -1.0f) dist = -1.0f;
        quat = Quaternion(2.0f * asin(dist), dir) * quat;
        quat.normalize();
      }
      dragX = x;
      dragY = y;
      drag = true;
    }
    break;
  case WM_KEYDOWN:
    onKey(wParam);
    break;
  case WM_LBUTTONUP:
    drag = false;
    break;
  case WM_MOUSEWHEEL:
    dist /= pow(1.2, GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
    break;
  default:
    return M_UNHANDLED;
  }
  return 0;
}

struct Sphere {
public:
  static void draw(int mask) {
    instance().doDraw(mask);
  }
  static void section(Vector const& offset) {
    instance().doSection(offset);
  }
private:
  static const int Lat = 16, Lon = 32;
  Vector points[Lat + 1][Lon + 1];
  Sphere() {
    for (int lat = 0; lat <= Lat; ++lat) {
      for (int lon = 0; lon <= Lon; ++lon) {
        float alat = (float(lat) / Lat - 0.5f) * M_PI;
        float alon = (float(lon) / Lon) * M_PI * 2;
        float clat = cos(alat);
        points[lat][lon] = Vector(cos(alon) * clat, sin(alon) * clat, sin(alat));
      }
    }
  }
  static Sphere instance() {
    static Sphere sphere;
    return sphere;
  }
  void doDraw(int mask);
  void doSection(Vector const& offset);
};
void Sphere::doDraw(int mask) {
  if (mask & 1) {
    for (int i = Lat / 2; i < Lat - 1; ++i) {
      glBegin(GL_TRIANGLE_STRIP);
      for (int j = 0; j <= Lon; ++j) {
        glNormal3fv(points[i + 1][j]);
        glVertex3fv(points[i + 1][j]);
        glNormal3fv(points[i][j]);
        glVertex3fv(points[i][j]);
      }
      glEnd();
    }
    glBegin(GL_TRIANGLE_FAN);
    glNormal3fv(points[Lat][0]);
    glVertex3fv(points[Lat][0]);
    for (int j = 0; j <= Lon; ++j) {
      glNormal3fv(points[Lat - 1][j]);
      glVertex3fv(points[Lat - 1][j]);
    }
    glEnd();
  }
  if (mask & 2) {
    for (int i = 1; i < Lat / 2; ++i) {
      glBegin(GL_TRIANGLE_STRIP);
      for (int j = 0; j <= Lon; ++j) {
        glNormal3fv(points[i + 1][j]);
        glVertex3fv(points[i + 1][j]);
        glNormal3fv(points[i][j]);
        glVertex3fv(points[i][j]);
      }
      glEnd();
    }
    glBegin(GL_TRIANGLE_FAN);
    glNormal3fv(points[0][0]);
    glVertex3fv(points[0][0]);
    for (int j = Lon; j >= 0; --j) {
      glNormal3fv(points[1][j]);
      glVertex3fv(points[1][j]);
    }
    glEnd();
  }
}
void Sphere::doSection(Vector const& offset) {
  glBegin(GL_TRIANGLE_STRIP);
  for (int j = 0; j <= Lon; ++j) {
    glNormal3fv(points[Lat / 2][j]);
    glVertex3fv(points[Lat / 2][j] + offset);
    glNormal3fv(points[Lat / 2][j]);
    glVertex3fv(points[Lat / 2][j]);
  }
  glEnd();
}

void GL::sphere(Vector const& c, float r) {
  glPushMatrix();
  glTranslatef(c.x, c.y, c.z);
  glScalef(r, r, r);
  Sphere::draw(3);
  glPopMatrix();
}
void GL::capsule(Vector const& a, Vector const& b, float r) {
  Vector z = (b - a).normalized();
  Vector x = z.perp().normalized();
  Vector y = z ^ x;
  float dist = (b - a).length();
  x *= r;
  y *= r;
  z *= r;
  glPushMatrix();
  glMultMatrixf(Matrix::rows(x, y, z, a));
  Sphere::draw(2);
  Sphere::section(Vector(0, 0, dist / r));
  glPopMatrix();
  glPushMatrix();
  glMultMatrixf(Matrix::rows(x, y, z, b));
  Sphere::draw(1);
  glPopMatrix();
}

void GL::trackable(Vector const& v, float r, char const* str) {
  trackables.emplace_back();
  auto& tr = trackables.back();
  tr.pos = v;
  tr.radius = r;
  tr.text = str;
}
void GL::beginTrackables() {
  trackables.clear();
}
void GL::endTrackables() {
  RECT rc;
  POINT pt;
  GetClientRect(hWnd, &rc);
  GetCursorPos(&pt);
  ScreenToClient(hWnd, &pt);
  float width = float(rc.right), height = float(rc.bottom);
  float cX = 2 * float(pt.x) / width - 1;
  float cY = 1 - 2 * float(pt.y) / height;
  Matrix transform, trProj;
  glGetFloatv(GL_PROJECTION_MATRIX, trProj);
  glGetFloatv(GL_MODELVIEW_MATRIX, transform);
  transform.transpose().invert();
  Vector dir(cX / trProj.m00, cY / trProj.m11, -1);
  dir = transform.rotate(dir);
  Vector eye = transform * Vector(0, 0, 0);
  dir.normalize();

  Trackable* closest = nullptr;
  float closestDistance;
  for (auto& tr : trackables) {
    Vector to = tr.pos - eye;
    float dist = to & dir;
    float perp = (to - dir * dist).length();
    if (perp > tr.radius - 1e-6f) continue;
    float delta = sqrt(tr.radius * tr.radius - perp * perp);
    float distance = std::max(0.0f, dist - delta);
    if (!closest || distance < closestDistance) {
      closest = &tr;
      closestDistance = distance;
    }
  }

  for (auto& tr : trackables) {
    std::string str = tr.text;
    if (&tr == closest) {
      glColor3d(1, 0, 0);
      str += fmtstring(" (%.3lf %.3lf %.3lf)", tr.pos.x, tr.pos.y, tr.pos.z);
    } else {
      glColor3d(1, 1, 1);
    }
    sphere(tr.pos, tr.radius);
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    if (&tr == closest) {
      glDisable(GL_DEPTH_TEST);
    }
    text(tr.pos, str.c_str());
    glPopAttrib();
  }
}

Image GL::capture() {
  if (!active) wglMakeCurrent(hDC, hRC);
  RECT rc;
  GetClientRect(hWnd, &rc);
  Image img(rc.right, rc.bottom);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadBuffer(GL_FRONT);
  std::vector<uint8> buf(rc.right * rc.bottom * 3);
  glReadPixels(0, 0, rc.right, rc.bottom, GL_RGB, GL_UNSIGNED_BYTE, &buf[0]);
  size_t index = 0;
  for (int y = rc.bottom - 1; y >= 0; --y) {
    for (int x = 0; x < rc.right; ++x) {
      int index = (x + (rc.bottom - y - 1) * rc.right) * 3;
      img.mutable_bits()[x + y * rc.right] = Image::Format::color(buf[index], buf[index + 1], buf[index + 2]);
    }
  }
  if (!active) wglMakeCurrent(NULL, NULL);
  return img;
}

static inline ImVec2 operator*(const ImVec2& lhs, const float rhs)
{
  return fVec2(lhs.x * rhs, lhs.y * rhs);
}
static inline ImVec2 operator/(const ImVec2& lhs, const float rhs)
{
  return fVec2(lhs.x / rhs, lhs.y / rhs);
}
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
  return fVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
  return fVec2(lhs.x - rhs.x, lhs.y - rhs.y);
}
static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs)
{
  return fVec2(lhs.x * rhs.x, lhs.y * rhs.y);
}
static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs)
{
  return fVec2(lhs.x / rhs.x, lhs.y / rhs.y);
}
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs)
{
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  return lhs;
}
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs)
{
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs)
{
  lhs.x *= rhs;
  lhs.y *= rhs;
  return lhs;
}
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs)
{
  lhs.x /= rhs;
  lhs.y /= rhs;
  return lhs;
}
static inline ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs)
{
  return fVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

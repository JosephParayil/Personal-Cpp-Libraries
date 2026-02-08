#pragma once

/*
vec4 and mat4 for operations in 3D space
*/

#include <cmath>
#include <cstdint>
#include <string>


struct vec4 {
    float x, y, z, w;

    
    constexpr vec4() : x(0), y(0), z(0), w(1) {}
    constexpr vec4(float x_, float y_, float z_)
        : x(x_), y(y_), z(z_), w(1) {}
    constexpr vec4(float x_, float y_, float z_, float w_)
        : x(x_), y(y_), z(z_), w(w_) {}

    float magnitude() const {
        return sqrt(x*x+ y*y+z*z);
    }

    std::string string() const {
        return "(" + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z)  + ")";
    }


    vec4 operator+(const vec4& other) {
        return vec4(x+other.x, y+other.y, z+other.z);
    }

    vec4& operator+=(const vec4& other) {
        x+=other.x;
        y+=other.y;
        z+=other.z;

        return *this;
    }

    vec4 operator-(const vec4& other) {
        return vec4(x-other.x, y-other.y, z-other.z);
    }

    vec4& operator-=(const vec4& other) {
        x-=other.x;
        y-=other.y;
        z-=other.z;

        return *this;
    }


    vec4 operator*(float number) {
        return vec4(x*number, y*number, z*number);
    }

    vec4& operator*=(float number) {
        x *= number;
        y *= number;
        z *= number;

        return *this;
    }

    vec4 operator/(float number) {
        return (*this)*(1.0f/number);
    }

    vec4 operator/=(float number) {
        (*this) *= (1.0f/number);
        return *this;
    }


    vec4 unit() {
        float r = magnitude();
        return vec4(x/r, y/r, z/r);
    }

    vec4& normalize() {
        float r = magnitude();
        x/=r;
        y/=r;
        z/=r;

        return *this;
    }

    vec4 cross(const vec4& b) const {
        return vec4(
            y * b.z - z * b.y,
            z * b.x - x * b.z,
            x * b.y - y * b.x
        );
    }
};




struct alignas(16) mat4 {
    // Column-major: m[col * 4 + row]
    float m[16]{};

    mat4& nullify_rotation() {
        m[0] = m[5] = m[10] = m[15] = 1.0f;

        m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = 0.0f;

        return *this;
    }

    vec4 get_position() const {
        return vec4(m[12], m[13], m[14], 1.0f);
    }


    void cancel_roll() {
        // World up
        const vec4 worldUp(0.0f, 1.0f, 0.0f, 0.0f);

        // Extract forward (column 2)
        vec4 forward(
            m[8],
            m[9],
            m[10],
            0.0f
        );

        // Normalize forward
        float fLen = std::sqrt(forward.x*forward.x +
                                forward.y*forward.y +
                                forward.z*forward.z);
        if (fLen < 1e-6f) return;
        forward.x /= fLen;
        forward.y /= fLen;
        forward.z /= fLen;

        // Recompute right = worldUp × forward
        vec4 right(
            worldUp.y * forward.z - worldUp.z * forward.y,
            worldUp.z * forward.x - worldUp.x * forward.z,
            worldUp.x * forward.y - worldUp.y * forward.x,
            0.0f
        );

        float rLen = std::sqrt(right.x*right.x +
                                right.y*right.y +
                                right.z*right.z);
        if (rLen < 1e-6f) return;
        right.x /= rLen;
        right.y /= rLen;
        right.z /= rLen;

        // Recompute up = forward × right
        vec4 up(
            forward.y * right.z - forward.z * right.y,
            forward.z * right.x - forward.x * right.z,
            forward.x * right.y - forward.y * right.x,
            0.0f
        );

        // Write axes back (columns)
        m[0] = right.x;   m[1] = right.y;   m[2] = right.z;
        m[4] = up.x;      m[5] = up.y;      m[6] = up.z;
        m[8] = forward.x; m[9] = forward.y; m[10] = forward.z;
    }


    // = Static 'Constructor' Methods =:

    static mat4 identity() {
        mat4 r{};
        r.m[0]  = 1.0f;
        r.m[5]  = 1.0f;
        r.m[10] = 1.0f;
        r.m[15] = 1.0f;
        return r;
    }

    static mat4 translation(float x, float y, float z) {
        mat4 r = identity();
        r.m[12] = x;
        r.m[13] = y;
        r.m[14] = z;
        return r;
    }

    //Yaw
    static mat4 rotation_y(float radians) {
        mat4 r = identity();
        float c = std::cos(radians);
        float s = std::sin(radians);

        r.m[0]  =  c;
        r.m[8]  =  s;
        r.m[2]  = -s;
        r.m[10] =  c;
        return r;
    }


    //Pitch
    static mat4 rotation_x(float radians) {
        mat4 r = identity();
        float c = std::cos(radians);
        float s = std::sin(radians);

        r.m[5]  =  c;  r.m[9]  = -s;
        r.m[6]  =  s;  r.m[10] =  c;
        return r;
    }

    //Roll
    static mat4 rotation_z(float radians) {
        mat4 r = identity();
        float c = std::cos(radians);
        float s = std::sin(radians);

        r.m[0] =  c;  r.m[4] = -s;
        r.m[1] =  s;  r.m[5] =  c;
        return r;
    }

    // = Operations =:

    /// Matrix * Vector
    vec4 operator*(const vec4& v) {
        return {
            m[0]  * v.x + m[4]  * v.y + m[8]  * v.z + m[12] * v.w,
            m[1]  * v.x + m[5]  * v.y + m[9]  * v.z + m[13] * v.w,
            m[2]  * v.x + m[6]  * v.y + m[10] * v.z + m[14] * v.w,
            m[3]  * v.x + m[7]  * v.y + m[11] * v.z + m[15] * v.w
        };
    }

    /// Matrix * Matrix
    mat4 operator*(const mat4& B) {
        mat4 R{};

        for (int c = 0; c < 4; ++c) {
            for (int r = 0; r < 4; ++r) {
                R.m[c * 4 + r] =
                    m[0 * 4 + r] * B.m[c * 4 + 0] +
                    m[1 * 4 + r] * B.m[c * 4 + 1] +
                    m[2 * 4 + r] * B.m[c * 4 + 2] +
                    m[3 * 4 + r] * B.m[c * 4 + 3];
            }
        }
        return R;
    }


    // -Matrix
    mat4 inverse_rigid() const {
        mat4 inv{};
        // transpose the rotation part
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c)
                inv.m[c*4 + r] = m[r*4 + c]; // transpose 3x3 rotation

        // invert translation
        inv.m[12] = -(inv.m[0]*m[12] + inv.m[4]*m[13] + inv.m[8]*m[14]);
        inv.m[13] = -(inv.m[1]*m[12] + inv.m[5]*m[13] + inv.m[9]*m[14]);
        inv.m[14] = -(inv.m[2]*m[12] + inv.m[6]*m[13] + inv.m[10]*m[14]);
        inv.m[15] = 1.0f;

        return inv;
    }


};


inline bool operator==(const vec4& a, const vec4& b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

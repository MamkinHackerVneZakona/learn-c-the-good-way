//
// Created by Ruslan Feizerakhmanov on 2019-07-22.
//

#ifndef LEARN_C_THE_GOOD_WAY_VEC_H
#define LEARN_C_THE_GOOD_WAY_VEC_H

struct Vec2f{
    float x;
    float y;
};

struct Vec3f{
    float x;
    float y;
    float z;
};

Vec2f add(Vec2f a, Vec2f b);

Vec2f sub(Vec2f a, Vec2f b);

Vec2f mult(Vec2f a, float k);

Vec2f mult(Vec2f a, Vec2f b);

float dot(Vec2f a, Vec2f b);

float mag(Vec2f);

Vec2f ortho(Vec2f a);

#endif //LEARN_C_THE_GOOD_WAY_VEC_H

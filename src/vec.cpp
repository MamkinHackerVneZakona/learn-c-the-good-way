//
// Created by Ruslan Feizerakhmanov on 2019-07-23.
//

#include "vec.h"

Vec2f add(Vec2f a, Vec2f b){
    Vec2f c;
    c.x = a.x + b.x;
    c.y = a.y + b.y;

    return c;
}

Vec2f sub(Vec2f a, Vec2f b){
    Vec2f c;
    c.x = a.x - b.x;
    c.y = a.y - b.y;

    return c;
}

Vec2f mult(Vec2f a, float k){
    Vec2f c;
    c.x = a.x * k;
    c.y = a.y * k;

    return c;
}

float dot(Vec2f a, Vec2f b){
    return  a.x * b.x + a.y * b.y;
}

Vec2f ortho(Vec2f a){
    Vec2f b;
    b.x = -a.y;
    b.y = a.x;

    return b;
}
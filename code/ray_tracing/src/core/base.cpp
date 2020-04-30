#include<core.h>

ray_tracing::ray::ray(const vec::vec3 &origin,const vec::vec3 &direction):origin_(origin),direction_(direction.normalized()){}
ray_tracing::ray::ray(const ray &r):origin_(r.origin()),direction_(r.direction()){}
const vec::vec3& ray_tracing::ray::origin()const{return origin_;}
const vec::vec3& ray_tracing::ray::direction()const{return direction_;}
vec::vec3 ray_tracing::ray::go(const vec::real &t)const{return origin_+direction_*t;}

ray_tracing::ray ray_tracing::camera::get_ray(vec::real u,vec::real v){return ray(vec::vec3(0,0,0),vec::vec3(1,2*v-1,0.5-u));}
ray_tracing::ray ray_tracing::camera::get_ray(vec::vec2 p){return ray(vec::vec3(0,0,0),vec::vec3(1,2*p.y()-1,0.5-p.x()));}
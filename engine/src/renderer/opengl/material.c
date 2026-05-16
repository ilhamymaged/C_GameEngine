#include "material.h"

void init_material(material* mat, shader* shader, texture* albedo, vec3 color, b8 use_albedo_map) {
    mat->shader = shader;
    mat->albedo = albedo;
    mat->color = color;
    mat->use_albedo_map = use_albedo_map;
}

void use_material(material* mat) {
    use_shader(mat->shader);
    if (mat->use_albedo_map && mat->albedo) {
        use_texture(mat->albedo, 0);
        set_shader_int(mat->shader, "uAlbedo", 0);
        set_shader_int(mat->shader, "uUseAlbedo", 1);
    } else {
        set_shader_int(mat->shader, "uUseAlbedo", 0);
    }
    set_shader_vec3(mat->shader, "uColor", mat->color);
}

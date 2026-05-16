#include "ui.h"

b8 ui_init(ui_context* ctx, u32 width, u32 height) {
    ctx->width = width;
    ctx->height = height;

    if (!create_shader(&ctx->shader,
        "../engine/shaders/ui.vs",
        "../engine/shaders/ui.frag")) {
        return FALSE;
    }

    quad_desc desc = {0};
    desc.color = (vec3){0,1,1};

    init_model(&ctx->quad, NULL, QUAD, &desc);
    return TRUE;
}


void ui_begin(ui_context* ctx) {
    use_shader(&ctx->shader);

    mat4 ortho = mat4_ortho(
        0.0f, (float)ctx->width,
        (float)ctx->height, 0.0f,
        -1.0f, 1.0f
    );

    set_shader_mat4(&ctx->shader, "uProjection", ortho);

}

void ui_draw_rect(ui_context* ctx, float x, float y, float w, float h) {

    mat4 model = get_model_matrix((vec3){ x + w * 0.5f, y + h * 0.5f, 0 }, (vec3){ w, h, 1 }, (vec3){ 0, 0, 0 });
    set_shader_mat4(&ctx->shader, "uModel", model);
    draw_model(&ctx->quad);
}

void ui_shutdown(ui_context* ctx) {
    destroy_model(&ctx->quad);
    destroy_shader(&ctx->shader);
}
